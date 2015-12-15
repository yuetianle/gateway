#ifndef _FUN_WRAP_H_
#define _FUN_WRAP_H_
typedef struct proc_util_wrap
{
#define WRAP_DLL(f) f = (decltype(f))GetProcAddress(dll, #f);
#define EXPORT_FUNCTION(f) decltype(&::f) f;		 
	void wrap(HMODULE dll)
	{
		memset(reserved, 0, sizeof(reserved));
		if (dll)
		{
			WRAP_DLL(register_device);
			WRAP_DLL(unregister_device);
			WRAP_DLL(get_stream_url);
			WRAP_DLL(get_device_status);
			WRAP_DLL(test);
		}
	}
	void unwrap()
	{
		register_device = 0;
		unregister_device = 0;
		get_stream_url = 0;
		get_device_status = 0;
		test = 0;
	}
	bool is_good()
	{
		if (register_device && unregister_device && get_stream_url && get_device_status && test)
		{
			return true;
		}
		return false;
	}
	union
	{
		struct {
			EXPORT_FUNCTION(register_device);
			EXPORT_FUNCTION(unregister_device);
			EXPORT_FUNCTION(get_stream_url);
			EXPORT_FUNCTION(get_device_status);
			EXPORT_FUNCTION(test);
		};
		char reserved[4096];
	};

}proc_util_func_wrap_t;
#endif
