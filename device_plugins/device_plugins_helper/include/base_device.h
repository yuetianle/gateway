#ifndef _BASE_DEVICE_H_
#define _BASE_DEVICE_H_
#pragma once
#include "base_device_data_types_defs.h"
#include "base_device_media.h"
class base_device
{
public:
	static int init_sdk();
	static int uninit_sdk();
	virtual int login(const string &user_name, const string &password) = 0;
	virtual int logout(const string &session_id="") = 0;
	virtual int get_device_status(char *out_data, long *out_len) = 0;
	virtual int get_device_info(char *out_data, long *out_len) = 0;

	virtual int add_user(const char *user, const char *password, const int &level = 0) = 0;
	virtual int delete_user(const char *user) = 0;
	virtual int alter_user(const char *old_name, const char *new_name, char *password, const int &level /* = 0 */) = 0;


	boost::signals2::connection wrap_device_status_change(const DeviceStatusSignal::slot_type &slot);
	void unwrap_device_status_change();

	virtual base_device_media* create_media_service() = 0;
	base_device_media* get_media_service() { return m_device_media; };
public:
	base_device(const string &device_id, const int &device_type, const string &device_name, const string &device_manufacture, const string &device_ip, const long &device_port, const string &user_name, const string &user_pwd);
	base_device(const DeviceInfo &info, const DeviceUserInfo &user_info);
	virtual ~base_device();
public:
	string device_id() const { return m_device_id; }
	int	   device_type() const { return m_device_type; }
	string device_name() const { return m_device_name; }
	string device_manufacture() const { return m_device_manufacture; }
	string device_ip() const { return m_device_ip; }
	long device_port() const { return m_device_port; }

	DeviceStatusSignal* status_signal(){ return &m_status_channge_signal; }
protected:
	DeviceStatusSignal m_status_channge_signal;
	DeviceInfo m_device_info;
	string m_device_id;
	int    m_device_type;
	string m_device_name;
	string m_device_manufacture;
	string m_device_ip;
	long   m_device_port;
	data_cache_ex<string, DeviceUserInfo> m_user_lists;
	base_device_media* m_device_media;
	vistek_device_status m_device_cur_status;
};
#endif