#include "stdafx.h"
#include "msg_manager.h"


data_cache_ex<ds_tcp_session_ptr, msg_distribution_ptr> msg_manager::s_rev_session_list;
msg_manager::msg_manager()
{
}


msg_manager::~msg_manager()
{
}

void msg_manager::do_recv_msg(const ds_tcp_session_ptr &session, const string &error_code)
{
	msg_distribution_ptr msg_distr_ptr = nullptr;
	if (session && !s_rev_session_list.find(session, msg_distr_ptr))
	{
		msg_distr_ptr.reset(new msg_distribution(session));
		session->subscribe_event_handler(std::bind(&msg_distribution::do_msg_send, msg_distr_ptr, placeholders::_1, placeholders::_2));
		session->wait_request();
		s_rev_session_list.insert(session, msg_distr_ptr);
	}
}
