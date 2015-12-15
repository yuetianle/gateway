#ifndef _BASE_DEVICE_DATA_TYPES_DEFS_H_
#define _BASE_DEVICE_DATA_TYPES_DEFS_H_

#pragma once
#include <string>
#include "data_cache_ex.h"
using std::string;
enum v_device_protocol_type
{
	V_PROTOCOL_NONE =-1,
	V_PORTOCOL_SDK = 0,
	V_PROTOCOL_ONVIF = 1,
	V_PROTOCOL_PSIA = 2,
	V_PROTOCOL_28181 = 3,
	V_PROTOCOL_HDCCTV = 4
};

enum v_device_type
{
	V_DEVICE_NONE = -1,
	V_DEVICE_IPC = 0,
	V_DEVICE_DVR = 1,
	V_DEVICE_NVR = 2,
	V_DEVICE_ENCODER = 3,
	V_DEVICE_DECODER = 4,
	V_DEVICE_BAYONET = 5 //交通卡口
};

enum v_device_manufacturer_type
{
	V_MANUFACTURER_NONE			= 0,
	V_MANUFACTURER_ONVIF		= 1,
	V_MANUFACTURER_PSIA			= 2,
	V_MANUFACTURER_GB28181		= 3,
	V_MANUFACTURER_HDCCTV		= 4,
	V_MANUFACTURER_HIKVISION	= 5,
	V_MANUFACTURER_DAHUA		= 6,
	V_MANUFACTURER_TDWY			= 8,//天地伟业
	V_MANUFACTURER_AXIS			= 16,//安讯士
	V_MANUFACTURER_BLUESTAR		= 32,//蓝色星际
	V_MANUFACTURER_HBGK			= V_MANUFACTURER_BLUESTAR + 16,//汉邦高科
	V_MANUFACTURER_ACTI			= V_MANUFACTURER_BLUESTAR + 32,
	V_MANUFACTURER_AEBELL		= V_MANUFACTURER_BLUESTAR + 48,
	V_MANUFACTURER_CANON		= V_MANUFACTURER_BLUESTAR + 64,
	V_MANUFACTURER_HUANGHE		= V_MANUFACTURER_BLUESTAR + 80,
	V_MANUFACTURER_INFINOVA		= V_MANUFACTURER_BLUESTAR + 96,//英飞拓
	V_MANUFACTURER_PELCO		= V_MANUFACTURER_BLUESTAR + 112,//派尔高
	V_MANUFACTURER_LANGCHI		= V_MANUFACTURER_BLUESTAR + 128,//朗驰
	V_MANUFACTURER_SAMSUNG		= V_MANUFACTURER_BLUESTAR 
};

struct v_manufacturer_name
{
	static const string v_manufacturer_onvif_name;
	static const string v_manufacturer_psia_name;
	static const string v_manufacturer_gb28181_name;
	static const string v_manufacturer_hdcctv_name;
	static const string v_manufacturer_hikvison_name;
	static const string v_manufacturer_dahua_name;
	static const string v_manufacturer_tdwy_name;
	static const string v_manufacturer_axis_name;
	static const string v_manufacturer_bluestar_name;
	static const string v_manufacturer_hbgk_name;
	static const string v_manufacturer_acti_name;
	static const string v_manufacturer_aebell_name;
	static const string v_manufacturer_canon_name;
	static const string v_manufacturer_huanghe;
	static const string v_manufacturer_infinova;
	static const string v_manufacturer_pelco;
	static const string v_manufacturer_langchi;
	static const string v_manufacturer_samsung;
};

typedef struct v_manufacturer_info
{
public:
	v_manufacturer_info();
	static data_cache_ex<v_device_manufacturer_type, string>* all_device_names();
	static int add_device_name(v_device_manufacturer_type type_id, string name);
	static int insert_all();
private:
	static data_cache_ex<v_device_manufacturer_type, string> all_name;
}ALL_DEVICE_NAME;


typedef struct device_info
{
public:
	device_info& operator=(const device_info &info);
	string device_id() const { return m_device_id; }
	int	   device_type() const { return m_device_type; }
	string device_name() const { return m_device_name; }
	string device_manufacture() const { return m_device_manufacture; }
	string device_ip() const { return m_device_ip; }
	long device_port() const { return m_device_port; }
private:
	string m_device_id;
	int    m_device_type;
	string m_device_name;
	string m_device_manufacture;
	string m_device_ip;
	long   m_device_port;
}DeviceInfo;

typedef struct user_info
{
public:
	string user_name() const { return m_user_name; }
	string password() const { return m_user_password; }
	user_info(string name, string pwd)
		:m_user_name(name), m_user_password(pwd)
	{ }
private:
	string m_user_name;
	string m_user_password;
}DeviceUserInfo;

enum HandleType
{
	HANDLE_UNKOWN = -1,
	HANDLE_I = 0,
	HANDLE_L = 1,
	HANDLE_VOID =2,
};

typedef struct HandleInfo
{
	HandleType handle_type;
	union handle_info
	{
		int  ihandle;
		long lhandle;
		void *pHandle;
	};
	handle_info handle;
	HandleInfo()
	{
		handle_type = HANDLE_UNKOWN;
	}
	HandleInfo& operator = (const HandleInfo &info)
	{
		handle_type = info.handle_type;
		handle.ihandle = info.handle.ihandle;
		handle.lhandle = info.handle.lhandle;
		handle.pHandle = info.handle.pHandle;
		return *this;
	}
}Handle_Info;

typedef boost::signals2::signal <int(char*out_data, long*out_len, void*sender_device)> DeviceStatusSignal;

enum MediaStreamType
{
	StreamMain	= 1,
	StreamSub	= 2,
	StreamThird = 4,
	StreamALL	= 7,
};

enum psia_method
{
	PISA_M_NONE = -1,
	PSIA_M_GET	= 0,
	PSIA_M_POST = 1,
	PSIA_M_PUT	= 2,
	PSIA_M_DEL	= 3
};

enum MediaStreamBitRateType
{
	STREAM_CBR,
	STREAM_VBR,
};

#endif