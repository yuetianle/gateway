//////////////////////////////////////////////////////////////////////////
// file_tcp_server.h
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Created By: FJW
// Last modification: 2015/05/23
// Copyright (c) 2013-2015 Vistek 
// Usage: 
//////////////////////////////////////////////////////////////////////////
#ifndef file_server_h__
#define file_server_h__

#include "tcp_server.h"
#include "libasio_exports.h"


using namespace std;
using boost::asio::ip::tcp;

class LIBASIO_API file_tcp_server : public tcp_server
{
public:
	file_tcp_server(int port, int concurrency = 1,  int timeout_ms =10000 );
	virtual ~file_tcp_server();
public:
	virtual int start();
	void stop();
private:
	int _timeout_ms;
};

#endif // file_server_h__
