// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DEVICE_GATEWAY_HIKVISIONV1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DEVICE_GATEWAY_HIKVISIONV1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef DEVICE_GATEWAY_HIKVISIONV1_EXPORTS
#define DEVICE_GATEWAY_HIKVISIONV1_API __declspec(dllexport)
#else
#define DEVICE_GATEWAY_HIKVISIONV1_API __declspec(dllimport)
#endif
DEVICE_GATEWAY_HIKVISIONV1_API int register_device(const char *device_id, const int &device_type, const char *device_name, const int &device_manufacture, const char *device_ip, const long &device_port, const char *user_name, const char *user_pwd)
{
	if (!device_id || device_ip || user_name || user_pwd)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	if ()
	{
	}
	return v_OK;
}

DEVICE_GATEWAY_HIKVISIONV1_API int unregister_device(const char *device_id)
{

}
