#include "stdafx.h"
#include "device_host_manager.h"

device_host_manager::device_host_manager()
	:m_is_start(false)
{
	m_server.reset(new ds_tcp_server(30000, 30000));
	m_server->start();
	m_host_loader.reset(new host_bootloader());
	m_msg_distribute.reset(new msg_distribution());
}

device_host_manager::~device_host_manager()
{
	m_server->stop();
}

int device_host_manager::start()
{
	int ec = 0;
	if (m_host_loader && !m_host_loader->is_start())
	{
		ec = m_host_loader->start_hosts();
	}
	if (m_msg_distribute && m_server)
	{
		m_server->subscribe_new_session(std::bind(&msg_distribution::do_msg_distribute, m_msg_distribute, placeholders::_1, placeholders::_2));
	}
	if (!ec)
	{
		m_is_start = true;
		return v_OK;
	}
	else
	{
		m_is_start = false;
		return v_ERR_BAD_DEVICE;
	}
}

int device_host_manager::stop()
{
	if (m_host_loader && m_host_loader->is_start())
	{
		return m_host_loader->stop_hosts();
	}
	return v_ERR_BAD_DEVICE;
}
