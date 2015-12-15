// device_gateway_onvif.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "device_plugins_helper.h"
#include "python_module_loader.h"

#ifdef DEVICE_PLUGINS_ONVIF_EXPORTS
#define DEVICE_PLUGINS_ONVIF_API __declspec(dllexport)
#else
#define DEVICE_PLUGINS_ONVIF_API __declspec(dllimport)
#endif

#pragma region PythonWrap
//#include <boost/python.hpp>
//using namespace boost::python;
//BOOST_PYTHON_MODULE(device_plugins_onvif)
//{
//	def("register_device", register_device, args("device_id", "device_type", "device_name", "device_manufacture", "device_ip", "device_port", "user_name", "user_pwd"), "register a gateway device.");
//	def("unregister_device", unregister_device, args("device_id"), "unregister a gateway device.");
//	def("get_stream_url", request_onvif_cmd, args("device_id", "cmd_name", "in_params", "out_data", "out_data_len"), "request onvif cmd");
//}
#pragma endregion PythonWrap

inline int convert_c_args_to_python_args(const string &uri)
{
	
	return v_OK;
}
std::once_flag init_python;
std::once_flag uninit_python;
data_cache_ex<string, PyObject*> s_module_instance_lists;
DEVICE_PLUGINS_ONVIF_API int register_device(const char *device_id, const int &device_type, const char *device_name, const int &device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd)
{
	if (!device_id || !device_ip || !user_name || !user_pwd)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	int ec = 0;
	std::call_once(init_python, [&ec](){
		ec = python_module_loader::init();
	});
	python_module_loader module_loader;
	PyObject *mod = module_loader.load_module("onvif_host");
	PyObject *args = PyTuple_New(5);
	PyTuple_SetItem(args, 0, Py_BuildValue("s", device_id));
	PyTuple_SetItem(args, 1, Py_BuildValue("s", device_ip));
	PyTuple_SetItem(args, 2, Py_BuildValue("i", device_port));
	PyTuple_SetItem(args, 3, Py_BuildValue("s", user_name));
	PyTuple_SetItem(args, 4, Py_BuildValue("s", user_pwd));
	if (mod)
	{
		s_module_instance_lists.insert(device_id, mod);
		PyObject *mod_func = module_loader.get_func(mod, "register_device");
		if (mod_func)
		{
			module_loader.call_func(mod_func, args);
		}
	}
	return v_OK;
}

DEVICE_PLUGINS_ONVIF_API int unregister_device(const char *device_id)
{
	int ec = 0;
	if (device_id)
	{
		std::call_once(uninit_python, [&ec](){
			ec = python_module_loader::uninit();
		});
		
	}
	python_module_loader module_loader;
	PyObject *mod = nullptr;
	if (s_module_instance_lists.find(device_id, mod))
	{
		PyObject *mod_func = module_loader.get_func(mod, "unregister_device");
		PyObject *args = PyTuple_New(1);
		PyTuple_SetItem(args, 0, Py_BuildValue("s", device_id));
		if (!mod_func)
		{
			module_loader.call_func(mod_func, args);
		}
	}
	return v_OK;
}

DEVICE_PLUGINS_ONVIF_API int request_onvif_cmd(const char *device_id, const int cmd_index, const char *uri, char *out_data, long *out_data_len)
{
	PyObject *mod = nullptr;
	python_module_loader module_loader;
	if (s_module_instance_lists.find(device_id, mod))
	{
		PyObject *mod_func = module_loader.get_func(mod, "request_onvif_cmd");
		PyObject *args = PyDict_New();
		PyDict_SetItemString(args, "device_id", Py_BuildValue("s", device_id));
		PyDict_SetItemString(args, "cmd", Py_BuildValue("i", cmd_index));
	}
	return v_OK;
}

DEVICE_PLUGINS_ONVIF_API int request_onvif_cmd_ext(const char *device_id, const string cmd_name, const char *in_params, char *out_data, long *out_data_len)
{
	PyObject *mod = nullptr;
	python_module_loader module_loader;
	if (s_module_instance_lists.find(device_id, mod))
	{
		PyObject *mod_func = module_loader.get_func(mod, "request_onvif_cmd");
		/*
		PyObject *args = PyDict_New();
		PyDict_SetItemString(args, "device_id", Py_BuildValue("s", device_id));
		PyDict_SetItemString(args, "cmd", Py_BuildValue("s", cmd_name));
		PyDict_SetItemString(args, "out_params", Py_BuildValue("s", out_data));
		*/
		regex_text_finder param_finder;
		const string params_format = "([^&]+)";
		vector<string> params;
		if (0 < strlen(in_params))
		{
			params = v_str_split_ex(in_params, "&");
		}
		string param_name;
		string param_value;
		PyObject *args = nullptr;
		PyObject *args_two = PyDict_New();
		if (params.empty())
		{
			args = PyTuple_New(3);
		}
		else
		{
			args = PyTuple_New(4);
		}
		PyTuple_SetItem(args, 0, Py_BuildValue("s", device_id));
		PyTuple_SetItem(args, 1, Py_BuildValue("s", cmd_name.c_str()));
		PyObject* out_data_list = nullptr;
		if (out_data_len && 0 < *out_data_len)
		{
			out_data_list = PyList_New(0);
			PyTuple_SetItem(args, 2, out_data_list);
		}
		for (auto param : params)
		{
			regex_text_finder rtf;
			if (rtf.find(param, "(.+?):(.+)"))
			{
				param_name = rtf[1];
				param_value = rtf[2];
			}
			if (!param_name.empty() && !param_value.empty())
			{
				PyDict_SetItemString(args_two, param_name.c_str(), Py_BuildValue("{s}", param_value));
			}
		}
		if (!params.empty())
		{
			PyTuple_SetItem(args, 4, args_two);
		}
		if (mod_func)
		{
			module_loader.call_func(mod_func, args);
		}
		int size = (int)PyList_Size(out_data_list);
		int item_content_len = 0;
		for (int item_index = 0; item_index < size; ++item_index)
		{
			PyObject *out_item = PyList_GetItem(out_data_list, item_index);
			int out_item_size = (int)PyString_Size(out_item);
			char *tmp = PyString_AsString(out_item);
			if (out_item_size < (*out_data_len - item_content_len))
			{
				memcpy(out_data + item_content_len, tmp, out_item_size);
				item_content_len += out_item_size;
			}
			else
			{
				memcpy(out_data, tmp, (*out_data_len - item_content_len));
				break;
			}
		}
		*out_data_len = item_content_len;
	}
	return v_OK;
}