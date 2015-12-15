#ifndef _DEVICE_HOST_H_
#define _DEVICE_HOST_H_
#pragma once
#include "device_plugins_helper.h"
#include "psia_xml_converter.h"
#include "http_clients.h"
class device_host
{
public:
	device_host(const char *id, const char *ip, const int &port, const int &device_type, const char *device_name, const char *device_manufacture, const char *user_name, const char *password);
	~device_host();
public:	
	int request(const string &uri, const int &method, char *out_data, long *out_data_len);
	string device_id() const { return m_device_id; }
	int	   device_type() const { return m_device_type; }
	string device_name() const { return m_device_name; }
	string device_manufacture() const { return m_device_manufacture; }
	string device_ip() const { return m_device_ip; }
	long device_port() const { return m_device_port; }
private:
	Vistek::v_http_client m_http_host;
	/**
	 * @brief ��curl��Ĵ�����ת��Ϊϵͳ������.
	 * @param [in] curl_err curl��׼������.
	 * @return ϵͳ������.
	 * 
	 */
	int _convert_curl_to_sys_error(const int &curl_err);
	
	/**
	 * @brief ����׼���ת��Ϊ�Զ������.
	 * @param [in] psia_data.
	 * @return self data.
	 */
	string convert_psia_to_selfdata(const string &psia_data);
private:
	string m_device_id;
	int    m_device_type;
	string m_device_name;
	string m_device_manufacture;
	string m_device_ip;
	long   m_device_port;
};
#endif
