#ifndef _MSG_MANAGER_H_
#define _MSG_MANAGER_H_
#pragma once
#include "msg_processer.h"
#include "ds_tcp_session.h"
class msg_manager
{
public:
	msg_manager();
	msg_manager(string manuc);
	msg_manager(PyObject *py_module);
	~msg_manager();
public:
	void do_msg_receive(const ds_tcp_session_ptr &session, const string &error_code);
private:
	static data_cache_ex<ds_tcp_session_ptr, msg_processer_ptr> s_rev_session_list;
	string m_manuc;
	PyObject *m_py_module;
};

typedef shared_ptr<msg_manager> msg_manager_ptr;
#endif

