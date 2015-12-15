#ifndef _MSG_DISTRIBUTION_H_
#define _MSG_DISTRIBUTION_H_
#pragma once
#include "ds_tcp_server.h"
#include "ds_tcp_session.h"

const string s_host_list_info_name = "host_lists.xml";
class msg_distribution : public enable_shared_from_this<msg_distribution>
{
public:
	msg_distribution();
	~msg_distribution();
public:
	static int read_all_host_lists_info(const string &host_info_dir);
	static int read_host_info(const string &file_name);
public:
	void do_msg_receive(const string &content, int error_code);
	void do_msg_send(const string &content, int error_code);
	void do_msg_distribute(const ds_tcp_session_ptr &session, const string &content);
private:
	ds_tcp_session_ptr m_send_session;
	static data_cache_ex<int, string> s_host_listen_lists;/*<< port ³§ÉÌÃû*/
	static data_cache_ex<string, ds_tcp_session_ptr> s_host_session_lists;/*<< device_id tcp-session*/
	static mutex s_host_mutex;
	const string m_host_listen_dir = "hostboot";
};
typedef shared_ptr<msg_distribution> msg_distribution_ptr;
#endif

