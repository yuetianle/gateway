#ifndef _DEVICE_HOST_H_
#define _DEVICE_HOST_H_
#pragma once
#include "msg_processer.h"
#include "ds_tcp_server.h"
#include "device_plugins_loader.h"
#include <boost/interprocess/sync/file_lock.hpp>
#define MAX_MSG_LEN 1*1024*1024

class device_host: public enable_shared_from_this<device_host>
{
public:
	device_host(const string manuc);
	~device_host();
public:
	static int write_host_lists(const string &file_name);
	static int read_host_lists(pugi::xml_document &doc, const string &file_name);
public:
	int start();
	int stop();
	bool is_start();
	uint16_t listen_port() { return m_ds_tcp_server->get_listen_port(); }
private:
	int _write_host_listen_ports(const string &manuc, const uint16_t &port);
	void _do_msg(const ds_tcp_session_ptr& session, const string &msg);
	void _exe_msg_cmd(const string &msg_cmd, int error_code);
private:
	atomic<bool> m_is_start;
	shared_ptr<ds_tcp_server> m_ds_tcp_server;
	string m_manufacture;/*<< 厂商类型*/
	static pugi::xml_document s_host_list;
	static mutex			  s_host_mutex;
	ds_tcp_session_ptr		  m_send_session;
	string					  m_send_content;
	string					  m_msg_delimiter = "<!--msg_end-->";
	const string m_host_listen_dir = "hostboot";
	pugi::xml_document m_host_lists_doc;
	FILE *m_exist_host_file;
	//string m_exist_host_file_path;
	//static boost::interprocess::file_lock  s_flock;
};

#endif
