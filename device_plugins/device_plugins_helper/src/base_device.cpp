#include "stdafx.h"
#include "base_device.h"

base_device::base_device(const string &device_id, const int &device_type, const string &device_name, const string &device_manufacture, const string &device_ip, const long &device_port, const string &user_name, const string &user_pwd)
	:m_device_id(device_id), m_device_type(device_type), m_device_name(device_name), m_device_manufacture(device_manufacture), m_device_ip(device_ip), m_device_port(device_port), m_device_cur_status(vds_OK)
{
	DeviceUserInfo user_info(user_name, user_pwd);
	m_user_lists.insert(user_name, user_info);
}

base_device::base_device(const DeviceInfo &info, const DeviceUserInfo &user_info)
	:m_device_cur_status(vds_OK)
{
	memcpy(&m_device_info, &info, sizeof(DeviceInfo));
	m_user_lists.insert(user_info.user_name(), user_info);
}

base_device::~base_device()
{
}

boost::signals2::connection base_device::wrap_device_status_change(const DeviceStatusSignal::slot_type &slot)
{
	return m_status_channge_signal.connect(slot);
}

void base_device::unwrap_device_status_change()
{
	m_status_channge_signal.disconnect_all_slots();
}

