//////////////////////////////////////////////////////////////////////////
// tcp_session.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Created By: FJW
// Last modification: 2015/05/23
// Copyright (c) 2013-2015 Vistek 
//////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "tcp_session.h"
#include "boost/thread.hpp"
#define MAX_CONCURENCY 0
#define DEFAULT_COMM_THREAD_STACK_SIZE 0x20000

tcp_session::tcp_session(bool bUseSharedService, int timeout_ms, int recv_buffer_size/* = 0x1000*/, int send_buffer_size/* = 0x1000*/)
	: m_use_shared_service(bUseSharedService), m_client_service(bUseSharedService ? _s_shared_service : make_shared<boost::asio::io_service>()),
	m_socket(*m_client_service), m_timer(0)
	, m_strand(m_socket.get_io_service()), m_timeout_ms(timeout_ms), m_is_client(true), m_recv_buff_size(recv_buffer_size), m_send_buff_size(send_buffer_size)
{
	m_client_work = make_shared<boost::asio::io_service::work>(*m_client_service);
	if (m_use_shared_service)
	{
		s_init_thread_pool(MAX_CONCURENCY);
	}
	else
	{
		auto svc = m_client_service;
#ifdef USE_BOOST_THREAD
		boost::thread::attributes attr;
		attr.set_stack_size(DEFAULT_COMM_THREAD_STACK_SIZE);
		boost::thread(attr, [svc]()
#else
		thread([svc]()
#endif
		{
			boost_error_code ec;
			svc->run(ec);
#ifdef _DEBUG
			cout << "tcp_session::m_own_service has been terminated" << endl;
#endif
		}).detach();
	}
}
tcp_session::tcp_session(const string& ip, int port, bool bUseSharedService, int timeout_ms, int recv_buffer_size/* = 0x1000 */, int send_buffer_size/* = 0x1000 */)
	: m_use_shared_service(bUseSharedService), m_client_service(bUseSharedService ? _s_shared_service : make_shared<boost::asio::io_service>()),
	m_socket(*m_client_service), m_remote_ep(boost::asio::ip::address::from_string(ip), port), m_timer(new coro_timer(m_socket.get_io_service())), m_strand(m_socket.get_io_service()), m_timeout_ms(timeout_ms), m_is_client(true), m_recv_buff_size(recv_buffer_size), m_send_buff_size(send_buffer_size)
{
	m_client_work = make_shared<boost::asio::io_service::work>(*m_client_service);
	if (m_use_shared_service)
	{
		s_init_thread_pool(MAX_CONCURENCY);
	}
	else
	{
		auto svc = m_client_service;
#ifdef USE_BOOST_THREAD
		boost::thread::attributes attr;
		attr.set_stack_size(DEFAULT_COMM_THREAD_STACK_SIZE);
		boost::thread(attr, [svc]()
#else
		thread([svc]()
#endif
		{
			boost_error_code ec; 
			boost::scoped_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(*svc));
			svc->run(ec);
#ifdef _DEBUG
			cout << "tcp_session::m_own_service has been terminated" << endl;
#endif
		}).detach();
	}
}

tcp_session::tcp_session(tcp::socket sock, int timeout_ms, int recv_buffer_size/* = 0x1000 */, int send_buffer_size/* = 0x1000 */)
	: m_socket(move(sock)), m_remote_ep(m_socket.remote_endpoint()), m_timer(new coro_timer(m_socket.get_io_service()))
	, m_strand(m_socket.get_io_service()), m_timeout_ms(timeout_ms), m_is_client(false), m_recv_buff_size(recv_buffer_size), m_send_buff_size(send_buffer_size)
{
}
tcp_session::~tcp_session()
{
	//_stop_async();
#ifdef _DEBUG
	vvprint_i("~~~~~~~~~~~~~~~~ tcp_session terminated. is_client = " << m_is_client);
#endif
}


void tcp_session::_spawn_handle_timeout(const coro_timer_ptr& ptimer, const coro_promise_ptr& prom)
{
	auto self = shared_from_this();
	boost::asio::spawn(m_strand,
		[this, self, ptimer, prom](boost::asio::yield_context yield)
	{
		while (m_socket.is_open())
		{
			boost::system::error_code ec;
			ptimer->async_wait(yield[ec]);
			
			if (ptimer->expires_from_now() <= chrono::milliseconds(0))
			{
				vvlog_e("timer timeout");
				m_socket.close(ec);
				if (prom != nullptr)
				{
					if (prom->get_future().valid())
					{
						prom->set_value(v_ERR_TIMEOUT);
					}
				}
			}
		}
	});
}

int tcp_session::connect_y(const string& content, boost::asio::yield_context yield)
{
	if (m_is_client)
	{
		boost::system::error_code ec;
		m_socket.async_connect(m_remote_ep, yield[ec]);
		if (ec)
		{
			_close();
			return v_ERR_BAD_CONNECTION;
		}
		if (m_socket.is_open())
		{
			resize_recv_buffer(m_recv_buff_size);
			resize_send_buffer(m_send_buff_size);
			set_no_delay(true);
		}
		if (content.length() > 0)
		{
			boost::asio::async_write(m_socket, boost::asio::buffer(content.data(), content.length()), yield[ec]);
			if (ec)
			{
				_close();
			}
		}
		return v_OK;
	}
	return v_ERR_Not_Supported;
}



int tcp_session::connect()
{
	/*auto self = shared_from_this();
	boost::asio::spawn(m_strand,
		[this, self](boost::asio::yield_context yield)
	{
		boost::system::error_code ec;
		boost::asio::steady_timer timer(m_service, chrono::milliseconds(m_timeout_ms));

		timer.async_wait(yield[ec]);
		if (timer.expires_from_now() <= chrono::milliseconds(0))
		{
			if (!m_socket.is_open())
			{
				m_socket.cancel(ec);
			}
		}
	});
	auto ar = m_socket.async_connect(m_remote_ep, boost::asio::use_future);
	ar.get();
	if (m_socket.is_open())
	{
		return v_OK;
	}*/
	int ec = connect_ex("");
	return ec;
}

int tcp_session::connect_ex(const string& content)
{
	if (m_is_client)
	{
		auto self = shared_from_this();
		int result = _run_sync_action([this, self, content](coro_promise_ptr prom, coro_timer_ptr ptimer, boost::asio::yield_context yield)
		{
			int result = connect_y(content, yield);
			if (ptimer != nullptr)
			{
				ptimer->cancel();
			}
			if (prom != nullptr)
			{
				prom->set_value(result);
			}
		}, m_timeout_ms);
		cout << "connect = " << (result == 0) << endl;
		
		return result;
	}
	return v_ERR_Not_Supported;
}

void tcp_session::s_init_thread_pool(int threadpool_size)
{
	static std::once_flag oc;
	std::call_once(oc, [&]()
	{
		auto svc = _s_shared_service;
		int default_size = std::thread::hardware_concurrency();
		//default_size = 1;
		int size = threadpool_size <= 0 ? default_size : threadpool_size;

		for (int i = 0; i < size; ++i)
		{
#ifdef USE_BOOST_THREAD
			boost::thread::attributes attr;
			attr.set_stack_size(DEFAULT_COMM_THREAD_STACK_SIZE);
			boost::thread(attr, [svc]()
#else
			thread([svc]()
#endif
			{
				boost::system::error_code ec;
				boost::scoped_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(*svc));
				svc->run(ec);
				cout << "tcp_session thread terminated. " << endl;
			}).detach();
		}
	});
}

void tcp_session::s_clean_thread_pool()
{
	static std::once_flag oc;
	std::call_once(oc, [&]()
	{
		_s_shared_service->stop();
	});
}
shared_ptr<boost::asio::io_service> tcp_session::_s_shared_service(new boost::asio::io_service);
