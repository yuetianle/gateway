#ifndef _DEVICE_HOST_MANAGER_H_
#define _DEVICE_HOST_MANAGER_H_
#pragma once
#include "ds_tcp_server.h"
#include "host_bootloader.h"
#include "msg_manager.h"
#include "msg_distribution.h"
class device_host_manager :public enable_shared_from_this<device_host_manager>
{
public:
	device_host_manager();
	~device_host_manager();
public:
	int start();
	int stop();
	bool is_start() { return m_is_start; }
private:
	host_bootloader_ptr		m_host_loader;
	ds_tcp_server_ptr		m_server;
	msg_distribution_ptr	m_msg_distribute;
	msg_manager_ptr			m_msg_manager;
	atomic<bool>			m_is_start;
};
typedef shared_ptr<device_host_manager> device_host_manager_ptr;
#endif

