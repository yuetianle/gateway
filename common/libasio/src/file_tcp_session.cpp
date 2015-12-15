//////////////////////////////////////////////////////////////////////////
// file_tcp_session.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Created By: FJW
// Last modification: 2015/05/23
// Copyright (c) 2013-2015 Vistek 
// Usage: 
//////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "file_tcp_session.h"

std::mutex file_tcp_session::_s_mtx_path;

file_tcp_session::file_tcp_session(const string& ip, int port, bool bOwnService, int timeout_ms /*= 10000*/, int recv_buffer_size /*= 0x10000*/, int send_buffer_size /*= 0x1000*/)
	: tcp_session(ip, port, bOwnService, timeout_ms, recv_buffer_size, send_buffer_size)
{

}

file_tcp_session::file_tcp_session(tcp::socket sock, int timeout_ms /*= 10000*/, int recv_buffer_size /*= 0x1000*/, int send_buffer_size /*= 0x10000*/)
	: tcp_session(move(sock), timeout_ms, recv_buffer_size, send_buffer_size)
{

}

std::string file_tcp_session::_make_response(file_response_param param)
{
	string fn = get<0>(param);
	string mode = get<1>(param);

	pugi::xml_document doc;
	auto node = doc.append_child("file_response").append_child("file");
	node.append_attribute("name").set_value(fn.c_str());
	node.append_attribute("mode").set_value(mode.c_str());
	if (mode == "download")
	{
		int64_t file_size = get<2>(param);
		node.append_attribute("file_size").set_value(file_size);
	}
	else
	{
		bool accept = get<3>(param);
		node.append_attribute("allow").set_value(accept);
	}
	stringstream ss;
	doc.save(ss);
	string resp = ss.str();
	return move(resp);
}

file_tcp_session::file_response_param file_tcp_session::_parse_response(const string& resp)
{
	pugi::xml_document doc;
	doc.load_string(resp.c_str());
	auto query = doc.select_node("/file_response/file");
	auto xn = query.node();
	string name = xn.attribute("name").as_string();
	string mode = xn.attribute("mode").as_string();
	int64_t file_size = xn.attribute("file_size").as_llong();
	bool accept = xn.attribute("allow").as_bool();
	return make_tuple(name, mode, file_size, accept);
}

std::string file_tcp_session::_make_request(file_request_param param)
{
	string fn = get<0>(param);
	string mode = get<1>(param);

	pugi::xml_document doc;
	auto node = doc.append_child("file_request").append_child("file");
	node.append_attribute("name").set_value(fn.c_str());
	node.append_attribute("mode").set_value(mode.c_str());
	if (mode == "upload")
	{
		int64_t sz = get<2>(param);
		node.append_attribute("file_size").set_value(sz);
	}
	stringstream ss;
	doc.save(ss);
	string resp = ss.str();
	return move(resp);
}

file_tcp_session::file_request_param file_tcp_session::_parse_request(const string& req)
{
	pugi::xml_document doc;
	doc.load_string(req.c_str());
	auto result = doc.select_node("/file_request/file");
	auto xn = result.node();
	string fn = xn.attribute("name").as_string();
	string mode = xn.attribute("mode").as_string();
	int64_t file_size = xn.attribute("file_size").as_llong();
	return make_tuple(fn, mode, file_size);
}

void file_tcp_session::_send_file(const string& fn, coro_timer_ptr ptimer, boost::asio::yield_context yield)
{
	boost_error_code ec;
	int64_t total_size = fsys::file_size(fn, ec);
	if (total_size > 0)
	{
		vector<uint8_t> buffer(0x100000);
		fstream fs(fn, ios::in | ios::binary);

		while (total_size > 0 && fs.good())
		{
			fs.read((char*)buffer.data(), buffer.size());
			int sz = fs.gcount();
			if (sz > 0)
			{
				total_size -= sz;
			}
			if (ptimer)
			{
				ptimer->expires_from_now(std::chrono::milliseconds(m_timeout_ms), ec);
			}

			size_t sent_sz = boost::asio::async_write(m_socket, boost::asio::buffer(buffer.data(), sz), yield[ec]);
			if (ec)
			{
				_close();
			}
			else
			{
				cout << "sent size = " << sent_sz << endl;
			}
		}
	}
}

int file_tcp_session::_read_file(const string& fn, int64_t file_size, coro_timer_ptr ptimer, boost::asio::yield_context yield)
{
	if (file_size > 0)
	{
		auto fs = make_shared<fstream>(fn, ios::out | ios::binary);
		auto fz = make_shared<int64_t>(file_size);
		bool bFinish = false;
		boost::aligned_storage<0x8000> buff;
		while (*fz > 0)
		{
			boost_error_code ec;

			int sz = m_socket.async_read_some(boost::asio::buffer(buff.address(), buff.size), yield[ec]);
			if (!ec)
			{
				if (ptimer)
				{
					ptimer->expires_from_now(std::chrono::milliseconds(m_timeout_ms), ec);
				}
				if (sz > 0)
				{
					fs->write((char*)buff.address(), sz);
					*fz -= sz;
				}
			}
			else
			{
				cout << ec.message() << endl;
				vistek_logger::write_log_args(v_LOG_ERR, "tcp_file_session_err", true, "read_file() failed; ec: ", ec.value(), "; msg: ", ec.message());
				_close();
				break;
			}
			
#ifdef _DEBUG
			//vprint_i("got ", sz, "bytes; remain ", *fz, " bytes");
#endif
		}
		fs->close();
		if (*fz == 0)
		{
			vistek_logger::write_log_args(v_LOG_INFO, "tcp_file_session_info", true, "read_file: ", fn, " successfully");
			return v_OK;
		}
		else
		{
			vistek_logger::write_log_args(v_LOG_ERR, "tcp_file_session_bad_file", true, "read_file: ", fn, " failed because bad file operation. remain size: ", *fz);
			return v_ERR_Write_File_Failure;
		}

	}
	else
	{
		vistek_logger::write_log_args(v_LOG_ERR, "tcp_file_session_err", true, "read_file: ", fn, " does not exist");
		return v_ERR_FILE_NOT_FOUND;
	}
}

void file_tcp_session::_handle_request(string req, coro_timer_ptr ptimer, boost::asio::yield_context yield)
{
	_file_task = new file_task_info(_parse_request(req));
	if (_file_task == 0)
	{
		return;
	}
	boost_error_code ec;
	v_unique_lock(lk, _s_mtx_path);
	fsys::path p(_file_task->name);
	lk.unlock();
	if (!p.has_parent_path())
	{
		p = fsys::current_path() / p;
	}
	_file_task->name = p.string();
	if (fsys::exists(p, ec))
	{
		_file_task->size = fsys::file_size(p, ec);
	}
	string resp = "";

	if (_file_task->mode == "download")
	{
		if (_file_task->size > 0)
		{
			_file_task->buff = _make_response(make_tuple(_file_task->name, _file_task->mode, _file_task->size, true));
			boost::asio::async_write(m_socket, boost::asio::buffer(_file_task->buff.c_str(), _file_task->buff.size()), yield[ec]);
			if (ec)
			{
				_close();
			}
			_send_file(_file_task->name, ptimer, yield);
		}
		else
		{
			_file_task->buff = _make_response(make_tuple(_file_task->name, _file_task->mode, 0, false));
			boost::asio::async_write(m_socket, boost::asio::buffer(_file_task->buff.c_str(), _file_task->buff.size()), [this](boost_error_code ec, int size)
			{
				if (ec)
				{
					_close();
				}
			});
		}

	}
	else
	{
		_file_task->buff = _make_response(make_tuple(_file_task->name, _file_task->mode, _file_task->size, true));
		boost::asio::async_write(m_socket, boost::asio::buffer(_file_task->buff.c_str(), _file_task->buff.size()), yield[ec]);
	}
	if (ec)
	{
		_close();
	}
}

void file_tcp_session::wait_request()
{
	if (m_socket.is_open())
	{
		set_no_delay(true);
	}
	auto self = shared_from_this();
	boost::asio::spawn(m_strand,
		[this, self](boost::asio::yield_context yield)
	{
		boost_error_code ec;
		m_timer->expires_from_now(std::chrono::milliseconds(m_timeout_ms), ec);
		boost::aligned_storage<0x8000> buff;
		for (;;)
		{
			m_timer->expires_from_now(std::chrono::milliseconds(m_timeout_ms), ec);
			size_t sz = m_socket.async_read_some(boost::asio::buffer((char*)buff.address(), buff.size), yield[ec]);
			if (!ec)
			{
				if (sz > 0)
				{
					string req((char*)buff.address(), sz);
					_handle_request(move(req), m_timer, yield);
				}
			}
			else
			{
				vistek_logger::write_log_args(v_LOG_ERR, "tcp_file_session_err", true, "wait_request() failed; ec: ", ec.value(), "; msg: ", ec.message());
				_close();
				break;
			}
		}

	});
	_spawn_handle_timeout(m_timer, nullptr);
}

int file_tcp_session::read_file(const string& fn, const string& target)
{
	auto self = shared_from_this();
	int result = _run_sync_action([this, self, fn, target](const coro_promise_ptr& prom, const coro_timer_ptr& ptimer, boost::asio::yield_context yield)
	{
		try
		{

			string req = _make_request(make_tuple(fn, "download", 0));
			cout << "start downloading " << fn << endl;

			boost_error_code ec;
			boost::asio::async_write(m_socket, boost::asio::buffer(req.data(), req.length()), yield[ec]);
			if (!ec)
			{
				vector<char> buff(m_recv_buff_size);
				size_t sz = m_socket.async_read_some(boost::asio::buffer(buff.data(), buff.size()), yield[ec]);
				if (!ec)
				{
					if (sz > 0)
					{
						int64_t file_size;
						file_response_param frp = _parse_response(buff.data());
						tie(ignore, ignore, file_size, ignore) = frp;
						int result = _read_file(target, file_size, ptimer, yield);

						if (prom != nullptr)
						{
							prom->set_value(result);
						}
						ptimer->cancel();
						return;
					}
				}
			}
			_close();
// 			if (!prom)
// 			{
// 				prom->set_value(ec.value());
// 			}
		}
		catch (exception& e)
		{
			cout << e.what() << endl;
		}
		catch (std::error_code &e)
		{
			cout << e.value() << endl;
		}

	}, m_timeout_ms);
	return result;
	/*auto prom = make_shared<std::promise<int>>();
	auto self = shared_from_this();
	m_timer.expires_from_now(chrono::milliseconds(m_timeout_ms));
	boost::asio::spawn(m_strand, [this, self, fn, target, prom](boost::asio::yield_context yield)
	{
	v_unique_lock(lck, m_mtx_operation_result);
	m_timer.expires_from_now(chrono::milliseconds(m_timeout_ms));
	string req = _make_request(make_tuple(fn, "download", 0));
	cout << "start downloading " << fn << endl;
	boost_error_code ec;
	if (!m_socket.is_open())
	{
	_connect(req, yield);
	}
	else
	{
	boost::asio::async_write(m_socket, boost::asio::buffer(req.data(), req.length()), yield[ec]);
	}
	if (!m_socket.is_open())
	{
	m_operation_result = v_ERR_BAD_CONNECTION;
	return;
	}
	size_t sz = m_socket.async_read_some(boost::asio::buffer(_buff.data(), _buff.size()), yield[ec]);
	if (sz > 0)
	{
	int64_t file_size;
	file_response_param frp = _parse_response(_buff.data());
	tie(ignore, ignore, file_size, ignore) = frp;
	m_operation_result = _read_file(target, file_size, TODO,yield);
	m_timer.cancel(ec);
	if (prom != nullptr)
	{
	prom->set_value(m_operation_result);
	}
	}
	boost::system::error_code ignored_ec;
	});
	int result = prom->get_future().get();
	return result;*/
}

