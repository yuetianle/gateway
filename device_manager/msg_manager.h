#ifndef _MSG_MANAGER_H_
#define _MSG_MANAGER_H_
#include "msg_distribution.h"
#include "ds_tcp_session.h"
#pragma once
class msg_manager
{
public:
	msg_manager();
	~msg_manager();
public:
	void do_recv_msg(const ds_tcp_session_ptr &session, const string &error_code);
private:
	static data_cache_ex<ds_tcp_session_ptr, msg_distribution_ptr> s_rev_session_list;
};
typedef shared_ptr<msg_manager> msg_manager_ptr;
#endif

