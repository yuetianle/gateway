//////////////////////////////////////////////////////////////////////////
// file_tcp_session.h
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Created By: FJW
// Last modification: 2015/05/23
// Copyright (c) 2013-2015 Vistek 
// Usage: 
//////////////////////////////////////////////////////////////////////////
#ifndef tcp_file_session_h__
#define tcp_file_session_h__

#pragma once
#include "tcp_session.h"
#include "pugixml.hpp"
#include "libasio_exports.h"
using namespace std;
using boost::asio::ip::tcp;

class LIBASIO_API file_tcp_session : public tcp_session
{
	static mutex _s_mtx_path; // this mutex is used for avoiding crash in fsys::path() because boost::filesystem::path::codecvt() is non-thread-safe;
public:
	typedef tuple<string, string, int64_t> file_request_param;
	typedef tuple<string, string, int64_t, bool> file_response_param;
	struct file_task_info
	{
		string name;
		string mode;
		int64_t size;
		bool allow;
		string buff;
		file_task_info(file_request_param req)
		{
			tie(name, mode, size) = req;
		}
		file_task_info(file_response_param resp)
		{
			tie(name, mode, size, allow) = resp;
		}
	};
public:
	explicit file_tcp_session(const string& ip, int port, bool bOwnService, int timeout_ms = 10000, int recv_buffer_size = 0x10000, int send_buffer_size = 0x1000);
	explicit file_tcp_session(tcp::socket sock, int timeout_ms = 10000, int recv_buffer_size = 0x1000, int send_buffer_size = 0x10000);
protected:
	file_task_info* _file_task = 0;
protected:
	virtual string _make_response(file_response_param param);
	virtual file_response_param _parse_response(const string& resp);

	//************************************
	// Method:    _make_request
	// FullName:  tcp_file_session::_make_request
	// Access:    virtual protected 
	// Returns:   std::string
	// Qualifier:
	// Parameter: const string & filename
	// Parameter: const string & mode : "upload" or "download"
	//************************************
	virtual string _make_request(file_request_param param);

	virtual file_request_param _parse_request(const string& req);

	virtual void _send_file(const string& fn, coro_timer_ptr ptimer, boost::asio::yield_context yield);

	virtual int _read_file(const string& fn, int64_t file_size, coro_timer_ptr ptimer, boost::asio::yield_context yield);

	virtual void _handle_request(string req, coro_timer_ptr ptimer, boost::asio::yield_context yield);
public:
	virtual void wait_request();
	virtual int read_file(const string& fn, const string& target);
	virtual int _coro_action(boost::asio::coroutine coro)
	{
		return v_ERR_TIMEOUT;
	}
	virtual int read_file_sync(const string& fn, const string& target)
	{
		return v_ERR_TIMEOUT;
	}
};
#endif // tcp_file_session_h__
