#ifndef _MODULE_HELPER_H_
#define _MODULE_HELPER_H_
#pragma once
class module_helper
{
public:
	module_helper(const string &cfg_name);
	~module_helper();
	static module_helper * get_instance();
	int get_dll_handle(const string &dll_name, HMODULE *dll_handle);
private:
	int _load_plugins(const string &cfg_name);
	int _load_current_plugins_cfg(const char* cfg_file);
	data_cache_ex<string, HMODULE> m_plugins_map;
};
#endif

