#include "stdafx.h"
#include "msg_distribution.h"
#include "msg_xml_parser.h"

data_cache_ex<string, ds_tcp_session_ptr> msg_distribution::s_host_session_lists;/*<< device_id tcp-session*/
data_cache_ex<int, string> msg_distribution::s_host_listen_lists;/*<< port ³§ÉÌÃû*/
std::mutex msg_distribution::s_host_mutex;
msg_distribution::msg_distribution()
{
}

msg_distribution::msg_distribution(ds_tcp_session_ptr session_ptr)
	:m_send_session(session_ptr)
{

}

msg_distribution::~msg_distribution()
{
}

void msg_distribution::do_msg_distribute(const ds_tcp_session_ptr &session, const string &content)
{
	if (session)
	{
		if (!m_send_session || m_send_session != session)
		{
			m_send_session = session;
			session->subscribe_event_handler(std::bind(&msg_distribution::do_msg_send, shared_from_this(), placeholders::_1, placeholders::_2));
			session->wait_request();
		}
	}
	else
	{
		vvlog_e("connect fail session null errorcode:%s", content.c_str());
	}
}

int msg_distribution::read_host_info(const string &file_name)
{
	if (!file_name.empty())
	{
		fsys::path s(file_name);
		v_lock(lk, s_host_mutex);
		if (fsys::exists(s))
		{
			
			pugi::xml_document doc;
			pugi::xml_parse_result xml_ret = doc.load_file(file_name.c_str());
			if (pugi::xml_parse_status::status_ok == xml_ret.status)
			{
				pugi::xml_node host_nodes = doc.child("hosts");
				if (!host_nodes.empty())
				{
					for (pugi::xml_node_iterator item = host_nodes.begin(); item != host_nodes.end(); ++item)
					{
						string manuc = item->attribute("manufacture").as_string();
						int	   port = item->attribute("port").as_int();
						s_host_listen_lists.insert(port, manuc);
					}
				}
				/*for (pugi::xml_node_iterator item = doc.begin(); item != doc.end(); ++item)
				{
					string manuc = item->attribute("manufacture").as_string();
					int	   port = item->attribute("port").as_int();
					s_host_listen_lists.insert(port, manuc);
				}*/
				return v_OK;
			}
		}
	}
	return v_ERR_BAD_ARGUMENT;
}

void msg_distribution::do_msg_receive(const string &content, int error_code)
{
	if (m_send_session)
	{
		m_send_session->send_pkt(content);
#ifdef _DEBUG
		vvlog_i("response:" << content << "errorcode:" << error_code);
#endif
	}
}

void msg_distribution::do_msg_send(const string &content, int error_code)
{
#ifdef _DEBUG
	//m_send_session->send_pkt(content);
	//return;
	vvlog_i("start time:"<< v_get_current_time_str() << "content:" << content);
#endif
	if (v_OK != error_code)
	{
		vvlog_w("error msg:" << content << "code_num:" << error_code);
		return;
	}
	msg_xml_parser xml_parser(content);
	if (xml_parser.is_invaild())
	{
		return;
	}
	ds_tcp_session_ptr ds_tcp_client_session = nullptr;
	if (!s_host_session_lists.find(xml_parser.device_id(), ds_tcp_client_session))
	{
		//read_host_info(s_host_list_info_name);
		read_all_host_lists_info(m_host_listen_dir);
		if (0 < msg_distribution::s_host_listen_lists.size())
		{
			map<int, string> tmp = msg_distribution::s_host_listen_lists.get_copy();
			for (auto item: tmp)
			{
				if (v_str_compare_no_case(item.second, xml_parser.manufacture()))
				{
					ds_tcp_client_session.reset(new ds_tcp_session("127.0.0.1", item.first, 30000));
					ds_tcp_client_session->connect();
					//pugi::xml_document doc;
					//pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
					//decl.append_attribute("version") = "1.0";
					//decl.append_attribute("coding") = "UTF-8";
					//stringstream ss;
					//doc.save(ss);
					//string msg_str = ss.str();
					//msg_str.append("xxxxxxxx<!--msg_end-->");
					ds_tcp_client_session->subscribe_event_handler(std::bind(&msg_distribution::do_msg_receive, shared_from_this(), placeholders::_1, placeholders::_2));
					ds_tcp_client_session->wait_request();
					//ds_tcp_client_session->keep_alive(msg_str);
					//ds_tcp_client_session->wait_request();
					//ds_tcp_client_session->keep_alive();
				}
			}
		}
		s_host_session_lists.insert(xml_parser.device_id(), ds_tcp_client_session);
	}
	if (ds_tcp_client_session)
	{
		//ds_tcp_client_session->subscribe_event_handler(std::bind(&msg_distribution::do_msg_receive, shared_from_this(), placeholders::_1, placeholders::_2));
		ds_tcp_client_session->send_pkt(content);
		//ds_tcp_client_session->wait_request();
	}
	vvlog_i("end time:"<< v_get_current_time_str());
}

int msg_distribution::read_all_host_lists_info(const string &host_info_dir)
{
	if (!host_info_dir.empty())
	{
		fsys::path tmp_dir(host_info_dir);
		if (fsys::exists(tmp_dir))
		{
			boost_error_code ec;
			fsys::directory_iterator search_dir(tmp_dir, ec);
			for (auto item : search_dir)
			{
				auto file = item.path();
				if (!fsys::is_directory(file) && file.extension().string() == ".xml")
				{
					auto file_name = file.string();
					read_host_info(file_name);
				}
			}
		}
	}
	if (0 < s_host_listen_lists.size())
	{
		return s_host_listen_lists.size();
	}
	else
	{
		return -1;
	}
}
