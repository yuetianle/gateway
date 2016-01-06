#include "stdafx.h"
#include "msg_processer.h"
msg_processer::msg_processer()
{
}

msg_processer::msg_processer(ds_tcp_session_ptr session, PyObject *py_module)
	:m_send_session_ptr(session), m_is_exist(true), m_py_module(py_module)
{

}

msg_processer::msg_processer(ds_tcp_session_ptr session, string manuc)
	:m_send_session_ptr(session), m_is_exist(true), m_manuc(manuc)
{

}

msg_processer::~msg_processer()
{
	m_is_exist = false;
}

//int msg_processer::process_msg(char *msg, const long &msg_len, char* out_data, long *out_data_len)
//{
//	if (msg && 0 < msg_len)
//	{
//		msg_xml_parser msg_parser(msg);
//#ifdef _DEBUG
//		//vvlog_i("begin params:" << msg_parser.device_id().c_str() << msg_parser.uri().c_str() << (void*)msg_parser.params().c_str() << out_data << out_data_len)
//#endif
//		m_msg_proc(msg_parser.device_id().c_str(), msg_parser.uri().c_str(), (void*)msg_parser.params().c_str(), out_data, out_data_len);
//		msg_xml_builder msg_builder();
//		//vvlog_i("end params:" << msg_parser.device_id().c_str() << msg_parser.uri().c_str() << (void*)msg_parser.params().c_str() << out_data << out_data_len)
//		if (out_data && 0 < *out_data_len)
//		{
//			return v_OK;
//		}
//		else
//		{
//			return v_ERR_Bad_XML;
//		}
//	}
//}

int msg_processer::subscribe_msg_action(MSG_PROC_HANDLE &&handler)
{
	m_msg_proc = handler;
	return v_OK;
}

void msg_processer::do_msg_exe(const string &msg_cmd, int error_code)
{
	if (0 != error_code)
	{
		vvlog_w("invaild cmd:" << msg_cmd << "error_code" << error_code);
		return;
	}
	else
	{
		vvlog_i("start do msg:" << msg_cmd << "errorcode:" << error_code);
	}
	long msg_response_len = 1024 * 10;
	char msg_response[10 * 1024] = { 0 };
	int ec = 0;
	msg_xml_parser msg_parser(msg_cmd);
	if (msg_response && 0 < msg_response_len)
	{
		if (msg_parser.is_invaild())
		{
			vvlog_e("invaild msg_cmd:" << msg_cmd);
			return;
		}
		//auto ar = async(launch::async, [&]()
		auto ar = async(launch::async, [&]()
		{
			python_module_loader loader;
			PyObject *py_module = loader.load_module(m_manuc);
			v_lock(lk, m_device_status_mtx);
			for (auto request_item : msg_parser.requests())
			{
				//PyObject *py_func = m_py_module_loader.get_func(m_py_module, m_req_name);
				PyObject *py_func = loader.get_func(py_module, m_req_name);
				PyObject *py_args = PyTuple_New(3);
				PyTuple_SetItem(py_args, 0, Py_BuildValue("s", request_item.device_id().c_str()));
				PyTuple_SetItem(py_args, 1, Py_BuildValue("s", request_item.uri.c_str()));
				PyTuple_SetItem(py_args, 2, Py_BuildValue("s", request_item.params.c_str()));
				PyObject *py_ret = nullptr;
				if (py_func)
				{
					 //py_ret = m_py_module_loader.call_func(py_func, py_args);
					 py_ret = loader.call_func(py_func, py_args);
				}
				if (py_ret)
				{
					if (/*PyTuple_Check(py_ret) &&*/ 2 == PyTuple_Size(py_ret))
					{
						PyObject *py_out = PyTuple_GetItem(py_ret, 0);
						//PyObject *py_out_repr = PyObject_Repr(py_out);
						if (py_out /*&& PyString_Check(py_out)*/)
						{
							char *tmp = PyString_AsString(py_out);
							int out_size = PyString_Size(py_out);
							if (out_size < msg_response_len)
							{
								memcpy(msg_response, tmp, out_size);
							}
							else
							{
								memcpy(msg_response, tmp, msg_response_len);
							}
						}
					}
				}
			}
			//ec = device_exec_cmd(msg_parser.device_id().c_str(), msg_parser.uri().c_str(), (void*)msg_parser.params().c_str(), msg_response, &msg_response_len);
		});
		ar.get();
	}
	if (0 < strlen(msg_response))
	{
		string send_content = msg_response;
		send_content.append(m_msg_delimiter);
		m_send_session_ptr->send_pkt(send_content);
		vvlog_i("end receive msgresponse:" << send_content << "errorcode:" << error_code);
	}
	else
	{
		vvlog_e("end out msg null." << endl);
	}
	//msg_proc.subscribe_msg_action(std::bind(&device_exec_cmd, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5));
	//int ec = msg_proc.process_msg((char*)msg_cmd.c_str(), msg_cmd.length(), msg_response, &msg_response_len);
	for (auto request_item : msg_parser.requests())
	{
		if (string::npos != request_item.uri.find("get_device_status") && !m_device_status_thrd_ptr)
		{
			m_device_status_thrd_ptr.reset(new thread([&](){
				string device_id = request_item.device_id();
				string uri = request_item.uri;
				string params = request_item.params;
				long msg_status_response_len = 1024 * 10;
				char msg_status_response[10 * 1024] = { 0 };
				python_module_loader device_status_loader;
				PyObject *py_status_module = device_status_loader.load_module(m_manuc);
				bool old_status = true;
				while (m_is_exist)
				{
					memset(msg_status_response, 0, sizeof(msg_status_response));
					v_lock(lk, m_device_status_mtx);
					if (/*m_py_module*/py_status_module)
					{
						//PyObject *py_func = m_py_module_loader.get_func(m_py_module, m_req_name);
						PyObject *py_status_func = device_status_loader.get_func(py_status_module, m_req_name);
						PyObject *py_args = PyTuple_New(3);
						PyTuple_SetItem(py_args, 0, Py_BuildValue("s", device_id.c_str()));
						PyTuple_SetItem(py_args, 1, Py_BuildValue("s", uri.c_str()));
						PyTuple_SetItem(py_args, 2, Py_BuildValue("s", params.c_str()));
						PyObject *py_ret = nullptr;
						if (py_status_func)
						{
							//py_ret = m_py_module_loader.call_func(py_func, py_args);
							py_ret = device_status_loader.call_func(py_status_func, py_args);
						}
						if (py_ret)
						{
							if (/*PyTuple_Check(py_ret) &&*/ 2 == PyTuple_Size(py_ret))
							{
								PyObject *py_out = PyTuple_GetItem(py_ret, 0);
								//PyObject *py_out_repr = PyObject_Repr(py_out);
								if (py_out /*&& PyString_Check(py_out)*/)
								{
									char *tmp = PyString_AsString(py_out);
									int out_size = PyString_Size(py_out);
									if (out_size < msg_status_response_len)
									{
										memcpy(msg_status_response, tmp, out_size);
									}
									else
									{
										memcpy(msg_status_response, tmp, msg_status_response_len);
									}
								}
							}
						}
						pugi::xml_document doc;
						pugi::xml_parse_result xml_ret = doc.load_string((char*)msg_status_response);
						bool cur_status = false;
						if (pugi::xml_parse_status::status_ok == xml_ret.status)
						{
							pugi::xml_node status_node = doc.child("device_status");
							cur_status = status_node.text().as_bool();
						}
						if (cur_status != old_status)
						{
							if (0 < strlen(msg_status_response))
							{
								string send_content = msg_status_response;
								send_content.append(m_msg_delimiter);
								m_send_session_ptr->send_pkt(send_content);
							}
						}
					}
					this_thread::sleep_for(chrono::seconds(5));
				}
		}));
			m_device_status_thrd_ptr->detach();
		}
	}
	//if (string::npos != msg_parser.uri().find("get_device_status") && !m_device_status_thrd_ptr)
	//{
	//	m_device_status_thrd_ptr.reset(new thread([&](){
	//		string device_id = msg_parser.device_id();
	//		string uri = msg_parser.uri();
	//		string params = msg_parser.params();
	//		long msg_status_response_len = 1024 * 10;
	//		char msg_status_response[10 * 1024] = { 0 };
	//		python_module_loader device_status_loader;
	//		PyObject *py_status_module = device_status_loader.load_module(m_manuc);
	//		while (m_is_exist)
	//		{
	//			memset(msg_status_response, 0, sizeof(msg_status_response));
	//			v_lock(lk, m_device_status_mtx);
	//			if (/*m_py_module*/py_status_module)
	//			{
	//				//PyObject *py_func = m_py_module_loader.get_func(m_py_module, m_req_name);
	//				PyObject *py_status_func = device_status_loader.get_func(py_status_module, m_req_name);
	//				PyObject *py_args = PyTuple_New(3);
	//				PyTuple_SetItem(py_args, 0, Py_BuildValue("s", device_id.c_str()));
	//				PyTuple_SetItem(py_args, 1, Py_BuildValue("s", uri.c_str()));
	//				PyTuple_SetItem(py_args, 2, Py_BuildValue("s", params.c_str()));
	//				PyObject *py_ret = nullptr;
	//				if (py_status_func)
	//				{
	//					//py_ret = m_py_module_loader.call_func(py_func, py_args);
	//					py_ret = device_status_loader.call_func(py_status_func, py_args);
	//				}
	//				if (py_ret)
	//				{
	//					if (/*PyTuple_Check(py_ret) &&*/ 2 == PyTuple_Size(py_ret))
	//					{
	//						PyObject *py_out = PyTuple_GetItem(py_ret, 0);
	//						//PyObject *py_out_repr = PyObject_Repr(py_out);
	//						if (py_out /*&& PyString_Check(py_out)*/)
	//						{
	//							char *tmp = PyString_AsString(py_out);
	//							int out_size = PyString_Size(py_out);
	//							if (out_size < msg_status_response_len)
	//							{
	//								memcpy(msg_status_response, tmp, out_size);
	//							}
	//							else
	//							{
	//								memcpy(msg_status_response, tmp, msg_status_response_len);
	//							}
	//						}
	//						//Py_XDECREF(py_out);
	//					}
	//				}
	//				if (0 < strlen(msg_status_response))
	//				{
	//					string send_content = msg_status_response;
	//					send_content.append(m_msg_delimiter);
	//					m_send_session_ptr->send_pkt(send_content);
	//				}
	//			}
	//			this_thread::sleep_for(chrono::seconds(5));
	//		}
	//}));
	//	m_device_status_thrd_ptr->detach();
	//}
	//if (string::npos != msg_parser.uri().find("get_device_status") && !m_device_status_thrd_ptr)
	//{
	//	m_device_status_thrd_ptr.reset(new thread([&](){
	//		string device_id = msg_parser.device_id();
	//		string uri = msg_parser.uri();
	//		string params = msg_parser.params();
	//		long msg_status_response_len = 1024 * 10;
	//		char msg_status_response[10 * 1024] = { 0 };
	//		python_module_loader device_status_loader;
	//		PyObject *py_status_module = device_status_loader.load_module(m_manuc);
	//		while (m_is_exist)
	//		{
	//			memset(msg_status_response, 0, sizeof(msg_status_response));
	//			v_lock(lk, m_device_status_mtx);
	//			if (/*m_py_module*/py_status_module)
	//			{
	//				//PyObject *py_func = m_py_module_loader.get_func(m_py_module, m_req_name);
	//				PyObject *py_status_func = device_status_loader.get_func(py_status_module, m_req_name);
	//				PyObject *py_args = PyTuple_New(3);
	//				PyTuple_SetItem(py_args, 0, Py_BuildValue("s", device_id.c_str()));
	//				PyTuple_SetItem(py_args, 1, Py_BuildValue("s", uri.c_str()));
	//				PyTuple_SetItem(py_args, 2, Py_BuildValue("s", params.c_str()));
	//				PyObject *py_ret = nullptr;
	//				if (py_status_func)
	//				{
	//					//py_ret = m_py_module_loader.call_func(py_func, py_args);
	//					py_ret = device_status_loader.call_func(py_status_func, py_args);
	//				}
	//				if (py_ret)
	//				{
	//					if (/*PyTuple_Check(py_ret) &&*/ 2 == PyTuple_Size(py_ret))
	//					{
	//						PyObject *py_out = PyTuple_GetItem(py_ret, 0);
	//						//PyObject *py_out_repr = PyObject_Repr(py_out);
	//						if (py_out /*&& PyString_Check(py_out)*/)
	//						{
	//							char *tmp = PyString_AsString(py_out);
	//							int out_size = PyString_Size(py_out);
	//							if (out_size < msg_status_response_len)
	//							{
	//								memcpy(msg_status_response, tmp, out_size);
	//							}
	//							else
	//							{
	//								memcpy(msg_status_response, tmp, msg_status_response_len);
	//							}
	//						}
	//						//Py_XDECREF(py_out);
	//					}
	//				}
	//				if (0 < strlen(msg_status_response))
	//				{
	//					string send_content = msg_status_response;
	//					send_content.append(m_msg_delimiter);
	//					m_send_session_ptr->send_pkt(send_content);
	//				}
	//			}
	//			this_thread::sleep_for(chrono::seconds(5));
	//		}
	//}));
	//	m_device_status_thrd_ptr->detach();
	//}
}

