#include "stdafx.h"
#include "msg_manager.h"

data_cache_ex<ds_tcp_session_ptr, msg_processer_ptr> msg_manager::s_rev_session_list;
msg_manager::msg_manager()
{
}
msg_manager::msg_manager(PyObject *py_module)
	:m_py_module(py_module)
{
}

msg_manager::msg_manager(string manuc)
	: m_manuc(manuc)
{

}

msg_manager::~msg_manager()
{
}

void msg_manager::do_msg_receive(const ds_tcp_session_ptr &session, const string &error_code)
{
	msg_processer_ptr msg_proc_ptr = nullptr;
	if (session && !s_rev_session_list.find(session, msg_proc_ptr))
	{
		//msg_proc_ptr.reset(new msg_processer(session, m_py_module));
		msg_proc_ptr.reset(new msg_processer(session, m_manuc));
		session->subscribe_event_handler(std::bind(&msg_processer::do_msg_exe, msg_proc_ptr, placeholders::_1, placeholders::_2));
		s_rev_session_list.insert(session, msg_proc_ptr);
		session->wait_request();
	}
}
