#include "stdafx.h"
#include "device_host.h"
#include "base_device_data_types_defs.h"

device_host::device_host(const char *id, const char *ip, const int &port, const int &device_type, const char *device_name, const char *device_manufacture, const char *user_name, const char *password)
	:m_device_id(id), m_device_ip(ip), m_device_port(port), m_device_type(device_type), m_device_name(device_name), m_device_manufacture(device_manufacture)
{
	if (!m_http_host.is_auth() && user_name && password)
	{
		m_http_host.set_auth(user_name, password);
	}
}


device_host::~device_host()
{
}

int device_host::request(const string &uri, const int &method, char *out_data, long *out_data_len)
{
	int ec = v_OK;
	if (m_http_host.is_auth() && out_data_len && out_data)
	{
		string tmp_out_data;
		if (PSIA_M_GET == method)
		{
			ec = m_http_host.request(Vistek::v_methods::GET, uri, tmp_out_data);
		} else if (PSIA_M_POST == method)
		{
			ec = m_http_host.request(Vistek::v_methods::POST, uri, tmp_out_data);
		} else if (PSIA_M_PUT == method)
		{
			ec = m_http_host.request(Vistek::v_methods::PUT, uri, tmp_out_data);
		}
		else if (PSIA_M_DEL == method)
		{
			ec = m_http_host.request(Vistek::v_methods::DEL, uri, tmp_out_data);
		}
		else
		{
			return v_ERR_BAD_ARGUMENT;
		}
		psia_xml_converter psia_converter(this, tmp_out_data);
		string std_xml = psia_converter.std_xml();
		string psia_xml = psia_converter.psia_xml();

		if (!std_xml.empty())
		{
			if (*out_data_len < std_xml.length())
			{
				memcpy(out_data, std_xml.c_str(), *out_data_len);
			}
			else
			{
				memcpy(out_data, std_xml.c_str(), tmp_out_data.length());
				*out_data_len = std_xml.length();
			}
		}
		return v_OK;
	}
	return v_ERR_BAD_ARGUMENT;
}

string device_host::convert_psia_to_selfdata(const string &psia_data)
{
	return "";
}
