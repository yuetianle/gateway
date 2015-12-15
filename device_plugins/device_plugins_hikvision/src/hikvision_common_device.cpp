#include "stdafx.h"
#include "hikvision_common_device.h"

atomic<bool> hikvision_common_device::s_is_init = false;
std::mutex   hikvision_common_device::s_init_mutex;
data_cache_ex<string, HandleInfo> hikvision_common_device::s_login_info;

hikvision_common_device::hikvision_common_device(const string &device_id, const int &device_type, const string &device_name, const string &device_manufacture, const string &device_ip, const long &device_port, const string &user_name, const string &user_pwd) : base_device(device_id, device_type, device_name, device_manufacture, device_ip, device_port, user_name, user_pwd)
{
	memset(&m_handle_info, 0, sizeof(HandleInfo));
}

hikvision_common_device::~hikvision_common_device()
{
}

int hikvision_common_device::init_sdk()
{
	if (!s_is_init)
	{
		v_lock(lk, s_init_mutex);
		if (!s_is_init)
		{
			if (!NET_DVR_Init())
			{
				//vvlog_e("init fail, hikerr:" << get_hik_error_info());
				//cout << "init fail" << endl;
				return convert_hikerror_to_sys(NET_DVR_GetLastError());
			}
			else
			{
				NET_DVR_SDKLOCAL_CFG localcfg;
				memset(&localcfg, 0, sizeof(NET_DVR_SDKLOCAL_CFG));
				localcfg.byEnableAbilityParse	= 1;
				localcfg.byVoiceComMode			= 0;
				localcfg.byLoginWithSimXml		= 1;
				if (!NET_DVR_SetSDKLocalConfig(&localcfg))
				{
					//vvlog_w("SetSDKLocalConfig fail," << get_hik_error_info());
					cout << "SetSDKLocalConfig fail," << endl;
				}
				cout << "init success" << endl;
				//vvlog_i("init success");
				NET_DVR_SetConnectTime(5000, 2);
				NET_DVR_SetReconnect(3, 1);
			}
		}
	}
	return v_OK;
}

std::string hikvision_common_device::get_hik_error_info()
{
	char error_msg_info[1024] = { 0 };
#ifdef _WIN32
	LONG errorcode = NET_DVR_GetLastError();
	string errormsg = NET_DVR_GetErrorMsg(&errorcode);
	sprintf_s(error_msg_info, sizeof(error_msg_info), "errorcode:%ld errormsg:%s", errorcode, errormsg.c_str());
	return error_msg_info;
#endif
}

int hikvision_common_device::convert_hikerror_to_sys(DWORD hik_err_code)
{
	return v_OK;
}

int hikvision_common_device::uninit_sdk()
{
	if (s_is_init)
	{
		v_lock(lk, s_init_mutex);
		if (s_is_init)
		{
			if (!NET_DVR_Cleanup())
			{
				vvlog_e("release fail" << get_hik_error_info());
				return convert_hikerror_to_sys(NET_DVR_GetLastError());
			}
			else
			{
				vvlog_i("uninit success.");
			}
		}
	}
	return v_OK;
}

int hikvision_common_device::login(const string &user_name, const string &password)
{
	if (m_device_ip.empty() || 0 > m_device_port || user_name.empty() || password.empty())
	{
		return v_ERR_BAD_ARGUMENT;
	}
	char session_id[100] = { 0 };
#ifdef _WIN32
	sprintf_s(session_id, sizeof(session_id), "%s:%s", m_device_ip.c_str(), user_name.c_str());
#endif
	HandleInfo login_handle;
	memset(&login_handle, 0, sizeof(HandleInfo));
	login_handle.handle_type = HANDLE_L;
	if (s_login_info.find(session_id, login_handle))
	{
		NET_DVR_SetExceptionCallBack_V30(0, nullptr, s_exception_message, this);
		m_handle_info = login_handle;
		return v_OK;
	}
	NET_DVR_DEVICEINFO_V30 hik_device_info;
	memset(&hik_device_info, 0, sizeof(hik_device_info));
	LONG ret = NET_DVR_Login_V30((char*)(m_device_ip.data()), m_device_port, (char*)user_name.data(), (char*)(password.data()), &hik_device_info);
	login_handle.handle.lhandle = ret;
	if (0 <= ret)
	{
		s_login_info.insert(session_id, login_handle);
		NET_DVR_SetExceptionCallBack_V30(0, nullptr, s_exception_message, this);
		memcpy(&m_handle_info, &login_handle, sizeof(Handle_Info));
		vvlog_i("login success, ip:" << m_device_ip << "user:" << user_name << "pwd:" << password << "handle:" << login_handle.handle.lhandle);
		DeviceUserInfo user(user_name, password);
		m_user_lists.insert(user_name, user);
		return v_OK;
	}
	else
	{
		vvlog_e("login fail, ip:" << m_device_ip << "user:" << user_name<< "pwd:" << password << "handle:" << login_handle.handle.lhandle << get_hik_error_info());
		return convert_hikerror_to_sys(NET_DVR_GetLastError());
	}
}

int hikvision_common_device::logout(const string &session_id)
{
	Handle_Info login_handle;
	memset(&login_handle, 0, sizeof(Handle_Info));
	int ret = v_OK;
	if (s_login_info.find(session_id, login_handle))
	{
		switch (login_handle.handle_type)
		{
		case HANDLE_I:
			break;
		case HANDLE_L:
			if (0 <= login_handle.handle.lhandle)
			{
				if (NET_DVR_Logout(login_handle.handle.lhandle))
				{
					vvlog_i("logout success, session_id:" << session_id << "handle:" << login_handle.handle.lhandle);
					s_login_info.remove(session_id);
					memset(&m_handle_info, 0, sizeof(Handle_Info));
					ret = v_OK;
				}
				else
				{
					vvlog_e("logout success, session_id:" << session_id << "handle:" << login_handle.handle.ihandle);
					ret = convert_hikerror_to_sys(NET_DVR_GetLastError());
				}
			}
			break;
		case HANDLE_VOID:
			break;
		default:
			break;
		}
	}
	return ret;
}

void __stdcall hikvision_common_device::s_exception_message(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
	cout << "s_excepion_msg type:" << dwType << "userId:" << lUserID << "lhandle:" << lHandle << endl;
	if (pUser)
	{
		hikvision_common_device *device_ptr = (hikvision_common_device *)pUser;
		char out_data[1024] = { 0 };
		long data_size = sizeof(out_data);
		device_ptr->get_device_status(out_data, &data_size);
		device_ptr->m_status_channge_signal(out_data, &data_size, device_ptr);
	}
	else
	{
		
	}
}

