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
	//m_py_module = m_py_module_loader.load_module(manuc);
	/*if (v_str_compare_no_case("hikvision", manuc))
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
	}*/
	//m_msg_manager_ptr.reset(new msg_manager(m_py_module));
	m_msg_manager_ptr.reset(new msg_manager(m_manufacture));
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
		/*pugi::xml_node decl_node = s_host_list.prepend_child(pugi::node_declaration);
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
		}*/
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
