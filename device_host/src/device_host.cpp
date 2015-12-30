#include "stdafx.h"
#include "device_host.h"

pugi::xml_document device_host::s_host_list;
mutex device_host::s_host_mutex;
data_cache_ex<string, ds_tcp_session_ptr> device_host::s_send_session;
//boost::interprocess::file_lock device_host::s_flock("host_lists.xml");
device_host::device_host(const string manuc)
	:m_is_start(false), m_ds_tcp_server(new ds_tcp_server(0, 30000)), m_manufacture(manuc), m_send_session(nullptr)
{
	python_module_loader::init();
	m_py_module = m_py_module_loader.load_module(manuc);
	if (v_str_compare_no_case("hikvision", manuc))
	{
		load_device_plugin(V_MANUFACTURER_HIKVISION);
	}
	else if (v_str_compare_no_case("onvif", manuc))
	{
		load_device_plugin(V_MANUFACTURER_ONVIF);
	}
	else if (v_str_compare_no_case("psia", manuc))
	{
		load_device_plugin(V_MANUFACTURER_PSIA);
	}
	m_msg_manager_ptr.reset(new msg_manager(m_py_module));
}

device_host::~device_host()
{
	if (m_ds_tcp_server && m_is_start)
	{
		m_ds_tcp_server->stop();
	}
	if (m_exist_host_file)
	{
		fclose(m_exist_host_file);
	}
	fsys::remove(m_host_info_name);
	python_module_loader::uninit();
	//unload_device_plugin(m_manufacture);
}

bool device_host::is_start()
{
	return m_is_start;
}

int device_host::start()
{
	int ec = m_ds_tcp_server->start();
	if (!ec)
	{
		m_is_start = true;
		//m_ds_tcp_server->subscribe_new_session(std::bind(&device_host::_do_msg, shared_from_this(), placeholders::_1, placeholders::_2));
		m_ds_tcp_server->subscribe_new_session(std::bind(&msg_manager::do_msg_receive, m_msg_manager_ptr, placeholders::_1, placeholders::_2));
		_write_host_listen_ports(m_manufacture, m_ds_tcp_server->get_listen_port());
		//v_lock(s_host_mutex, s_host_list);
		pugi::xml_node decl_node = s_host_list.prepend_child(pugi::node_declaration);
		decl_node.append_attribute("version") = "1.0";
		decl_node.append_attribute("encoding") = "utf-8";
		auto host_nodes = s_host_list.child("hosts");
		if (host_nodes.empty())
		{
			host_nodes = s_host_list.append_child("hosts");
		}
		pugi::xml_node host_node = host_nodes.append_child("host");
		auto manu_attr = host_node.attribute("manufacture");
		if (manu_attr.empty())
		{
			manu_attr = host_node.append_attribute("manufacture");
			manu_attr.set_value(m_manufacture.c_str());
		}
		auto port_attr = host_node.attribute("port");
		if (port_attr.empty())
		{
			port_attr = host_node.append_attribute("port");
			port_attr.set_value(m_ds_tcp_server->get_listen_port());
		}
		vvlog_i("start host success, manufacture"<< m_manufacture<< "listen port:" << m_ds_tcp_server->get_listen_port());
		return ec;
	}
}

int device_host::stop()
{
	if (m_is_start)
	{
		m_is_start = false;
	}
	return v_OK;
}

void device_host::_do_msg(const ds_tcp_session_ptr& session, const string &msg)
{
	if (session /*&& !msg.empty()*/)
	{
		if (!m_send_session || session != m_send_session)
		{
			m_send_session = session;
			session->subscribe_event_handler(std::bind(&device_host::_exe_msg_cmd, shared_from_this(), placeholders::_1, placeholders::_2));
			session->wait_request();
		}
	}
	else
	{
		vvlog_w("session null, error_code:" << msg);
	}
}

int device_host::write_host_lists(const string &file_name)
{
	//v_lock(s_host_mutex, s_host_list);
	fsys::path s(file_name);
	pugi::xml_document doc;
	boost::system::error_code ec;
	if (fsys::exists(s, ec))
	{
		int ec =  read_host_lists(doc, file_name);
		boost::interprocess::file_lock flock(file_name.c_str());
		flock.lock();
		if (v_OK !=  ec)
		{
			cout << "empty node" << endl;
			//flock.unlock();
			fsys::remove(s);
		}
		auto host_nodes_tmp = doc.child("hosts");
		pugi::xml_node decl_node = doc.prepend_child(pugi::node_declaration);
		decl_node.append_attribute("version") = "1.0";
		decl_node.append_attribute("encoding") = "utf-8";
		pugi::xml_node host_nodes = s_host_list.child("hosts");
		for (pugi::xml_node_iterator item = host_nodes.begin(); item != host_nodes.end(); ++item)
		{
			host_nodes_tmp.append_copy(*item);
		}
		/*if (!host_nodes.empty())
		{
			doc.append_copy(host_nodes);
		}*/
		if (doc.save_file(file_name.c_str(), "\t", pugi::encoding_utf8))
		{
			doc.print(std::cout);
			std::cout << std::endl;
			vvlog_i("device hosts info writes success.");
			//flock.unlock();
			return v_OK;
		}
		else
		{
			vvlog_e("device hosts info writes failed.");
			//flock.unlock();
			return v_ERR_Bad_XML;
		}
		//fsys::remove(s);
	}
	/*pugi::xml_node decl_node = doc.prepend_child(pugi::node_declaration);
	decl_node.append_attribute("version") = "1.0";
	decl_node.append_attribute("encoding") = "utf-8";
	pugi::xml_node host_nodes = s_host_list.child("hosts");
	if (!host_nodes.empty())
	{
		doc.append_copy(host_nodes);
	}*/
	if (s_host_list.save_file(file_name.c_str()))
	{
		//doc.print(std::cout);
		//std::cout << std::endl;
		vvlog_i("device hosts info writes success.");
		//flock.unlock();
		return v_OK;
	}
	else
	{
		vvlog_e("device hosts info writes failed.");
		//flock.unlock();
		return v_ERR_Bad_XML;
	}
}

void device_host::_exe_msg_cmd(const string &msg_cmd, int error_code)
{
	if (0 != error_code)
	{
		vvlog_w("invaild cmd:" << msg_cmd << "error_code" << error_code);
		return;
	}
	else
	{
		vvlog_i("start receive msg:" << msg_cmd << "errorcode:" << error_code);
	}
	msg_processer msg_proc;
	long msg_response_len = 1024 * 10;
	char msg_response[10 * 1024] = { 0 };
	int ec = 0;
	msg_xml_parser msg_parser(msg_cmd);
	if (msg_response && 0 < msg_response_len)
	{
		if (msg_parser.is_invaild())
		{
			return;
		}
		auto ar = async(launch::async, [&]()
		{
			if (m_py_module)
			{
				PyObject *py_func = m_py_module_loader.get_func(m_py_module, m_req_name);
				PyObject *py_args = PyTuple_New(3);
				PyTuple_SetItem(py_args, 0, Py_BuildValue("s", msg_parser.device_id().c_str()));
				PyTuple_SetItem(py_args, 1, Py_BuildValue("s", msg_parser.uri().c_str()));
				PyTuple_SetItem(py_args, 2, Py_BuildValue("s", msg_parser.params().c_str()));
				PyObject *py_ret = nullptr;
				if (py_func)
				{
					 py_ret = m_py_module_loader.call_func(py_func, py_args);
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
						//Py_XDECREF(py_out);
					}
				}
				//Py_XDECREF(py_args);
				//Py_XDECREF(py_func);
			}
			//ec = device_exec_cmd(msg_parser.device_id().c_str(), msg_parser.uri().c_str(), (void*)msg_parser.params().c_str(), msg_response, &msg_response_len);
		});
		ar.get();
	}

	//msg_proc.subscribe_msg_action(std::bind(&device_exec_cmd, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5));
	//int ec = msg_proc.process_msg((char*)msg_cmd.c_str(), msg_cmd.length(), msg_response, &msg_response_len);
	
	if (string::npos != msg_parser.uri().find("get_device_status"))
	{
		m_device_status_thrd_ptr.reset(new thread([&](){
			string device_id = msg_parser.device_id();
			string uri = msg_parser.uri();
			string params = msg_parser.params();
			long msg_status_response_len = 1024 * 10;
			char msg_status_response[10 * 1024] = { 0 };
			while (m_is_start )
			{
				if (m_py_module)
				{
					PyObject *py_func = m_py_module_loader.get_func(m_py_module, m_req_name);
					PyObject *py_args = PyTuple_New(3);
					PyTuple_SetItem(py_args, 0, Py_BuildValue("s", device_id.c_str()));
					PyTuple_SetItem(py_args, 1, Py_BuildValue("s", uri.c_str()));
					PyTuple_SetItem(py_args, 2, Py_BuildValue("s", params.c_str()));
					PyObject *py_ret = nullptr;
					if (py_func)
					{
						py_ret = m_py_module_loader.call_func(py_func, py_args);
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
							//Py_XDECREF(py_out);
						}
					}
					if (0 < strlen(msg_status_response))
					{
						string send_content = msg_status_response;
						send_content.append(m_msg_delimiter);
						m_send_session->send_pkt(send_content);
					}
					//Py_XDECREF(py_ret);
					//Py_XDECREF(py_args);
					//Py_XDECREF(py_func);
				}
				this_thread::sleep_for(chrono::seconds(5));
			}
			
	}));
		m_device_status_thrd_ptr->detach();
	}
	/*ds_tcp_session_ptr send_session = nullptr;
	if (!s_send_session.find(msg_parser.device_id(), send_session))
	{
		PyObject *py_func = m_py_module_loader.get_func(m_py_module, "wrap_device_status");
		PyObject *py_wrap_args = PyTuple_New(4);
		PyTuple_SetItem(py_wrap_args, 0, Py_BuildValue("s", msg_parser.device_id().c_str()));
		PyTuple_SetItem(py_wrap_args, 1, Py_BuildValue("s", msg_parser.uri().c_str()));
		PyTuple_SetItem(py_wrap_args, 2, Py_BuildValue("s", msg_parser.params().c_str()));
		PyTuple_SetItem(py_wrap_args, 3,Py_BuildValue("(O)", &device_host::_device_status_callback));
		if (py_func)
		{
			PyObject *py_wrap_ret = nullptr;
			py_wrap_ret = m_py_module_loader.call_func(py_func, py_wrap_args);
		}
		s_send_session.insert(msg_parser.device_id(), m_send_session);
	}*/

	if (0 < strlen(msg_response))
	{
		m_send_content = msg_response;
		m_send_content.append(m_msg_delimiter);
		m_send_session->send_pkt(m_send_content);
	}
	else
	{
		cout << "out msg null" << endl;
	}
	vvlog_i("end receive msgresponse:" << m_send_content << "errorcode:" << error_code);
}

int device_host::read_host_lists(pugi::xml_document &doc, const string &file_name)
{
	if (!file_name.empty())
	{
		fsys::path s(file_name);
		if (fsys::exists(s))
		{
			pugi::xml_parse_result xml_ret = doc.load_file(file_name.c_str());
			if (pugi::xml_parse_status::status_ok == xml_ret.status)
			{
				return v_OK;
			}
		}
	}
	return v_ERR_BAD_ARGUMENT;

}

int device_host::_write_host_listen_ports(const string &manuc, const uint16_t &port)
{
	if (m_is_start)
	{
		fsys::path host_listen_dir(m_host_listen_dir);
#ifdef _WIN32
		char file_name[100] = { 0 };
		sprintf_s(file_name, "%d.xml", GetCurrentProcessId());
#endif
		if (!fsys::exists(host_listen_dir))
		{
			fsys::create_directory(host_listen_dir);
		}
		host_listen_dir /= file_name;
		fsys::path full_file = host_listen_dir;
		if (fsys::exists(full_file))
		{
			return v_OK;
		}
		auto host_nodes_tmp = m_host_lists_doc.child("hosts");
		pugi::xml_node decl_node = m_host_lists_doc.prepend_child(pugi::node_declaration);
		decl_node.append_attribute("version") = "1.0";
		decl_node.append_attribute("encoding") = "utf-8";
		auto host_nodes = m_host_lists_doc.child("hosts");
		if (host_nodes.empty())
		{
			host_nodes = m_host_lists_doc.append_child("hosts");
		}
		pugi::xml_node host_node = host_nodes.append_child("host");
		auto manu_attr = host_node.attribute("manufacture");
		if (manu_attr.empty())
		{
			manu_attr = host_node.append_attribute("manufacture");
			manu_attr.set_value(manuc.c_str());
		}
		auto port_attr = host_node.attribute("port");
		if (port_attr.empty())
		{
			port_attr = host_node.append_attribute("port");
			port_attr.set_value(port);
		}
		if (m_host_lists_doc.save_file(full_file.string().c_str()))
		{
			vvlog_i("save host_listen file sucess:file:" << full_file.string());
			//m_exist_host_file_path = full_file.string();
			m_exist_host_file = fopen(full_file.string().c_str(), "r");
			m_host_info_name = full_file.string();
			return v_OK;
		}
		else
		{
			vvlog_e("save host_listen file fail:file:" << full_file.string());
			return v_ERR_Bad_XML;
		}
	}
	return v_ERR_Bad_XML;
}

void device_host::_device_status_callback(char* device_id, char* content, long content_len)
{
	if (device_id && 0 < content_len && content)
	{
		ds_tcp_session_ptr tmp_session_ptr = nullptr;
#ifdef _DEBUG
		cout << "device_id:" << device_id << "content:" << content << "content_len:" << content_len << endl;
#endif
		if (s_send_session.find(device_id, tmp_session_ptr))
		{
			string send_content = content;
			tmp_session_ptr->send_pkt(send_content);
		}
	}
}
