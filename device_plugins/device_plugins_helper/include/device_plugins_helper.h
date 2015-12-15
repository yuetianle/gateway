#ifndef _DEVICE_PLUGINS_HELPER_H_
#define _DEVICE_PLUGINS_HELPER_H_

#pragma once
#include "base_device.h"
#ifdef DEVICE_PLUGINS_HELPER_EXPORTS
#define DEVICE_PLUGINS_HELPER_API __declspec(dllexport)
#else
#define DEVICE_PLUGINS_HELPER_API __declspec(dllimport)
#endif
#pragma once

	/**
	 * @brief 
	 * @param [in] device_id 设备唯一ID.
	 * @return 0:success else return error code.
	 */
extern "C"
{

#pragma region BASE

	/*
	 * @brief 注册设备.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] device_type 设备类型, @reference v_device_type.
	 * @param [in] device_name 设备名称.
	 * @param [in] device_manufacture 设备厂商 @reference v_device_manufacture_type.
	 * @param [in] device_ip 设备IP.
	 * @param [in] device_port 设备端口.
	 * @param [in] user_name 用户名.
	 * @param [in] user_pwd 用户密码.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int register_device(const char *device_id, const int &device_type, const char *device_name, const int &device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd);

	/* 
	 * @brief 反注册设备.
	 * @param [in] 设备唯一ID.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int unregister_device(const char *device_id);
#pragma endregion BASE

#pragma region SDK
	/*
	 * @brief 获取设备实时流地址.
	 * @param [out] out_data
	 * @param [in|out] out_len in:分配内存 out:实际数据长度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_type 码流类型, @reference MediaStreamType 
	 * @param [in] channel 通道号
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_stream_url(char *out_data, long *out_len, const char *device_id, const int &stream_type, const int &channel = 0);

	/*
	 * @brief 获取设备当前状态.
	 * @param [out] out_data
	 * @param [in|out] out_len in:分配内存 out:实际数据长度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] channel 通道号
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_device_status(char *out_data, long *out_len, const char *device_id, const int &channel = 0);

	DEVICE_PLUGINS_HELPER_API int test();
	DEVICE_PLUGINS_HELPER_API int test_ext();
	/**
	 * @brief 添加设备用户.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] user_name 用户名.
	 * @param [in] password 密码.
	 * @param [in] level 控制权限.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int add_user(const char *device_id, const char *user_name, const char *password, const int &level = 0);

	/**
	 * @brief 删除用户.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] user_name 用户名.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int delete_user(const char *device_id, const char *user_name);
	
	/**
	 * @brief 修改用户.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] old_name 需要修改的用户名.
	 * @param [in] new_name 新用户名.
	 * @param [in] password 密码.
	 * @param [in] level 权限等级.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int alter_user(const char *devcie_id, const char *old_name, const char *new_name, const char *password, const int &level = 0);

	/**
	 * @brief 获取设备状态变化.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in|out]  in:函数指针 out:状态信息的输出
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int wrap_status_func(const char *device_id, function<int (char *out_data, long *out_len, void *obj)>);

	/**
	 * @brief 获取当前分辨率.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_resolution(const char *device_id, const int &stream_id, char *out_data, long *out_len);

	/**
	 * @brief 设置当前分辨率.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] content 分辨率字符串.@example 234*1024
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_resolution(const char *device_id, const int &stream_id, const char *content);

	/**
	 * @brief 获取码率类型.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_stream_bit_rate_type(const char *device_id, const int &stream_id, char *out_data, long *out_len);

	/**
	 * @brief 设置码率类型.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] type 码率类型. @reference MeidaStramBitRateType.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_stream_bit_rate_type(const char *device_id, const int &stream_id, int type);

	/**
	 * @brief 获取视频帧率.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_stream_fps(const char *device_id, const int &stream_id, char *out_data, long *out_len);
	
	/**
	 * @brief 设置视频帧率.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] stream_fps 帧率.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_stream_fps(const char *device_id, const int &stream_id, const int stream_fps);

	/**
	 * @brief 获取视频编码类型.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_video_codec(const char *device_id, const int &stream_id, char *out_data, long *out_len);

	/**
	 * @brief 设置视频编码类型.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] codec_type 视频编码类型 @reference vitesk_codec_type.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_video_codec(const char *device_id, const int &stream_id, const int &codec_type);

	/**
	 * @brief 获取I帧间隔.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_iframe_interval(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);

	/**
	 * @brief 设置I帧间隔.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] iframe_interval I帧间隔.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_iframe_interval(const char *device_id, const int &stream_id, const int iframe_interval);

	/**
	 * @brief 获取音频编码类型.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_audio_codec(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);
	/**
	 * @brief 获取音频编码类型.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] codec_type 音频编码类型.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_audio_codec(const char *device_id, const int &stream_id, const char *codec_type);
	
	/**
	 * @brief 获取音频采样率.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_audio_samplingrate(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);

	/**
	 * @brief 设置音频采样率.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] samplingrate 音频采样率.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_audio_samplingrate(const char *device_id, const int &stream_id, const char *samplingrate);
	
	/**
	 * @brief 获取音频比特率.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_audio_bitrates(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);
	/**
	 * @brief 设置音频比特率.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] bitrates 比特率.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_audio_bitrates(const char *device_id, const int &stream_id, const char *bitrates);

	/**
	 * @brief 获取音频采样精度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [out] out_data 返回的分辨率数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_audio_sampling_accuracy(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);

	/**
	 * @brief 设置音频采样精度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] stream_id 码流类型 @reference MediaStreamType.
	 * @param [in] sampling_accuar 音频采样精度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_audio_sampling_accuracy(const char *device_id, const int &stream_id, const char *bitrates);

	/**
	 * @brief 重启设备.
	 * @param [in] device_id 设备唯一ID.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int reboot_device(const char *device_id);

	/**
	 * @brief 设备关机.
	 * @param [in] device_id 设备唯一ID.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int shutdown_device(const char *device_id);

	/**
	 * @brief 设备开机.
	 * @param [in] device_id 设备唯一ID.
	 * @return 0:success else return error code.
	 */

	DEVICE_PLUGINS_HELPER_API int startup_device(const char *device_id);
	/**
	 * @brief 获取设备信息.
	 * @param [in] device_id 设备唯一ID.
	 * @param [out] out_data 返回的数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_device_info(const char *device_id, char *out_data, long *out_data_len);
	/**
	 * @brief 
	 * @param [in] device_id 设备唯一ID.
	 * @param [out] out_data 返回的数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_device_ablitity(const char *device_id, char *out_data, long *out_data_len);

	
	/**
	 * @brief 获取图像亮度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [out] out_data 返回的数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_bright(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief 设置图像亮度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] bright 亮度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_bright(const char *device_id, const char *bright);

	/**
	 * @brief 获取图像对比度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [out] out_data 返回的数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_contrast(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief 设置图像对比度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] contrast 图像对比度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_contrast(const char *device_id, const char* contrast);

	/**
	 * @brief 获取图像饱和度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [out] out_data 返回的数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_saturation(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief 设置图像饱和度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] saturation 图像饱和度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_saturation(const char *device_id, const char *saturation);

	/**
	 * @brief 获取图像锐度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [out] out_data 返回的数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_hue(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief 设置图像锐度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] hue 图像色度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_hue(const char *device_id, const char *hue);

	/**
	 * @brief 获取图像锐度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [out] out_data 返回的数据信息.
	 * @param [out] out_data_len 返回数据的实际长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_sharpess(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief 设置图像的锐度.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] sharpness 图像锐度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_sharpness(const char *device_id, const char *sharpness);
#pragma endregion SDK

#pragma region ONVIF
	/**
	 * @brief ONVIF协议请求名利.
	 * @param [in]
	 */
	DEVICE_PLUGINS_HELPER_API int request_onvif_cmd(const char *device_id, const int cmd_index, const char *uri, char *out_data, long *out_data_len);
	/**
	 * @brief ONVIF协议请求命令.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] cmd_name 命令名称.
	 * @param [in] in_params 命令参数.
	 * @param [out] out_data 返回数据.
	 * @param [out] out_data_len 返回数据长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int request_onvif_cmd_ext(const char *device_id, const string cmd_name, const char *in_params, char *out_data, long *out_data_len);

#pragma endregion ONVIF

#pragma region PSIA
	/**
	 * @brief PSIA协议请求命令.
	 * @param [in] device_id 设备唯一ID.
	 * @param [in] method [GET(0)|PUT(1)|POST(2)|DEL(3)] @reference psia_method
	 * @param [in] uri PSIA协议URI地址.
	 * @param [out] out_data 返回数据.
	 * @param [out] out_data_len 返回数据长度.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int request_pisa_cmd(const char *device_id, const int &method, const char *uri, char *out_data, long *out_data_len);
#pragma endregion PSIA

#pragma region GB28181
#pragma endregion GB28181

#pragma region HDCCTV
#pragma endregion HDCCTV
}

typedef struct proc_util_wrap
{
#define WRAP_DLL(f) f = (decltype(f))GetProcAddress(dll, #f);
#define EXPORT_FUNCTION(f) decltype(&::f) f;		 
	void wrap(HMODULE dll)
	{
		memset(reserved, 0, sizeof(reserved));
		if (dll)
		{
			WRAP_DLL(register_device);
			WRAP_DLL(unregister_device);
			WRAP_DLL(get_stream_url);
			WRAP_DLL(get_device_status);
			WRAP_DLL(request_pisa_cmd);
			WRAP_DLL(request_onvif_cmd);
			WRAP_DLL(request_onvif_cmd_ext);

			WRAP_DLL(add_user);
			WRAP_DLL(delete_user);
			WRAP_DLL(alter_user);

			WRAP_DLL(get_resolution);
			WRAP_DLL(set_resolution);
			WRAP_DLL(get_stream_bit_rate_type);
			WRAP_DLL(set_stream_bit_rate_type);
			WRAP_DLL(get_stream_fps);
			WRAP_DLL(set_stream_fps);
			WRAP_DLL(get_video_codec);
			WRAP_DLL(set_video_codec);
			WRAP_DLL(get_iframe_interval);
			WRAP_DLL(set_iframe_interval);

			WRAP_DLL(wrap_status_func);
			WRAP_DLL(test);
		}
	}
	void unwrap()
	{
		register_device		= 0;
		unregister_device	= 0;
		get_stream_url		= 0;
		get_device_status	= 0;

		request_pisa_cmd	= 0;
		request_onvif_cmd	= 0;
		request_onvif_cmd_ext = 0;

		alter_user			= 0;
		add_user			= 0;
		delete_user			= 0;
		get_resolution		= 0; 
		set_resolution		= 0;
		get_stream_bit_rate_type = 0;
		set_stream_bit_rate_type = 0;
		get_stream_fps		= 0;
		set_stream_fps		= 0;
		get_video_codec		= 0;
		set_video_codec		= 0;
		get_iframe_interval = 0;
		set_iframe_interval = 0;
		wrap_status_func	= 0;
		test				= 0;
	}
	bool is_good()
	{
		if (register_device && unregister_device && get_stream_url && get_device_status && add_user && delete_user && alter_user && wrap_status_func)
		{
			return true;
		}
		return false;
	}
	bool is_video_good()
	{
		if (get_resolution && set_resolution && get_stream_bit_rate_type  && set_stream_bit_rate_type &&get_stream_fps &&set_stream_fps &&get_video_codec &&set_video_codec &&get_iframe_interval &&set_iframe_interval)
		{
			return true;
		}
		return false;
	}
	bool is_psia_good()
	{
		if (register_device && unregister_device && request_pisa_cmd&& add_user && delete_user && alter_user)
		{
			return true;
		}
		return false;
	}
	bool is_onvif_good()
	{
		if (register_device && unregister_device && request_onvif_cmd && request_onvif_cmd_ext)
		{
			return true;
		}
		return false;
	}
	union
	{
		struct {
			EXPORT_FUNCTION(register_device);
			EXPORT_FUNCTION(unregister_device);
			EXPORT_FUNCTION(get_stream_url);
			EXPORT_FUNCTION(get_device_status);
			EXPORT_FUNCTION(request_pisa_cmd);
			EXPORT_FUNCTION(request_onvif_cmd);
			EXPORT_FUNCTION(request_onvif_cmd_ext);
			
			EXPORT_FUNCTION(add_user);
			EXPORT_FUNCTION(alter_user);
			EXPORT_FUNCTION(delete_user);

			EXPORT_FUNCTION(get_resolution);
			EXPORT_FUNCTION(set_resolution);
			EXPORT_FUNCTION(get_stream_bit_rate_type);
			EXPORT_FUNCTION(set_stream_bit_rate_type);
			EXPORT_FUNCTION(get_stream_fps);
			EXPORT_FUNCTION(set_stream_fps);
			EXPORT_FUNCTION(get_video_codec);
			EXPORT_FUNCTION(set_video_codec);
			EXPORT_FUNCTION(get_iframe_interval);
			EXPORT_FUNCTION(set_iframe_interval);

			EXPORT_FUNCTION(wrap_status_func);
			EXPORT_FUNCTION(test);
		};
		char reserved[4096];
	};
}proc_util_func_wrap_t;
#endif
