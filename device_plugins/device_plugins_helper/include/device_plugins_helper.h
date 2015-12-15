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
	 * @param [in] device_id �豸ΨһID.
	 * @return 0:success else return error code.
	 */
extern "C"
{

#pragma region BASE

	/*
	 * @brief ע���豸.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] device_type �豸����, @reference v_device_type.
	 * @param [in] device_name �豸����.
	 * @param [in] device_manufacture �豸���� @reference v_device_manufacture_type.
	 * @param [in] device_ip �豸IP.
	 * @param [in] device_port �豸�˿�.
	 * @param [in] user_name �û���.
	 * @param [in] user_pwd �û�����.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int register_device(const char *device_id, const int &device_type, const char *device_name, const int &device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd);

	/* 
	 * @brief ��ע���豸.
	 * @param [in] �豸ΨһID.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int unregister_device(const char *device_id);
#pragma endregion BASE

#pragma region SDK
	/*
	 * @brief ��ȡ�豸ʵʱ����ַ.
	 * @param [out] out_data
	 * @param [in|out] out_len in:�����ڴ� out:ʵ�����ݳ���.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_type ��������, @reference MediaStreamType 
	 * @param [in] channel ͨ����
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_stream_url(char *out_data, long *out_len, const char *device_id, const int &stream_type, const int &channel = 0);

	/*
	 * @brief ��ȡ�豸��ǰ״̬.
	 * @param [out] out_data
	 * @param [in|out] out_len in:�����ڴ� out:ʵ�����ݳ���.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] channel ͨ����
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_device_status(char *out_data, long *out_len, const char *device_id, const int &channel = 0);

	DEVICE_PLUGINS_HELPER_API int test();
	DEVICE_PLUGINS_HELPER_API int test_ext();
	/**
	 * @brief ����豸�û�.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] user_name �û���.
	 * @param [in] password ����.
	 * @param [in] level ����Ȩ��.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int add_user(const char *device_id, const char *user_name, const char *password, const int &level = 0);

	/**
	 * @brief ɾ���û�.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] user_name �û���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int delete_user(const char *device_id, const char *user_name);
	
	/**
	 * @brief �޸��û�.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] old_name ��Ҫ�޸ĵ��û���.
	 * @param [in] new_name ���û���.
	 * @param [in] password ����.
	 * @param [in] level Ȩ�޵ȼ�.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int alter_user(const char *devcie_id, const char *old_name, const char *new_name, const char *password, const int &level = 0);

	/**
	 * @brief ��ȡ�豸״̬�仯.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in|out]  in:����ָ�� out:״̬��Ϣ�����
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int wrap_status_func(const char *device_id, function<int (char *out_data, long *out_len, void *obj)>);

	/**
	 * @brief ��ȡ��ǰ�ֱ���.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_resolution(const char *device_id, const int &stream_id, char *out_data, long *out_len);

	/**
	 * @brief ���õ�ǰ�ֱ���.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] content �ֱ����ַ���.@example 234*1024
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_resolution(const char *device_id, const int &stream_id, const char *content);

	/**
	 * @brief ��ȡ��������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_stream_bit_rate_type(const char *device_id, const int &stream_id, char *out_data, long *out_len);

	/**
	 * @brief ������������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] type ��������. @reference MeidaStramBitRateType.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_stream_bit_rate_type(const char *device_id, const int &stream_id, int type);

	/**
	 * @brief ��ȡ��Ƶ֡��.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_stream_fps(const char *device_id, const int &stream_id, char *out_data, long *out_len);
	
	/**
	 * @brief ������Ƶ֡��.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] stream_fps ֡��.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_stream_fps(const char *device_id, const int &stream_id, const int stream_fps);

	/**
	 * @brief ��ȡ��Ƶ��������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_video_codec(const char *device_id, const int &stream_id, char *out_data, long *out_len);

	/**
	 * @brief ������Ƶ��������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] codec_type ��Ƶ�������� @reference vitesk_codec_type.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_video_codec(const char *device_id, const int &stream_id, const int &codec_type);

	/**
	 * @brief ��ȡI֡���.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_iframe_interval(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);

	/**
	 * @brief ����I֡���.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] iframe_interval I֡���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_iframe_interval(const char *device_id, const int &stream_id, const int iframe_interval);

	/**
	 * @brief ��ȡ��Ƶ��������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_audio_codec(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);
	/**
	 * @brief ��ȡ��Ƶ��������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] codec_type ��Ƶ��������.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_audio_codec(const char *device_id, const int &stream_id, const char *codec_type);
	
	/**
	 * @brief ��ȡ��Ƶ������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_audio_samplingrate(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);

	/**
	 * @brief ������Ƶ������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] samplingrate ��Ƶ������.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_audio_samplingrate(const char *device_id, const int &stream_id, const char *samplingrate);
	
	/**
	 * @brief ��ȡ��Ƶ������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_audio_bitrates(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);
	/**
	 * @brief ������Ƶ������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] bitrates ������.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_audio_bitrates(const char *device_id, const int &stream_id, const char *bitrates);

	/**
	 * @brief ��ȡ��Ƶ��������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [out] out_data ���صķֱ���������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_audio_sampling_accuracy(const char *device_id, const int &stream_id, char *out_data, long *out_data_len);

	/**
	 * @brief ������Ƶ��������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] stream_id �������� @reference MediaStreamType.
	 * @param [in] sampling_accuar ��Ƶ��������.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_audio_sampling_accuracy(const char *device_id, const int &stream_id, const char *bitrates);

	/**
	 * @brief �����豸.
	 * @param [in] device_id �豸ΨһID.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int reboot_device(const char *device_id);

	/**
	 * @brief �豸�ػ�.
	 * @param [in] device_id �豸ΨһID.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int shutdown_device(const char *device_id);

	/**
	 * @brief �豸����.
	 * @param [in] device_id �豸ΨһID.
	 * @return 0:success else return error code.
	 */

	DEVICE_PLUGINS_HELPER_API int startup_device(const char *device_id);
	/**
	 * @brief ��ȡ�豸��Ϣ.
	 * @param [in] device_id �豸ΨһID.
	 * @param [out] out_data ���ص�������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_device_info(const char *device_id, char *out_data, long *out_data_len);
	/**
	 * @brief 
	 * @param [in] device_id �豸ΨһID.
	 * @param [out] out_data ���ص�������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_device_ablitity(const char *device_id, char *out_data, long *out_data_len);

	
	/**
	 * @brief ��ȡͼ������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [out] out_data ���ص�������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_bright(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief ����ͼ������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] bright ����.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_bright(const char *device_id, const char *bright);

	/**
	 * @brief ��ȡͼ��Աȶ�.
	 * @param [in] device_id �豸ΨһID.
	 * @param [out] out_data ���ص�������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_contrast(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief ����ͼ��Աȶ�.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] contrast ͼ��Աȶ�.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_contrast(const char *device_id, const char* contrast);

	/**
	 * @brief ��ȡͼ�񱥺Ͷ�.
	 * @param [in] device_id �豸ΨһID.
	 * @param [out] out_data ���ص�������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_saturation(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief ����ͼ�񱥺Ͷ�.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] saturation ͼ�񱥺Ͷ�.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_saturation(const char *device_id, const char *saturation);

	/**
	 * @brief ��ȡͼ�����.
	 * @param [in] device_id �豸ΨһID.
	 * @param [out] out_data ���ص�������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_hue(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief ����ͼ�����.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] hue ͼ��ɫ��.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_hue(const char *device_id, const char *hue);

	/**
	 * @brief ��ȡͼ�����.
	 * @param [in] device_id �豸ΨһID.
	 * @param [out] out_data ���ص�������Ϣ.
	 * @param [out] out_data_len �������ݵ�ʵ�ʳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int get_image_sharpess(const char *device_id, char *out_data, long *out_data_len);

	/**
	 * @brief ����ͼ������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] sharpness ͼ�����.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int set_image_sharpness(const char *device_id, const char *sharpness);
#pragma endregion SDK

#pragma region ONVIF
	/**
	 * @brief ONVIFЭ����������.
	 * @param [in]
	 */
	DEVICE_PLUGINS_HELPER_API int request_onvif_cmd(const char *device_id, const int cmd_index, const char *uri, char *out_data, long *out_data_len);
	/**
	 * @brief ONVIFЭ����������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] cmd_name ��������.
	 * @param [in] in_params �������.
	 * @param [out] out_data ��������.
	 * @param [out] out_data_len �������ݳ���.
	 * @return 0:success else return error code.
	 */
	DEVICE_PLUGINS_HELPER_API int request_onvif_cmd_ext(const char *device_id, const string cmd_name, const char *in_params, char *out_data, long *out_data_len);

#pragma endregion ONVIF

#pragma region PSIA
	/**
	 * @brief PSIAЭ����������.
	 * @param [in] device_id �豸ΨһID.
	 * @param [in] method [GET(0)|PUT(1)|POST(2)|DEL(3)] @reference psia_method
	 * @param [in] uri PSIAЭ��URI��ַ.
	 * @param [out] out_data ��������.
	 * @param [out] out_data_len �������ݳ���.
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
