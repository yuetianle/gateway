#ifndef vistek_process_h__
#define vistek_process_h__


#pragma once
#include "pre_std_basic.h"
#include "vistek_file_util.h"
using namespace std;
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
class vistek_process
{
private:
	std::string m_rolename;
	std::string m_workpath;
	std::string m_execname;
	std::string m_arguments;
	bool m_run_flag;
	bool m_create_new_window;
#ifdef WIN32
	PROCESS_INFORMATION m_proc_info;
#else
        int m_pid = 0;
#endif
	shared_ptr<thread> trd_proc_control;
	mutex mtx_proc_control;
	condition_variable_any cond_proc_control;
public:
	vistek_process(const std::string& rolename, const std::string& workpath, const std::string& execname, const std::string& args, bool bCreateNewConsole = true);
	~vistek_process();
private:
	int _open_process();
	void _close_process();
	int _proc_exec();
public:
	int start_once(int timeout = 2000);
	int start();
	int stop();
#ifdef _WIN32
	PROCESS_INFORMATION process_info() { return m_proc_info; }
#else
	int process_pid(){ return m_pid; }
#endif

};
extern "C"
{
	int v_kill_process(const string& process_name, int timeout = 1000); 
	int v_kill_process_by_id(int pid, int timeout = 0);
}
typedef shared_ptr<vistek_process> vistek_proc_ptr;
#endif // vistek_process_h__
