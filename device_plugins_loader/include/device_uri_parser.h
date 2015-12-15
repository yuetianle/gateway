#ifndef _DEVICE_URI_PARSER_H
#define _DEVICE_URI_PARSER_H
#pragma once
#include "base_device_data_types_defs.h"
#include "psiacmd.h"
#define PARSER_FUNC_NAME_ID			"func=([^&]+)"
#define PARSER_STREAM_ID			"stream_id=([^&]+)"
#define PARSER_CONTENT_ID			"content=([^&]+)"
#define PARSER_USERNAME_ID			"user=([^&]+)"
#define PARSER_PASSWORD_ID			"password=([^&]+)"
#define PARSER_TYPE_ID				"type=([^&]+)"
#define PARSER_FPS_ID				"fps=([^&]+)"
#define PARSER_CODEC_ID				"codec=([^&]+)"
#define PARSER_IFRAME_INTERVAL_ID	"iframe_interval=([^&]+)"
#define PARSER_OLDNAME_ID			"old_name=([^&]+)"
#define PARSER_CHANNEL_ID			"channel=([^&]+)"
#define PARSER_DEVICE_PROTOCOL_TYPE_ID	"protocol_type=([^&]+)"
#define PARSER_DEVICE_TYPE_ID			"device_type=([^&]+)"
#define PARSER_MANUFACTURER_TYPE_ID		"manufacturer_type=([^&]+)"
#define PARSER_DEVICE_ID_ID			"device_id=([^&]+)"
#define PARSER_SESSION_ID_ID		"session_id=([^&]+)"

class func_lists
{
public:
#define _FUNC_LISTS
#define FUNC_NAME(a, b) const static string a;
#include "func_lists.dat"
#undef  _FUNC_LISTS
#undef  FUNC_NAME
};
class func_params_name_lists
{
public:
#define _FUNC_PARAMS_NAME_LISTS
#define FUNC_PARAM_NAME(a, b) const static string a;
#include "func_params_name_lists.dat"
#undef _FUNC_PARAMS_NAME_LISTS
#undef FUNC_PARAM_NAME
};

struct func_params
{
public:
	map<string, string> param_lists;
	inline int params_cout(){ return param_lists.size(); }
};

class device_uri_parser
{
public:
	device_uri_parser(const string &cmd_uri);
	~device_uri_parser();
	enum parser_cmd_type
	{
		CMD_C = 0,
		CMD_PYTHON = 1
	};
	enum func_name_index
	{
		func_none_id				 = -1,
		func_register_device_id		 = 0,
		func_unregister_device_id	 = 1,
		func_get_stream_url_id		 = 2,
		func_get_device_status_id	 = 3,
#pragma region user
		func_add_user_id			 = 10,
		func_delete_user_id			 = 12,
		func_alter_user_id			 = 14,
#pragma endregion user
	};
public:
	string  uri() const { return m_cmd_uri; }
	string	protocol_name() const { return m_protocol_name; }
	string	ip() const { return m_ip; }
	int		port() const { return m_port; }
	int	cmd_type() const { return m_cmd_type; }
	string  cmd_resource() const { return m_cmd_resource; }
	string  id() const;
	string  id_python() const;

	string  func_name() const { return m_func_name; }
	int		func_name_id() const { return m_func_name_id; }
	string	params() const { return m_params; }
	string  stream_id() const { return m_stream_id; }
	string  content() const { return m_content; }
	string  user_name() const { return m_user_name; }
	string  old_name() const { return m_oldname; }
	string  password() const { return m_password; }
	string  channel() const { return m_channel; }
	v_device_protocol_type		protocol_type() const \
	{ return m_device_protocol_type; }
	v_device_type device_type() const \
	{ return m_device_type; }
	v_device_manufacturer_type		device_manufacture_type() const \
	{ return m_manufacturer_type; }
	string	device_id() const{ return m_device_id; }
	string  session_id() const{ return m_session_id; }
	/**
	 * @brief 将device_uri转换为标准PSIAuri.
	 * @return psia uri.
	 */
	string get_psia_uri();
	string get_onvif_uri();
private:
	int _parse_uri_full(const char *cmd);
	int _parse_query_string(const char *cmd_params);
	int _parse_fun_name_id(const char *funcname);
private:

	string  m_cmd_uri;
	string	m_protocol_name;
	string	m_ip;
	int		m_port;
	parser_cmd_type	m_cmd_type;
	string	m_cmd_resource;
	string	m_python_module_name;

	string  m_params;/*<< 参数字符串.*/
	string	m_func_name;/*<< 函数名称.*/
	string	m_stream_id;/*<< 流ID*/
	string	m_content;
	string  m_user_name;/*<< 用户名.*/
	string  m_password;/*<< 密码.*/
	string  m_type;/*<< */
	string  m_fps;/*<< */
	string  m_codec;/*<< 编码类型.*/
	string  m_oldname;/*<< 用户名.*/
	string  m_iframe_interval;/*<< I帧间隔.*/
	string  m_channel;/*<< 通道号.*/
	v_device_type m_device_type;/*<< 设备类型.*/
	v_device_protocol_type m_device_protocol_type;/*<< 设备协议类型.*/
	v_device_manufacturer_type m_manufacturer_type;/*<< 厂商类型.*/
	string  m_device_id;/*<< 设备唯一标识.*/
	string  m_session_id;/*<< 设备建立的SessionID*/
	int		m_func_name_id;/*<< 函数名ID*/
	std::map<func_name_index, string> m_func_maps;/*<< 函数表索引, 函数名*/
	func_params m_func_params_value_lists;/*<< 函数参数值列表*/
};
#endif
