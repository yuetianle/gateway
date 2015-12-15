// device_gateway_testV1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "device_plugins_loader.h"
#include "device_data_types_defs.h"
#include "pugiconfig.hpp"
#include "pugixml.hpp"
void test_SDK()
{
	//register_gateway_device(V_PORTOCOL_SDK, "221", V_DEVICE_IPC, "hikipc", V_MANUFACTURER_HIKVISION, "172.16.1.221", 8000, "admin", "12345");
	char out_data[1024] = { 0 };
	long out_data_len = 1024;
	load_device_plugin(V_MANUFACTURER_HIKVISION);
	string url = "http://172.16.1.221:8000/device/meida?func=register_device&user=admin&password=12345&manufacturer_type=5&protocol_type=0&device_type=0";
	device_exec_cmd("111", url.c_str(), nullptr, out_data, &out_data_len);
	out_data_len = 1024;
	device_exec_cmd("111", "http://172.16.1.221:8000/device/meida?func=get_stream_url&stream_id=1", nullptr, out_data, &out_data_len);
	//device_exec_cmd("111", "http://172.16.1.221:80/Security/user?func=add_user&user=onvif1&password=12345", nullptr, out_data, &out_data_len);
	//device_exec_cmd("111", "http://172.16.1.221:80/security/user?func=del_user&user=onvif1", nullptr, out_data, &out_data_len);
	cin.get();
}
void test_psia()
{
	//register_gateway_device(V_PROTOCOL_PSIA, "221", V_DEVICE_IPC, "hikipc", V_MANUFACTURER_PSIA, "172.16.1.190", 8000, "admin", "12345");
	load_device_plugin(V_MANUFACTURER_PSIA);
	char out_data[43024] = { 0 };
	long out_data_len = 43024;
	string url = "http://172.16.1.190:80/device/meida?func=register_device&user=admin&password=12345&manufacturer_type=2&protocol_type=2&device_type=0";
	device_exec_cmd("111", url.c_str(), nullptr, out_data, &out_data_len);
	out_data_len = 43024;
	device_exec_cmd("111", "http://172.16.1.190:80/device/meida?func=get_stream_url&stream_id=1", nullptr, out_data, &out_data_len);
	pugi::xml_document doc;
	doc.load_string(out_data);
	doc.save_file("psia_uri.xml");
	cin.get();
}

void test_onvif()
{
	//register_gateway_device(V_PROTOCOL_ONVIF, "221", V_DEVICE_IPC, "hikipc", V_MANUFACTURER_ONVIF, "172.16.1.190", 80, "admin", "12345");
	char out_data[1024] = { 0 };
	long out_data_len = 1024;
	load_device_plugin(V_MANUFACTURER_ONVIF);
	device_exec_cmd("221", "http://172.16.1.190:80/device/meida?func=register_device&user=admin&password=12345&manufacturer_type=1&protocol_type=1&device_type=0", nullptr, out_data, &out_data_len);
	out_data_len = 1024;
	device_exec_cmd("221", "http://172.16.1.190:80/device/meida?func=get_stream_url", nullptr, out_data, &out_data_len);
	pugi::xml_document doc;
	doc.load_string(out_data);
	doc.save_file("onvif_uri.xml");
	cin.get();
}
int _tmain(int argc, _TCHAR* argv[])
{
	test_psia();
	//test_SDK();
	//test_onvif();
	return 0;
}

