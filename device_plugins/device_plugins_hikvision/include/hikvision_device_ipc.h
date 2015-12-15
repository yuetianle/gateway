#ifndef _HIKVISION_DEVICE_IPC_H_
#define _HIKVISION_DEVICE_IPC_H_
#pragma once
#include "base_device_media.h"
#include "hikvision_common_device.h"
#include "hikvision_device_types.h"
class hikvision_device_ipc :
	public hikvision_common_device
{
public:
	hikvision_device_ipc(const string &device_id, const int &device_type, const string &device_name, const string &device_manufacture, const string &device_ip, const long &device_port, const string &user_name, const string &user_pwd);
	~hikvision_device_ipc();

	//static void __stdcall s_exception_message(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser);
	virtual int get_device_status(char *out_data, long *out_len)override;
	virtual int get_device_info(char *out_data, long *out_len)override;
	virtual int add_user(const char *user, const char *password, const int &level = 0)override;
	virtual int delete_user(const char *user)override;
	virtual int alter_user(const char *old_name, const char *new_name, char *password, const int &level /* = 0 */)override;
	virtual base_device_media* create_media_service()override;
private:
	int _createStatusXml(const bool &status, char *out_data, long *out_size);
	/**
	 * @brief 检查用户是否存在.
	 * @param [in] user 用户.
	 * @param [out] hik_user_info 设备所有用户信息.
	 * @param [out] hik_user_index 此用户的索引.
	 * @return 0:success else return error code.
	 */
	int _check_user_exist(const char *user, NET_DVR_USER_V40 *hik_user_info, int *hik_user_index);
};
//////////////////////////////////////////////////////////////////////////
class hikvision_device_media :
	public base_device_media
{
public:
	hikvision_device_media(hikvision_device_ipc *device);
	~hikvision_device_media();

	virtual int get_stream_urls(const int &stream_id, char *out_data, long *out_len)override;
private:

	int _parser_hik_stream_xml(const char *in_xml, HIKStreamInfoVec *stream_info);
	std::string _create_stream_url(char *out_xml, long *out_size, const HIKStreamInfoVec &stream_info, const int &stream_id/*0xff*/);
};
#endif

