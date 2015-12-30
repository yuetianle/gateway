#ifndef _MSG_PROCESSER_H_
#define _MSG_PROCESSER_H_
#pragma once
#include "msg_xml_parser.h"
#include "msg_xml_builder.h"
#include "ds_tcp_session.h"
#include "python_module_loader.h"
typedef std::function<int (const char*, const char*, void *, char*, long *)> MSG_PROC_HANDLE;/* << */
class msg_processer
{
public:
	msg_processer();
	msg_processer(ds_tcp_session_ptr session, PyObject *py_module);
	~msg_processer();
public:
	void do_msg_exe(const string &msg_cmd, int error_code);
	int process_msg(char *msg, const long &msg_len, char* out_data, long *out_data_len);
	int subscribe_msg_action(MSG_PROC_HANDLE &&handler);
private:
	pugi::xml_document m_xml_msg;
	MSG_PROC_HANDLE m_msg_proc;
	ds_tcp_session_ptr m_send_session_ptr;
	python_module_loader m_py_module_loader;
	const string m_req_name = "request_cmd";
	PyObject *m_py_module;
	shared_ptr<thread> m_device_status_thrd_ptr;
	string m_msg_delimiter = "<!--msg_end-->";
	bool m_is_exist = false;
};
typedef shared_ptr<msg_processer> msg_processer_ptr;
#endif

