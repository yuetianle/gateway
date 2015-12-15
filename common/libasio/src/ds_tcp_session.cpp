#include "pch.h"
#include "..\include\ds_tcp_session.h"

ds_tcp_session::ds_tcp_session(const string& ip, int port, int timeout_ms/* = 10000*/, bool bUseSharedService/* = true*/, int recv_buffer_size/* = 0x2000*/, int send_buffer_size/* = 0x2000*/)
	:tcp_session(ip, port, bUseSharedService, timeout_ms, recv_buffer_size, recv_buffer_size)
{
}

ds_tcp_session::ds_tcp_session(tcp::socket sock, int timeout_ms/* = 10000*/, int recv_buffer_size/* = 0x2000*/, int send_buffer_size/* = 0x2000*/)
	: tcp_session(move(sock), timeout_ms, recv_buffer_size, send_buffer_size)
{
}

ds_tcp_session::~ds_tcp_session()
{
}

int ds_tcp_session::send_pkt(const string& data)
{
	auto sf = shared_from_this();
	m_socket.async_write_some(boost::asio::buffer(data.data(), data.length()), [this, sf](const boost::system::error_code& ec, size_t bytes_transferred) 
	{
		if (ec && m_event_handler)
		{
			m_event_handler(ec.message(), ec.value());
		}
	});
	return v_OK;
}

int ds_tcp_session::wait_request()
{
	if (m_socket.is_open())
	{
		set_no_delay(true);
	}
	else
	{
		return v_ERR_SOCKET;
	}
	auto sf = shared_from_this();
	reset_timer();
	boost::asio::spawn(m_strand, [this, sf](boost::asio::yield_context yield)
	{
		boost::system::error_code ec;
		size_t sz = 0;
		for (;;)
		{
			boost::asio::streambuf sb;
			sb.size();
			sz = boost::asio::async_read_until(m_socket, sb, m_msg_delimiter, yield[ec]);
			if (!ec)
			{
				reset_timer();
				if (sz > 0 && m_event_handler)
				{
					boost::asio::streambuf::const_buffers_type buffs = sb.data();
					std::string data(boost::asio::buffers_begin(buffs), boost::asio::buffers_begin(buffs) + sz);
					//cout << "receive data:" << data << "size:" << data.length() << endl;
				/*	sb.commit(sz);
					istream is(&sb);
					string data;
					is >> data;*/
					m_event_handler(data, 0);
				}
			}
			else
			{
				_close();
				if (m_event_handler)
				{
					m_event_handler(ec.message(), ec.value());
				}
				break;
			}
		}
	});
	_spawn_handle_timeout(m_timer, nullptr);
	return v_OK;
}
