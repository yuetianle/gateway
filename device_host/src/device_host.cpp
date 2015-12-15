#include "stdafx.h"
#include "device_host.h"

pugi::xml_document device_host::s_host_list;
mutex device_host::s_host_mutex;
//boost::interprocess::file_lock device_host::s_flock("host_lists.xml");
device_host::device_host(const string manuc)
	:m_is_start(false), m_ds_tcp_server(new ds_tcp_server(0, 30000)), m_manufacture(manuc), m_send_session(nullptr)
{
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
		m_ds_tcp_server->subscribe_new_session(std::bind(&device_host::_do_msg, shared_from_this(), placeholders::_1, placeholders::_2));
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
	}
	return v_OK;
}

void device_host::_do_msg(const ds_tcp_session_ptr& session, const string &msg)
{
	if (session /*&& !msg.empty()*/)
	{
		m_send_session = session;
		session->subscribe_event_handler(std::bind(&device_host::_exe_msg_cmd, shared_from_this(), placeholders::_1, placeholders::_2));
		session->wait_request();
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
	if (msg_response && 0 < msg_response_len)
	{
		msg_xml_parser msg_parser(msg_cmd);
		auto ar = async(launch::async, [&]()
		{
			ec = device_exec_cmd(msg_parser.device_id().c_str(), msg_parser.uri().c_str(), (void*)msg_parser.params().c_str(), msg_response, &msg_response_len);
		});
		ar.get();
	}

	//msg_proc.subscribe_msg_action(std::bind(&device_exec_cmd, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5));
	//int ec = msg_proc.process_msg((char*)msg_cmd.c_str(), msg_cmd.length(), msg_response, &msg_response_len);
	vvlog_i("end receive msg:" << msg_cmd << "errorcode:" << error_code);
	if (!ec)
	{
		string send_content;
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
	}
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
