// device_gateway_hikvisionV1.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "device_plugins_helper.h"
#include "hikvision_common_device.h"
#include "hikvision_device_ipc.h"
#include "data_cache_ex.h"
#include <boost/python.hpp>

#ifdef DEVICE_PLUGINS_HIKVISION_EXPORTS
#define DEVICE_PLUGINS_HIKVISION_API __declspec(dllexport)
#else
#define DEVICE_PLUGINS_HIKVISION_API __declspec(dllimport)
#endif

data_cache_ex<string, hikvision_common_device*> s_device_lists;
data_cache_ex<string, int> s_device_type_lists;
std::mutex s_device_lists_mtx;
#pragma region PythonModule
using namespace boost::python;
BOOST_PYTHON_MODULE(device_plugins_hikvision)
{
	def("register_device", register_device, args("device_id", "device_type", "device_name", "device_manufacture", "device_ip", "device_port", "user_name", "user_pwd"), "register a gateway device.");
	def("unregister_device", unregister_device, args("device_id"), "unregister a gateway device.");
	def("get_stream_url", get_stream_url, args("out_data", "out_len", "device_id", "stream_type", "channel"));
}
#pragma endregion PythonModule

hikvision_common_device* find_device(char *device_id);

DEVICE_PLUGINS_HIKVISION_API int register_device(const char *device_id, const int &device_type, const char *device_name, const int &device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd)
{
	if (!device_id || !device_ip || !user_name || !user_pwd)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	hikvision_common_device *device_ptr = nullptr;
	//hikvision_common_device::init_sdk();
	ALL_DEVICE_NAME::insert_all();
	string device_manufacture_str;
	ALL_DEVICE_NAME::all_device_names()->find((v_device_manufacturer_type)device_manufacture, device_manufacture_str);
	v_lock(lk, s_device_lists_mtx);
	if (s_device_lists.find(device_id, device_ptr))
	{
		return v_OK;
	}
	if (V_DEVICE_DVR == device_type)
	{
	} else if (V_DEVICE_IPC == device_type)
	{
		hikvision_device_ipc *device_ipc = new hikvision_device_ipc(device_id, device_type, device_name, device_manufacture_str, device_ip, device_port, user_name, user_pwd);
		if (device_ipc)
		{
			if (v_OK != device_ipc->login(user_name, user_pwd))
			{
				return v_ERR_BAD_CONNECTION;
			}
			device_ptr = device_ipc;
		}
	} else if (V_DEVICE_NVR == device_type)
	{
	} else if (V_DEVICE_ENCODER == device_type)
	{
	} else if (V_DEVICE_DECODER == device_type)
	{
	} else if (V_DEVICE_BAYONET == device_type)
	{
	} else
	{
		return v_ERR_BAD_DEVICE;
	}
	assert(nullptr != device_ptr);
	s_device_lists.insert(device_id, device_ptr);
	s_device_type_lists.insert(device_id, device_type);
	return v_OK;
}

DEVICE_PLUGINS_HIKVISION_API int unregister_device(const char *device_id)
{
	assert(nullptr != device_id);
	v_lock(lk, s_device_lists_mtx);
	s_device_lists.remove(device_id);
	s_device_type_lists.remove(device_id);
	return v_OK;
}

DEVICE_PLUGINS_HIKVISION_API int get_stream_url(char *out_data, long *out_len, const char *device_id, const int &stream_type, const int &channel/*=0*/)
{
	if (!out_data || !out_len || !device_id)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	hikvision_common_device *device_ptr = nullptr;
	if (s_device_lists.find(device_id, device_ptr))
	{
		v_device_type device_type = V_DEVICE_NONE;
		int tmp_device = -1;
		s_device_type_lists.find(device_id, tmp_device);
		device_type = (v_device_type)tmp_device;
		int ec = 0;
		switch (device_type)
		{
		case V_DEVICE_NONE:
			break;
		case V_DEVICE_IPC:
		{
			auto ptr = dynamic_cast<hikvision_device_ipc*>(device_ptr);
			if (ptr->create_media_service())
			{
				ec = ptr->get_media_service()->get_stream_urls(stream_type, out_data, out_len);
			}
		}
			break;
		case V_DEVICE_DVR:
			break;
		case V_DEVICE_NVR:
			break;
		case V_DEVICE_ENCODER:
			break;
		case V_DEVICE_DECODER:
			break;
		case V_DEVICE_BAYONET:
			break;
		default:
			return v_ERR_Bad_XML;
			break;
		}
		return ec;
	}
	else
	{
		return v_ERR_BAD_DEVICE;
	}
}

DEVICE_PLUGINS_HIKVISION_API int get_device_status(char *out_data, long *out_len, const char *device_id, const int &channel /* = 0 */)
{
	if (!out_data || !out_len || !device_id)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	hikvision_common_device *device_ptr = nullptr;
	if (s_device_lists.find(device_id, device_ptr))
	{
		v_device_type device_type = V_DEVICE_NONE;
		int tmp_device = -1;
		s_device_type_lists.find(device_id, tmp_device);
		device_type = (v_device_type)tmp_device;
		int ec = 0;
		switch (device_type)
		{
		case V_DEVICE_NONE:
			break;
		case V_DEVICE_IPC:
		{
			auto ptr = dynamic_cast<hikvision_device_ipc*>(device_ptr);
			ec = ptr->get_device_status(out_data, out_len);
		}
			break;
		case V_DEVICE_DVR:
			break;
		case V_DEVICE_NVR:
			break;
		case V_DEVICE_ENCODER:
			break;
		case V_DEVICE_DECODER:
			break;
		case V_DEVICE_BAYONET:
			break;
		default:
			return v_ERR_BAD_DEVICE;
			break;
		}
		return ec;
	}
	else
	{
		return v_ERR_BAD_DEVICE;
	}
}

DEVICE_PLUGINS_HIKVISION_API int add_user(const char *device_id, const char *user_name, const char *password, const int &level /* = 0 */)
{
	if (!device_id || !user_name || !password)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	hikvision_common_device *device_ptr = nullptr;
	if (s_device_lists.find(device_id, device_ptr))
	{
		v_device_type device_type = V_DEVICE_NONE;
		int tmp_device = -1;
		s_device_type_lists.find(device_id, tmp_device);
		device_type = (v_device_type)tmp_device;
		int ec = 0;
		switch (device_type)
		{
		case V_DEVICE_NONE:
			break;
		case V_DEVICE_IPC:
		{
			auto ptr = dynamic_cast<hikvision_device_ipc*>(device_ptr);
			ptr->add_user(user_name, password, level);
		}
			break;
		case V_DEVICE_DVR:
			break;
		case V_DEVICE_NVR:
			break;
		case V_DEVICE_ENCODER:
			break;
		case V_DEVICE_DECODER:
			break;
		case V_DEVICE_BAYONET:
			break;
		default:
			return v_ERR_BAD_DEVICE;
			break;
		}
		return ec;
	}
	return v_OK;
}

DEVICE_PLUGINS_HIKVISION_API int delete_user(const char *device_id, const char *user_name)
{
	if (!device_id || !user_name)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	int ec = 0;
	hikvision_common_device *device_ptr = nullptr;
	if (s_device_lists.find(device_id, device_ptr))
	{
		v_device_type device_type = V_DEVICE_NONE;
		int tmp_device = -1;
		s_device_type_lists.find(device_id, tmp_device);
		device_type = (v_device_type)tmp_device;
		int ec = 0;
		switch (device_type)
		{
		case V_DEVICE_NONE:
			break;
		case V_DEVICE_IPC:
		{
			auto ptr = dynamic_cast<hikvision_device_ipc*>(device_ptr);
			ec = ptr->delete_user(user_name);
		}
			break;
		case V_DEVICE_DVR:
			break;
		case V_DEVICE_NVR:
			break;
		case V_DEVICE_ENCODER:
			break;
		case V_DEVICE_DECODER:
			break;
		case V_DEVICE_BAYONET:
			break;
		default:
			return v_ERR_BAD_DEVICE;
			break;
		}
	}
	return ec;
}

DEVICE_PLUGINS_HIKVISION_API int alter_user(const char *devcie_id, const char *old_name, const char *new_name, const char *password, const int &level/*0*/)
{
	return v_OK;
}

DEVICE_PLUGINS_HIKVISION_API int wrap_status_func(const char *device_id, function<int(char *out_data, long *out_len, void *obj)>)
{
	return v_OK;
}
