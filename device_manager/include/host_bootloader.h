#ifndef _HOST_BOOTLOADER_H_
#define _HOST_BOOTLOADER_H_
#pragma once
#include "vistek_process.h"
typedef tuple<string, string, string, string, int> ProcParams;//role_name work_path exec_name arguments proc_counts
class host_bootloader: public enable_shared_from_this<host_bootloader>
{
public:
	host_bootloader();
	~host_bootloader();
public:
	bool is_start() { return m_is_start; }
	int start_hosts();
	int stop_hosts();
private:
	int _load_host_cfg(const char *cfg_file);
	int _remove_invaild_cfg(const char *cfg_dir);
	//int _write_host_listen_ports(vistek_proc_ptr proc, const string &manuc);
	vector<ProcParams> m_hosts_info;
	vector<vistek_proc_ptr> m_host_proc_lists;
	const string m_cfg_file = "host_cfg.xml";
	const string m_host_listen_dir = "hostboot";
	pugi::xml_document m_host_lists_doc;
	atomic<bool> m_is_start;
};
typedef shared_ptr<host_bootloader> host_bootloader_ptr;
#endif
