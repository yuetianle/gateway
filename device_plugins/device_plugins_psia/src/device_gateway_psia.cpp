// device_gateway_psia.cpp : Defines the exported functions for the DLL application.
//

#ifdef DEVICE_PLUGINS_PSIA_EXPORTS
#define DEVICE_PLUGINS_PSIA_API __declspec(dllexport)
#else
#define DEVICE_PLUGINS_PSIA_API __declspec(dllimport)
#endif

#include "stdafx.h"
#include "device_plugins_helper.h"
#include "data_cache_ex.h"
#include "device_host.h"
data_cache < string, device_host*> s_psia_device_lists;
__declspec(dllexport) int register_device(const char *device_id, const int &device_type, const char *device_name, const int &device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd)
{
	if (!device_id || !device_ip || !user_name || !user_pwd)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	string manufacture_name;
	ALL_DEVICE_NAME::insert_all();
	ALL_DEVICE_NAME::all_device_names()->find((v_device_manufacturer_type)device_manufacture, manufacture_name);
	device_host * host = new device_host(device_id, device_ip, device_port, device_type, device_name, manufacture_name.c_str(), user_name, user_pwd);
	if (host)
	{
		s_psia_device_lists.insert(device_id, host);
		return v_OK;
	}
	else
	{
		return v_ERR_BAD_DEVICE;
	}
}

/*DEVICE_PLUGINS_PSIA_API*/ __declspec(dllexport) int unregister_device(const char *device_id)
{
	if (!device_id)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	s_psia_device_lists.remove(device_id);
	return v_OK;
}
__declspec(dllexport) int request_pisa_cmd(const char *device_id, const int &method, const char *uri, char *out_data, long *out_data_len)
{
	device_host *host = nullptr;
	int ec = v_OK;
	if (s_psia_device_lists.find(device_id, host))
	{
		ec = host->request(uri, method, out_data, out_data_len);
	}
	return ec;
}

__declspec(dllexport) int add_user(const char *device_id, const char *user_name, const char *password, const int &level /* = 0 */)
{
	return v_OK;
}

__declspec(dllexport) int delete_user(const char *device_id, const char *user_name)
{
	return v_OK;
}

__declspec(dllexport) int alter_user(const char *devcie_id, const char *old_name, const char *new_name, const char *password, const int &level /* = 0 */)
{
	return v_OK;
}