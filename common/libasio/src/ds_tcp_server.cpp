#include "pch.h"
#include "..\include\ds_tcp_server.h"


ds_tcp_server::ds_tcp_server(const string& ip, int port, int timeout_ms/* = 10000*/, int concurrency_num/* = 1*/)
	:tcp_server(ip, port, concurrency_num)
	, m_timeout_ms(timeout_ms)
{
}


ds_tcp_server::ds_tcp_server(int port, int timeout_ms/* = 10000*/, int concurrency_num /*= 1*/)
	:tcp_server(port, concurrency_num)
	, m_timeout_ms(timeout_ms)
{

}

ds_tcp_server::~ds_tcp_server()
{
}

int ds_tcp_server::start()
{
	_init_threads();
	auto sf = shared_from_this();
	boost::asio::spawn(m_strand, [this, sf](boost::asio::yield_context yield)
	{
		boost::system::error_code ec;
		for (;;)
		{
			ec = _accept(yield);
			if (!ec)
			{
				auto session = make_shared<ds_tcp_session>(move(m_socket), m_timeout_ms);
				if (m_session_handler)
				{
					m_session_handler(session, "");
				}
			}
			else
			{
				if (m_session_handler)
				{
					m_session_handler(nullptr, ec.message());
				}
				break;
			}
		}
	});
	return v_OK;
}

int ds_tcp_server::stop()
{
	_clean_threads();
	return v_OK;
}

uint16_t ds_tcp_server::get_listen_port()
{
	boost::system::error_code ec;
	boost::asio::ip::tcp::endpoint ep = m_acceptor.local_endpoint(ec);
	if (!ec)
	{
		return ep.port();
	}
	return 0;
}
