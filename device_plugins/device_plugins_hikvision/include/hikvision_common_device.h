#ifndef _HIKVISION_COMMON_DEVICE_H_
#define _HIKVISION_COMMON_DEVICE_H_
#pragma once
#include "base_device.h"
class hikvision_common_device :
	public base_device
{
public:
	hikvision_common_device(const string &device_id, const int &device_type, const string &device_name, const string &device_manufacture, const string &device_ip, const long &device_port, const string &user_name, const string &user_pwd);
	virtual ~hikvision_common_device();

	static void __stdcall s_exception_message(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser);
	static string get_hik_error_info();
	static int convert_hikerror_to_sys(DWORD hik_err_code);
public:
	static int init_sdk();
	static int uninit_sdk();
	virtual int login(const string &user_name, const string &password)override;
	virtual int logout(const string &session_id="")override;
	//virtual base_device_media* create_media_service()override;
	static atomic<bool> s_is_init;
	static std::mutex   s_init_mutex;
	static data_cache_ex<string, Handle_Info> s_login_info;
	Handle_Info m_handle_info;
public:
	Handle_Info get_login_handle() const { return m_handle_info; }
};
#endif

