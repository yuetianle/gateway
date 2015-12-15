//////////////////////////////////////////////////////////////////////////
// tcp_server.h
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Created By: FJW
// Last modification: 2015/05/23
// Copyright (c) 2013-2015 Vistek 
//////////////////////////////////////////////////////////////////////////
#ifndef tcp_server_h__
#define tcp_server_h__

#include "libasio_exports.h"
#include <boost/asio/steady_timer.hpp>
using namespace std;

class LIBASIO_API tcp_server : public enable_shared_from_this<tcp_server>
{
public:
	explicit tcp_server(int port, int concurrency_num = 1);
	explicit tcp_server(const string& ip, int port, int concurrency_num = 1);
	virtual ~tcp_server();
	boost::asio::io_service& service() { return *m_service; }
protected:
	virtual void _init_threads();
	virtual void _clean_threads();
	virtual boost::system::error_code _accept(boost::asio::yield_context yield);
protected:
	shared_ptr<boost::asio::io_service> m_service;
	shared_ptr<boost::asio::io_service::work> m_worker;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::io_service::strand m_strand;

	boost::asio::ip::tcp::endpoint m_local_ep;
	boost::asio::ip::tcp::acceptor m_acceptor;
	int _concurrency_num;
	std::vector<std::thread> _thread_array; 
};
#endif // tcp_server_h__