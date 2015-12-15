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

data_cache_ex < string, int > s_device_lists;/* << deviceid:动态库索引*/
data_cache_ex < string, int > s_psia_device_lists;/* << deviceid:动态库索引*/
data_cache_ex < string, int > s_onvif_device_lists;/* << deviceid:动态库索引*/
data_cache_ex < string, int > s_gb28181_device_lists;/* << deviceid:动态库索引*/
data_cache_ex < int, proc_util_func_wrap_t > s_fun_wrap;/* 动态库索引:动态库对象*/

#ifdef _WIN32
#define VISTEK_STDCALL __stdcall
#elif defined(linux)
#define VISTEK_STDCALL 
#endif
typedef function<int(char*, long*, void*)> status_change_call_back;
extern "C"
{
/**
 * @brief 注册设备.
 * @param [in] protocol_type 协议类型.
 * @param [in] device_id 设备唯一ID.
 * @param [in] device_type 设备类型.
 * @param [in] device_name 设备名称.
 * @param [in] device_manfacture 设备厂商类型.
 * @param [in] device_ip 设备IP地址.
 * @param [in] device_port 设备端口.
 * @param [in] user_name 用户名.
 * @param [in] user_pwd 用户密码.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL register_gateway_device(v_device_protocol_type protocol_type, const char *device_id, v_device_type device_type, const char *device_name, v_device_manufacturer_type device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd);

/**
 * @brief 反注册设备.
 * @param [in] 设备唯一ID.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL unregister_gateway_device(const char *device_id);

/**
 * @brief 执行设备相关命令. 
 * @param [in] device_id 设备唯一ID.
 * @param [in] cmd_uri 设备URI命令字符串.
 * @param [in] cmd_extcontent 额外参数.
 * @param [in|out] out_xml in申请数据|out命令执行返回数据.
 * @param [in|out] out_size in申请数据长度|实际返回数据长度.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL device_exec_cmd(const char* device_id, const char *cmd_uri, void *cmd_extcontent, char *out_data, long *out_size);
/**
 * @brief 获取设备状态回掉信息.
 * @param [in] device_id 设备唯一ID.
 * @param [in] call_back 回掉函数指针.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL device_wrap(const char *device_id, status_change_call_back fp_call_back);
/**
 * @brief 加载设备插件.
 * @param [in] manu 插件厂商.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL load_device_plugin(v_device_manufacturer_type manu);

/**
 * @brief 卸载设备插件.
 * @param [in] manu 插件厂商.
 * @return 0:success eles return error code.
 */
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL unload_device_plugin(v_device_manufacturer_type manu);
}
#endif
