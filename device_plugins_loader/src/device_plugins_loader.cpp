// device_gateway_loaderV1.cpp : Defines the exported functions for the DLL application.

#include "stdafx.h"
#include "device_plugins_loader.h"
DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL register_gateway_device(v_device_protocol_type protocol_type, const char *device_id, v_device_type device_type, const char *device_name, v_device_manufacturer_type device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd)
{
	if (!device_id || !device_ip || !user_name || !user_pwd)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	proc_util_func_wrap_t tmp_fun_warp;
	switch (protocol_type)
	{
	case V_PORTOCOL_SDK:
	{
		module_helper *tmp = module_helper::get_instance();
#ifdef _DEBUG
		vvlog_i("1");
#endif
		HMODULE module = nullptr;
		string  dll_name;
		ALL_DEVICE_NAME::insert_all();
		ALL_DEVICE_NAME::all_device_names()->find((v_device_manufacturer_type)device_manufacture, dll_name);
		tmp->get_dll_handle(dll_name, &module);
		tmp_fun_warp.wrap(module);
#ifdef _DEBUG
		vvlog_i("2");
#endif
		if (tmp_fun_warp.is_good())
		{
			tmp_fun_warp.register_device(device_id, device_type, device_name, device_manufacture, device_ip, device_port, user_name, user_pwd);
		}
		switch (device_manufacture)
		{
		case V_MANUFACTURER_HIKVISION:
		{
			s_device_lists.insert(device_id, DLL_INDEX_HIKVISION);
			s_fun_wrap.insert(DLL_INDEX_HIKVISION, tmp_fun_warp);
		}
		break;
		case V_MANUFACTURER_DAHUA:
		{
			s_device_lists.insert(device_id, DLL_INDEX_DAHUA);
			s_fun_wrap.insert(DLL_INDEX_DAHUA, tmp_fun_warp);
		}
			break;
		default:
			break;
		}
	}
	break;
	case V_PROTOCOL_ONVIF:
	{
		module_helper *tmp = module_helper::get_instance();
		HMODULE module = nullptr;
		string  dll_name;
		ALL_DEVICE_NAME::insert_all();
		ALL_DEVICE_NAME::all_device_names()->find((v_device_manufacturer_type)device_manufacture, dll_name);
		tmp->get_dll_handle(dll_name, &module);
		tmp_fun_warp.wrap(module);
		if (tmp_fun_warp.is_onvif_good())
		{
			tmp_fun_warp.register_device(device_id, device_type, device_name, device_manufacture, device_ip, device_port, user_name, user_pwd);
		}
		s_onvif_device_lists.insert(device_id, DLL_INDEX_ONVIF);
		s_fun_wrap.insert(DLL_INDEX_ONVIF, tmp_fun_warp);
	}
	break;
	case V_PROTOCOL_PSIA:
	{
		module_helper *tmp = module_helper::get_instance();
		HMODULE module = nullptr;
		string  dll_name;
		ALL_DEVICE_NAME::insert_all();
		ALL_DEVICE_NAME::all_device_names()->find((v_device_manufacturer_type)device_manufacture, dll_name);
		tmp->get_dll_handle(dll_name, &module);
		tmp_fun_warp.wrap(module);
		if (tmp_fun_warp.is_psia_good())
		{
			tmp_fun_warp.register_device(device_id, device_type, device_name, device_manufacture, device_ip, device_port, user_name, user_pwd);
		}
		s_psia_device_lists.insert(device_id, DLL_INDEX_PSIA);
		s_fun_wrap.insert(DLL_INDEX_PSIA, tmp_fun_warp);
	}
	break;
	case V_PROTOCOL_28181:
	{

	}
	break;
	case V_PROTOCOL_HDCCTV:
	{
	}
	break;
	default:
	{

	}
	break;
	}
	return v_OK;
}

DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL unregister_gateway_device(const char *device_id)
{
	if (!device_id)
	{
		int dll_index = 0;
		if (s_device_lists.find(device_id, dll_index))
		{
			s_fun_wrap.remove(dll_index);
		}
		s_device_lists.remove(device_id);
		return v_OK;
	}
	return v_ERR_BAD_ARGUMENT;
}

DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL device_exec_cmd(const char* device_id, const char *cmd_uri, void *cmd_extcontent, char *out_data, long *out_size)
{
#ifdef _DEBUG
	vvlog_i("begin device_id:" << device_id << "uri" << cmd_uri);
#endif
	device_uri_parser cmd_parser(cmd_uri);
	if (device_uri_parser::func_register_device_id == cmd_parser.func_name_id())
	{
		v_device_protocol_type proto_type = cmd_parser.protocol_type();
		//string device_id = cmd_parser.device_id();
		v_device_type device_type = cmd_parser.device_type();
		string device_name = "";
		v_device_manufacturer_type device_manuc = cmd_parser.device_manufacture_type();
		string device_ip = cmd_parser.ip();
		int	   device_port = cmd_parser.port();
		string user_name = cmd_parser.user_name();
		string password = cmd_parser.password();
		proc_util_func_wrap_t func_tmp;
		switch (device_manuc)
		{
		case V_MANUFACTURER_HIKVISION:
			s_device_lists.insert(device_id, DLL_INDEX_HIKVISION);
			s_fun_wrap.find(DLL_INDEX_HIKVISION, func_tmp);
			break;
		case V_MANUFACTURER_ONVIF:
			s_onvif_device_lists.insert(device_id, DLL_INDEX_ONVIF);
			s_fun_wrap.find(DLL_INDEX_ONVIF, func_tmp);
			break;
		case V_MANUFACTURER_PSIA:
			s_psia_device_lists.insert(device_id, DLL_INDEX_PSIA);
			s_fun_wrap.find(DLL_INDEX_PSIA, func_tmp);
			break;
		case V_MANUFACTURER_DAHUA:
			s_device_lists.insert(device_id, DLL_INDEX_DAHUA);
			s_fun_wrap.find(DLL_INDEX_DAHUA, func_tmp);
			break;
		default:
			break;
		}
		
#ifdef _DEBUG
		pugi::xml_document doc;
		pugi::xml_node res = doc.append_child("response");
		res.append_attribute("error_code").set_value("true");
		stringstream ss_out;
		doc.save(ss_out);
		string out = ss_out.str();
		out.append("<!--msg_end-->");
#endif
		int ec =func_tmp.register_device(device_id, device_type, device_name.c_str(), device_manuc, device_ip.c_str(), device_port, user_name.c_str(), password.c_str());
		memcpy(out_data, out.c_str(), out.length());
		*out_size = out.length();
		//int ec = register_gateway_device(proto_type,device_id, device_type, device_name.c_str(), device_manuc, device_ip.c_str(), device_port, user_name.c_str(), password.c_str());
		vvlog_i("end device_id:" << device_id << "uri" << cmd_uri);
		return ec;
	}
	int ec = 0;
	if (device_id && cmd_uri && out_data && out_size)
	{
		int dll_index = 0;
		if (s_device_lists.find(device_id, dll_index))
		{
			proc_util_func_wrap_t tmp_func;
			if (s_fun_wrap.find(dll_index, tmp_func) && tmp_func.is_good())
			{
				if (!cmd_parser.func_name().empty())
				{
					switch (cmd_parser.func_name_id())
					{
					case device_uri_parser::func_get_stream_url_id:
					{
						int stream_type = atoi(cmd_parser.stream_id().c_str());
						int channel = 0;
						if (!cmd_parser.channel().empty())
						{
							channel = atoi(cmd_parser.channel().c_str());
						}
						ec = tmp_func.get_stream_url(out_data, out_size, device_id, stream_type, channel);
					}
					break;
					case device_uri_parser::func_add_user_id:
					{
						string user_name = cmd_parser.user_name();
						string password  = cmd_parser.password();
						int level = 0;
						ec = tmp_func.add_user(device_id, user_name.c_str(), password.c_str(), 0);
					}
					break;
					case device_uri_parser::func_delete_user_id:
					{
						ec = tmp_func.delete_user(device_id, cmd_parser.user_name().c_str());
					}
					break;
					case device_uri_parser::func_alter_user_id:
					{
						ec = tmp_func.alter_user(device_id, cmd_parser.old_name().c_str(), cmd_parser.user_name().c_str(), cmd_parser.password().c_str(), 0);
					}
					default:
						break;
					}
				}
			}
		}
		if (s_psia_device_lists.find(device_id, dll_index))
		{
			proc_util_func_wrap_t tmp_func;
			if (s_fun_wrap.find(dll_index, tmp_func) && tmp_func.is_psia_good())
			{
				if (!cmd_parser.func_name().empty())
				{
					switch (cmd_parser.func_name_id())
					{
					case device_uri_parser::func_get_stream_url_id:
					{
						if (tmp_func.is_psia_good())
						{
							tmp_func.request_pisa_cmd(device_id, PSIA_M_GET, cmd_parser.get_psia_uri().c_str(), out_data, out_size);
						}
					}
					break;
					default:
						break;
					}
				}
			}
		}

		if (s_onvif_device_lists.find(device_id, dll_index))
		{
			proc_util_func_wrap_t tmp_func;
			if (s_fun_wrap.find(dll_index, tmp_func) && tmp_func.is_onvif_good())
			{
				if (tmp_func.is_onvif_good())
				{
					tmp_func.request_onvif_cmd_ext(device_id, cmd_parser.func_name(), cmd_parser.get_onvif_uri().c_str(), out_data, out_size);
				}
			}
		}
	}
	return ec;
}

DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL device_wrap(const char *device_id, status_change_call_back fp_call_back)
{
	int ec = 0;
	if (device_id)
	{
		int dll_index = 0;
		if (s_device_lists.find(device_id, dll_index))
		{
			proc_util_func_wrap_t tmp_func;
			if (s_fun_wrap.find(dll_index, tmp_func) && tmp_func.is_good())
			{
				tmp_func.wrap_status_func(device_id, fp_call_back);
			}
		}
	}
	return ec;
}

DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL load_device_plugin(v_device_manufacturer_type manu)
{
	proc_util_func_wrap_t tmp_fun_warp;
	module_helper *tmp = module_helper::get_instance();

	HMODULE module = nullptr;
	string  dll_name;
	ALL_DEVICE_NAME::insert_all();
	ALL_DEVICE_NAME::all_device_names()->find((v_device_manufacturer_type)manu, dll_name);
	tmp->get_dll_handle(dll_name, &module);
	tmp_fun_warp.wrap(module);
	switch (manu)
	{
	case V_MANUFACTURER_HIKVISION:
	{
		s_fun_wrap.insert(DLL_INDEX_HIKVISION, tmp_fun_warp);
	}
	break;
	case V_MANUFACTURER_DAHUA:
	{
		s_fun_wrap.insert(DLL_INDEX_DAHUA, tmp_fun_warp);
	}
	break;
	case V_MANUFACTURER_ONVIF:
	{
		s_fun_wrap.insert(DLL_INDEX_ONVIF, tmp_fun_warp);
	}
	break;
	case V_MANUFACTURER_PSIA:
	{
		s_fun_wrap.insert(DLL_INDEX_PSIA, tmp_fun_warp);
	}
	break;
	default:
		break;
	}
	return v_OK;
}

DEVICE_PLUGINS_LOADER_API int VISTEK_STDCALL unload_device_plugin(v_device_manufacturer_type manu)
{
	return v_OK;
}