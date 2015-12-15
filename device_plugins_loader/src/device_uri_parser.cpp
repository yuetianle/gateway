#include "stdafx.h"
#include "device_uri_parser.h"
device_uri_parser::device_uri_parser(const string &cmd_uri) : m_cmd_uri(cmd_uri), m_device_type(V_DEVICE_NONE), m_device_protocol_type(V_PROTOCOL_NONE), m_manufacturer_type(V_MANUFACTURER_NONE), m_func_name_id(func_none_id)
{
	_parse_uri_full(m_cmd_uri.c_str());
}


device_uri_parser::~device_uri_parser()
{
}

int device_uri_parser::_parse_uri_full(const char *cmd)
{
	regex_text_finder finder;
	
	const string cmd_format = "(.+?)://(\\d{1,3}(\.\\d{1,3}){3}):?(\\d{0,6})/(.+?)\\?(.+$)";
	if (finder.find(cmd, cmd_format))
	{
		m_protocol_name = finder[1];
		m_ip			= finder[2];
		m_port			= atoi(finder[4].c_str());
		//m_cmd			= finder[5];
		m_cmd_resource	= finder[5];
		if (string::npos != m_cmd_resource.find(".py"))
		{
			m_cmd_type = CMD_PYTHON;
		}
		else
		{
			m_cmd_type = CMD_C;
		}
		string tmp_content = finder[6];
		m_params = tmp_content;
		char first_flag = m_params.at(0);
		if ('&' != first_flag)
		{
			m_params = "&" + m_params;
		}
		_parse_query_string(m_params.c_str());
		_parse_fun_name_id(m_func_name.c_str());
		return v_OK;
	}
	return v_ERR_BAD_ARGUMENT;
}

int device_uri_parser::_parse_query_string(const char *cmd_params)
{
	regex_text_finder param_finder;
	const string params_format = "([^&]+)";
	string param = cmd_params;
	if (param_finder.find(param, PARSER_CONTENT_ID))
	{
		m_content = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_FUNC_NAME_ID))
	{
		m_func_name = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_STREAM_ID))
	{
		m_stream_id = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_USERNAME_ID))
	{
		m_user_name = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_PASSWORD_ID))
	{
		m_password = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_CODEC_ID))
	{
		m_codec = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_FPS_ID))
	{
		m_fps = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_TYPE_ID))
	{
		m_type = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_OLDNAME_ID))
	{
		m_oldname = param_finder[1];
	} 
	if (param_finder.find(param, PARSER_IFRAME_INTERVAL_ID))
	{
		m_iframe_interval = param_finder[1];
	}
	if (param_finder.find(param, PARSER_CHANNEL_ID))
	{
		m_channel = param_finder[1];
	}
	if (param_finder.find(param, PARSER_MANUFACTURER_TYPE_ID))
	{
		m_manufacturer_type = (v_device_manufacturer_type)(atoi(param_finder[1].c_str()));
	}
	if (param_finder.find(param, PARSER_DEVICE_PROTOCOL_TYPE_ID))
	{
		m_device_protocol_type = (v_device_protocol_type)(atoi(param_finder[1].c_str()));
	}
	if (param_finder.find(param, PARSER_DEVICE_TYPE_ID))
	{
		m_device_type = (v_device_type)(atoi(param_finder[1].c_str()));
	}
	if (param_finder.find(param, PARSER_DEVICE_ID_ID))
	{
		m_device_id = param_finder[1].c_str();
	}
	return v_OK;
}

std::string device_uri_parser::id() const
{
	char tmp_id[200];
	sprintf_s(tmp_id, sizeof(tmp_id), "%s:%s%d", m_protocol_name.c_str(), m_ip.c_str(), m_port);
	return tmp_id;
}
std::string device_uri_parser::id_python() const
{
	char tmp_id[200];
	sprintf_s(tmp_id, sizeof(tmp_id), "%s:%s:%d:%s:%s", m_protocol_name.c_str(), m_ip.c_str(), m_port, m_user_name.c_str(), m_password.c_str());
	return tmp_id;
}

int device_uri_parser::_parse_fun_name_id(const char *funcname)
{
	if (!funcname)
	{
		m_func_name_id = func_none_id;
	}
	else
	{
		if (v_str_compare_no_case(func_lists::register_device, funcname))
		{
			m_func_name_id = func_register_device_id;
			m_func_params_value_lists.param_lists.clear();
			m_func_params_value_lists.param_lists.insert(make_pair(func_params_name_lists::device_id_name, m_ip));
			char tmp_port[10] = { 0 };
			_itoa_s(m_port, tmp_port, 10, 10);
			string port_str = tmp_port;
			m_func_params_value_lists.param_lists.insert(make_pair(func_params_name_lists::port_name, port_str));
			m_func_params_value_lists.param_lists.insert(make_pair(func_params_name_lists::user_name, m_user_name));
			m_func_params_value_lists.param_lists.insert(make_pair(func_params_name_lists::password_name, m_password));
		} else if (v_str_compare_no_case(func_lists::unregister_device, funcname))
		{
			m_func_name_id = func_unregister_device_id;
			m_func_params_value_lists.param_lists.clear();
			m_func_params_value_lists.param_lists.insert(make_pair(func_params_name_lists::device_id_name, m_device_id));
		} else if (v_str_compare_no_case(func_lists::get_stream_url, funcname))
		{
			m_func_name_id = func_get_stream_url_id;
			m_func_params_value_lists.param_lists.clear();
			if (!m_stream_id.empty())
			{
				m_func_params_value_lists.param_lists.insert(make_pair(func_params_name_lists::stream_id_name, m_stream_id));
			}
		} else if (v_str_compare_no_case(func_lists::get_device_status, funcname))
		{
			m_func_name_id = func_get_device_status_id;
		} else if (v_str_compare_no_case(func_lists::add_user, funcname))
		{
			m_func_name_id = func_add_user_id;
		} else if (v_str_compare_no_case(func_lists::del_user, funcname))
		{
			m_func_name_id = func_delete_user_id;
		} else if (v_str_compare_no_case(func_lists::alter_user, funcname))
		{
			m_func_name_id = func_alter_user_id;
		}
		else
		{
			m_func_name_id = func_none_id;
		}
	}
	return v_OK;
}

string device_uri_parser::get_psia_uri()
{
	char psia_uri[1024] = {0};
	switch (m_func_name_id)
	{
	case func_get_stream_url_id:
	{
		sprintf_s(psia_uri, sizeof(psia_uri), "http://%s:%d%s%s", m_ip.c_str(), m_port,  psia_service::S_streaming.c_str(), psia_resource::R_channels.c_str(), m_channel);
		/*if (0xff & atoi(m_stream_id.c_str()))
		{
			sprintf_s(psia_uri, sizeof(psia_uri), "http://%s:%d%s%s", m_ip.c_str(), m_port,  psia_service::S_streaming.c_str(), psia_resource::R_channels.c_str(), m_channel);
		}
		else
		{
			sprintf_s(psia_uri, sizeof(psia_uri), "http://%s:%d%s%s/%d", m_ip.c_str(), m_port,  psia_service::S_streaming.c_str(), psia_resource::R_channels.c_str(), atoi(m_channel.c_str()));
		}*/
	}
		break;
	default:
		break;
	}
	return psia_uri;
}

string device_uri_parser::get_onvif_uri()
{
	string psia_params;
	for (auto item : m_func_params_value_lists.param_lists)
	{
		char param[100] = {0};
		sprintf_s(param, "%s:%s&", item.first.c_str(), item.second.c_str());
		psia_params.append(param);
	}
	return psia_params;
}

#define _FUNC_LISTS
#define FUNC_NAME(a, b) const string func_lists::a = b;
#include "func_lists.dat"
#undef	_FUNC_LISTS
#undef  FUNC_NAME

#define _FUNC_PARAMS_NAME_LISTS
#define FUNC_PARAM_NAME(a, b) const string func_params_name_lists::a = b;
#include "func_params_name_lists.dat"
#undef  FUNC_PARAM_NAME
#undef  FUNC_PARAM_NAME