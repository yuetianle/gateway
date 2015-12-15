//////////////////////////////////////////////////////////////////////////
// tcp_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Created By: FJW
// Last modification: 2015/05/23
// Copyright (c) 2013-2015 Vistek 
//////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "tcp_server.h"

tcp_server::tcp_server(int port, int concurrency_num /*= 1*/) 
	: m_service(new boost::asio::io_service()),
	m_socket(*m_service), m_strand(*m_service), m_local_ep(boost::asio::ip::tcp::v4(), port), m_acceptor(*m_service, m_local_ep), _concurrency_num(concurrency_num)
{

}

tcp_server::tcp_server(const string& ip, int port, int concurrency_num /*= 1*/) 
	: m_service(new boost::asio::io_service()),
	m_socket(*m_service), m_strand(*m_service), m_local_ep(boost::asio::ip::address::from_string(ip), port), m_acceptor(*m_service, m_local_ep), _concurrency_num(concurrency_num)
{

}

tcp_server::~tcp_server()
{

}

void tcp_server::_init_threads()
{
	if (m_worker == 0)
	{
		m_worker = make_shared<boost::asio::io_service::work>(*m_service);
		auto svc = m_service;
		for (int i = 0; i < _concurrency_num; ++i)
		{
			thread trd([svc]()
			{
				boost_error_code ec;
				svc->run(ec);
				cout << "tcp_server thread has been terminated" << endl;
			});
			trd.detach();
		}
	}
	
}

void tcp_server::_clean_threads()
{
	boost_error_code ec;
	m_acceptor.cancel(ec);
	m_acceptor.close(ec);
	if (m_worker != 0)
	{
		m_worker.reset();
	}
}

boost::system::error_code tcp_server::_accept(boost::asio::yield_context yield)
{
	boost::system::error_code ec;
	m_acceptor.async_accept(m_socket, yield[ec]);
	return move(ec);
}
