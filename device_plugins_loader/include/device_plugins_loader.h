#ifndef _DEVICE_GATEWAY_LOADERV1_H_
#define _DEVICE_GATEWAY_LOADERV1_H_

#ifdef DEVICE_PLUGINS_LOADER_EXPORTS
#define DEVICE_PLUGINS_LOADER_API __declspec(dllexport)
#else
#define DEVICE_PLUGINS_LOADER_API __declspec(dllimport)
#endif
#pragma once
#include "module_helper.h"
#include "device_uri_parser.h"
#include "device_plugins_helper.h"
enum DLL_INDEX
{
	DLL_INDEX_INVAILD = -1,
	DLL_INDEX_HIKVISION= 0,	
	DLL_INDEX_DAHUA = 1,	
	DLL_INDEX_PSIA = 2,	
	DLL_INDEX_ONVIF = 4,	
	DLL_INDEX_GB28181 = 8,	
};

data_cache_ex < string, int > s_device_lists;/* << deviceid:��̬������*/
data_cache_ex < string, int > s_psia_device_lists;/* << deviceid:��̬������*/
data_cache_ex < string, int > s_onvif_device_lists;/* << deviceid:��̬������*/
data_cache_ex < string, int > s_gb28181_device_lists;/* << deviceid:��̬������*/
data_cache_ex < int, proc_util_func_wrap_t > s_fun_wrap;/* ��̬������:��̬�����*/

#ifdef _WIN32
#define VISTEK_STDCALL __stdcall
#elif defined(linux)
#define VISTEK_STDCALL 
#endif
typedef function<int(char*, long*, void*)> status_change_call_back;
extern "C"
{
/**
 * @brief ע���豸.
 * @param [in] protocol_type Э������.
 * @param [in] device_id �豸ΨһID.
 * @param [in] device_type �豸����.
 * @param [in] device_name �豸����.
 * @param [in] device_manfacture �豸��������.
 * @param [in] device_ip �豸IP��ַ.
 * @param [in] device_port �豸�˿�.
 * @param [in] user_name �û���.
 * @param [in] user_pwd �û�����.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL register_gateway_device(v_device_protocol_type protocol_type, const char *device_id, v_device_type device_type, const char *device_name, v_device_manufacturer_type device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd);

/**
 * @brief ��ע���豸.
 * @param [in] �豸ΨһID.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL unregister_gateway_device(const char *device_id);

/**
 * @brief ִ���豸�������. 
 * @param [in] device_id �豸ΨһID.
 * @param [in] cmd_uri �豸URI�����ַ���.
 * @param [in] cmd_extcontent �������.
 * @param [in|out] out_xml in��������|out����ִ�з�������.
 * @param [in|out] out_size in�������ݳ���|ʵ�ʷ������ݳ���.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL device_exec_cmd(const char* device_id, const char *cmd_uri, void *cmd_extcontent, char *out_data, long *out_size);
/**
 * @brief ��ȡ�豸״̬�ص���Ϣ.
 * @param [in] device_id �豸ΨһID.
 * @param [in] call_back �ص�����ָ��.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL device_wrap(const char *device_id, status_change_call_back fp_call_back);
/**
 * @brief �����豸���.
 * @param [in] manu �������.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL load_device_plugin(v_device_manufacturer_type manu);

/**
 * @brief ж���豸���.
 * @param [in] manu �������.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL unload_device_plugin(v_device_manufacturer_type manu);
}
#endif
