//////////////////////////////////////////////////////////////////////////
// file_tcp_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Created By: FJW
// Last modification: 2015/05/23
// Copyright (c) 2013-2015 Vistek 
// Usage: 
//////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "file_tcp_server.h"
#include "file_tcp_session.h"

file_tcp_server::file_tcp_server(int port, int concurrency /*= 1*/, int timeout_ms/*= 10000*/ )
	: tcp_server(port, concurrency), _timeout_ms(timeout_ms)
{

}

file_tcp_server::~file_tcp_server()
{

}

int file_tcp_server::start()
{
	_init_threads();
	auto self = shared_from_this();
	boost::asio::spawn(m_strand,
		[this, self](boost::asio::yield_context yield)
	{
		for (;;)
		{
			boost_error_code ec = _accept(yield[ec]);
			if (!ec)
			{
				auto session = make_shared<file_tcp_session>(move(m_socket), _timeout_ms);
				session->wait_request();
			}
			else
			{
				break;
			}
		}
	});
	return v_OK;
}

void file_tcp_server::stop()
{
	_clean_threads();
}
