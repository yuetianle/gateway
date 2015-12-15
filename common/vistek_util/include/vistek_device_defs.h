#ifndef vistek_device_defs_h__
#define vistek_device_defs_h__

#pragma once
#include "boost/signals2.hpp"

typedef enum vistek_device_category_flag
{
	v_DEVICE_CATEGORY_NONE = 0,
	v_DEVICE_CATEGORY_SYS_DEVICE = 1,
	v_DEVICE_CATEGORY_REMOTE_CONFIG = 2,
	v_DEVICE_CATEGORY_STREAM_RT = 4,
	v_DEVICE_CATEGORY_STREAM_PB = 8,
	v_DEVICE_CATEGORY_BAYONET = 16,
	v_DEVICE_CATEGORY_ALARM_IN = 32,
	v_DEVICE_CATEGORY_ALARM_OUT = 64,
	v_DEVICE_CATEGORY_STORAGE= 128
}vistek_device_category_flag_t;
typedef enum vistek_device_protocol_flag
{
	v_Support_None = 0,
	v_Support_SDK = 1,
	v_Support_Onvif = 2,
	v_Support_PSIA = 4,
	v_Support_28181 = 8,
	v_Support_HDcctv = 16
}vistek_device_protocol_flag_t;
struct vistek_device_info
{
	std::string device_id;
	std::string device_type_code;
	std::string device_code;
	std::string device_manufacture;
	std::string device_ip;
	int device_port;
	std::string username;
	std::string password;
	vistek_device_protocol_flag device_protocol;
	std::string extension;
	int error_count;
	std::string sub_mask;
	vistek_device_info(const std::string& did, const std::string& dev_typecode, const std::string& code, const std::string& manufacture, const std::string& ip, int port, const std::string& un, const std::string& pw, int protocol, const std::string& ext, const std::string& submask = "")
		: device_id(did), device_type_code(dev_typecode), device_code(code), device_manufacture(manufacture), device_ip(ip), device_port(port), username(un), password(pw), device_protocol((vistek_device_protocol_flag)protocol), extension(ext), error_count(0), sub_mask(submask)
	{

	}
};
typedef std::shared_ptr<vistek_device_info> vistek_device_info_ptr;

enum vistek_device_status
{
	vds_OK = 0,
	vds_OFFLINE = 1,
	vds_ERROR = 2
};

typedef boost::signals2::signal<void(const std::string& did, vistek_device_status sts, int ec)> device_status_notifier;
struct vistek_stream_info
{
	vistek_device_info_ptr device_info;
	int channel_index;
	int stream_index;
	vistek_stream_info()
		: device_info(0), channel_index(0), stream_index(0)
	{

	}
	vistek_stream_info(const vistek_device_info_ptr& deviceInfo, int ch, int si)
		: device_info(deviceInfo), channel_index(ch), stream_index(si)
	{

	}
};
typedef std::shared_ptr<vistek_stream_info> vistek_stream_info_ptr;
enum ptz_switch_command
{
	LIGHT,
	SWIPER,
	HEATER
};

enum ptz_tuning_command
{
	IRIS,
	FOCUS,
	ZOOM
}; 
#endif // vistek_device_defs_h__