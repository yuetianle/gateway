// device_manager_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ds_tcp_session.h"

void receive(const string &out, int error_code)
{
	pugi::xml_document doc;
	doc.load_string(out.c_str());
	doc.save_file("receive_out.xml");
	vvlog_i("receive data:" << out << "ret :" << error_code);
}
int request_url(ds_tcp_session_ptr &tcp_client, string file_name)
{
	pugi::xml_document doc;
	pugi::xml_parse_result xml_ret = doc.load_file(file_name.c_str());
	if (pugi::xml_parse_status::status_ok == xml_ret.status)
	{
		stringstream ss_1;
		doc.save(ss_1);
		string request_content = ss_1.str();
		string end_str = "<!--msg_end-->";
		request_content.append(end_str);
		vvlog_i("request content:" << request_content << "size" << request_content.length());
		//for (int i = 0; i < 4;i++)
		{
			tcp_client->send_pkt(request_content);
			//this_thread::sleep_for(chrono::seconds(10));
		}
	}
	return 0;
}
void test_hikvision(int port)
{
	//ds_tcp_session_ptr tcp_client(new ds_tcp_session("127.0.0.1", port, 30000));
	ds_tcp_session_ptr tcp_client(new ds_tcp_session("127.0.0.1", 30000, 30000));
	tcp_client->connect();
	tcp_client->subscribe_event_handler(std::bind(&receive, placeholders::_1, placeholders::_2));
	pugi::xml_document doc;
	//pugi::xml_parse_result xml_ret = doc.load_file("request.xml");
	pugi::xml_parse_result xml_ret = doc.load_file("request_hikvision.xml");
	string end_str = "<!--msg_end-->";
	if (pugi::xml_parse_status::status_ok == xml_ret.status)
	{
		stringstream ss;
		doc.save(ss);
		string request_content = ss.str();
		request_content.append(end_str);
		//for (int i = 0; i < 20;i++)
		{
			vvlog_i("send data:" << request_content << "len:" << request_content.length());
			tcp_client->send_pkt(request_content);
			//this_thread::sleep_for(chrono::seconds(1));
		}
	}
	//this_thread::sleep_for(chrono::seconds(3));
	request_url(tcp_client, "request_hikvision_uri.xml");
	tcp_client->wait_request();

}

void test_onvif(int port)
{
	ds_tcp_session_ptr tcp_client(new ds_tcp_session("127.0.0.1", port, 30000));
	tcp_client->connect();
	tcp_client->subscribe_event_handler(std::bind(&receive, placeholders::_1, placeholders::_2));
	pugi::xml_document doc;
	//pugi::xml_parse_result xml_ret = doc.load_file("request.xml");
	pugi::xml_parse_result xml_ret = doc.load_file("request_onvif.xml");
	string end_str = "<!--msg_end-->";
	if (pugi::xml_parse_status::status_ok == xml_ret.status)
	{
		stringstream ss;
		doc.save(ss);
		string request_content = ss.str();
		request_content.append(end_str);
		//for (int i = 0; i < 20;i++)
		{
			vvlog_i("send data:" << request_content << "len:" << request_content.length());
			tcp_client->send_pkt(request_content);
			//this_thread::sleep_for(chrono::seconds(1));
		}
	}
	//this_thread::sleep_for(chrono::seconds(3));
	request_url(tcp_client, "request_onvif_uri.xml");
	tcp_client->wait_request();
}

void test_psia(int port)
{
	ds_tcp_session_ptr tcp_client(new ds_tcp_session("127.0.0.1", port, 30000));
	tcp_client->connect();
	tcp_client->subscribe_event_handler(std::bind(&receive, placeholders::_1, placeholders::_2));
	pugi::xml_document doc;
	//pugi::xml_parse_result xml_ret = doc.load_file("request.xml");
	pugi::xml_parse_result xml_ret = doc.load_file("request_psia.xml");
	string end_str = "<!--msg_end-->";
	if (pugi::xml_parse_status::status_ok == xml_ret.status)
	{
		stringstream ss;
		doc.save(ss);
		string request_content = ss.str();
		request_content.append(end_str);
		{
			vvlog_i("send data:" << request_content << "len:" << request_content.length());
			tcp_client->send_pkt(request_content);
		}
	}
	request_url(tcp_client, "request_psia_uri.xml");
	tcp_client->wait_request();

}


data_cache_ex<int, string>s_host_listen_lists;
int read_host_info(const string &file_name)
{

	fsys::path cur = fsys::current_path();
	cur /= "hostboot\\" + file_name;
	if (!file_name.empty())
	{
		fsys::path s(file_name);
		//v_lock(lk, s_host_mutex);
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
				return v_OK;
			}
		}
	}
	return v_ERR_BAD_ARGUMENT;
}

int test(int choice)
{
	if (0 == choice)
	{
		test_hikvision(30000);
	}
	else if (1 == choice)
	{
		test_onvif(30000);
	} else if (2 == choice)
	{
		test_psia(30000);
	}
	cin.get();
	return v_OK;
}
int main(int argc, char* argv[])
{
	test(0);
	/*pugi::xml_document doc;
	pugi::xml_node decl_node = doc.prepend_child(pugi::node_declaration);
	decl_node.append_attribute("version") = "1.0";
	decl_node.append_attribute("encoding") = "utf-8";
	auto host_nodes = doc.child("hosts");
	if (host_nodes.empty())
	{
		host_nodes = doc.append_child("hosts");
	}
	pugi::xml_node host_node = host_nodes.append_child("host");
	auto manu_attr = host_node.attribute("manufacture");
	if (manu_attr.empty())
	{
		manu_attr = host_node.append_attribute("manufacture");
		manu_attr.set_value("ddd");
	}
	auto port_attr = host_node.attribute("port");
	if (port_attr.empty())
	{
		port_attr = host_node.append_attribute("port");
		port_attr.set_value(2);
	}
	doc.save_file("host_lists.xml");*/
	map<int, string> tmp = s_host_listen_lists.get_copy();
	int port = 0;
	for (auto item : tmp)
	{
		/*if (v_str_compare_no_case(item.second, "onvif"))
		{
			port = item.first;
			test_onvif(port);
			break;
		}*/
		if (v_str_compare_no_case(item.second, "hikvision"))
		{
			port = item.first;
			test_hikvision(port);
			break;
		}
		/*if (v_str_compare_no_case(item.second, "psia"))
		{
			port = item.first;
			test_psia(port);
			break;
		}*/
	}
	////ds_tcp_session_ptr tcp_client(new ds_tcp_session("127.0.0.1", 30000));
	//ds_tcp_session_ptr tcp_client(new ds_tcp_session("127.0.0.1", port, 30000));
	//tcp_client->connect();
	//tcp_client->subscribe_event_handler(std::bind(&receive, placeholders::_1, placeholders::_2));
	////tcp_client->keep_alive();
	//pugi::xml_document doc;
	////pugi::xml_parse_result xml_ret = doc.load_file("request.xml");
	//pugi::xml_parse_result xml_ret = doc.load_file("request_onvif.xml");
	//string end_str = "<!--msg_end-->";
	//if (pugi::xml_parse_status::status_ok == xml_ret.status)
	//{
	//	stringstream ss;
	//	doc.save(ss);
	//	string request_content = ss.str();
	//	request_content.append(end_str);
	//	//for (int i = 0; i < 20;i++)
	//	{
	//		vvlog_i("send data:" << request_content << "len:" << request_content.length());
	//		tcp_client->send_pkt(request_content);
	//		//this_thread::sleep_for(chrono::seconds(1));
	//	}
	//}
	////this_thread::sleep_for(chrono::seconds(3));
	//request_url(tcp_client);
	//tcp_client->wait_request();
	cin.get();
	return 0;
}

