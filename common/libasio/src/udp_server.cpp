#include "pch.h"
#include "../include/udp_server.h"


udp_server::udp_server(const string& ip, int port, int concurrency_num /*= 1*/)
	: m_service(new boost::asio::io_service())
	, m_socket(new boost::asio::ip::udp::socket(*m_service))
	, m_strand(*m_service)
	, m_local_ep(boost::asio::ip::address::from_string(ip), port)
	, m_concurrency_num(concurrency_num)
{
}

udp_server::udp_server(int port, int concurrency_num /*= 1*/)
	: m_service(new boost::asio::io_service())
	, m_socket(new boost::asio::ip::udp::socket(*m_service))
	, m_strand(*m_service)
	, m_local_ep(boost::asio::ip::udp::v4(), port)
	, m_concurrency_num(concurrency_num)
{

}


udp_server::~udp_server()
{
}

void udp_server::_init_threads()
{
	if (m_worker == 0)
	{
		m_worker = make_shared<boost::asio::io_service::work>(*m_service);
		auto svc = m_service;
		for (int i = 0; i < m_concurrency_num; ++i)
		{
			thread trd([svc]()
			{
				boost_error_code ec;
				svc->run(ec);
				cout << "udp_server thread has been terminated" << endl;
			});
			trd.detach();
		}
	}
}

void udp_server::_clean_threads()
{
	if (m_worker != 0)
	{
		m_worker.reset();
	}
}
