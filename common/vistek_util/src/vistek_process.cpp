#include "stdafx.h"
#include "vistek_process.h"
#include "vistek_file_util.h"
#include "vistek_error_code.h"
#include "vistek_lock_defs.h"
#include "vistek_string_util.h"

using namespace std;

vistek_process::~vistek_process()
{
    stop();
}

vistek_process::vistek_process(const string& rolename, const string& workpath, const string& execname, const string& args, bool bCreateNewConsole /*= true*/)
: m_rolename(rolename), m_workpath(workpath), m_execname(execname), m_arguments(args), m_create_new_window(bCreateNewConsole)
{
#ifdef WIN32
    memset(&m_proc_info, 0, sizeof (m_proc_info));
#endif
}

int vistek_process::stop()
{
    if (trd_proc_control != 0)
    {
        _close_process();
        {
            v_lock(lck, mtx_proc_control);
            m_run_flag = false;
            cond_proc_control.notify_one();
        }

        if (trd_proc_control->joinable())
        {
            trd_proc_control->join();
            trd_proc_control.reset();
        }
    }
    return v_OK;
}

int vistek_process::start()
{
    if (trd_proc_control == 0)
    {
        m_run_flag = true;
        trd_proc_control.reset(new thread(bind(&vistek_process::_proc_exec, this)));
    }
    return v_OK;
}

int vistek_process::_proc_exec()
{
    int64_t duration_ms = 1000;
    bool bExited = true;
    int ret = v_OK;
    while (m_run_flag)
    {
        v_unique_lock(lk, mtx_proc_control);
        chrono::system_clock::time_point start_ts = chrono::system_clock::now();
        if (bExited)
        {
            ret = _open_process();
            if (ret == v_OK)
            {
                bExited = false;
            }
        }
        if (!bExited)
        {
#ifdef WIN32
            int32_t x = WaitForSingleObject(m_proc_info.hProcess, 1000);
            if (x == WAIT_OBJECT_0)
            {
                bExited = true;
            }
#else
			int y = 0;
            int x = waitpid(m_pid, &y, WNOHANG);
            if (x == m_pid || x < 0 )
            {
				_close_process();
                bExited = true;
            }
#endif
        }
        if (!m_run_flag)
        {
            break;
        }
        duration_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_ts).count();
        if (m_run_flag && duration_ms < 3000)
        {
            cond_proc_control.wait_for(lk, chrono::seconds(5));
        }
        if (!m_run_flag)
        {
            break;
        }
    }
    _close_process();
    return v_OK;
}

void vistek_process::_close_process()
{
#ifdef WIN32
    if (m_proc_info.hProcess != nullptr)
    {
        TerminateProcess(m_proc_info.hProcess, 0);
        CloseHandle(m_proc_info.hProcess);
        memset(&m_proc_info, 0, sizeof (m_proc_info));
    }
#else
	cout<<"process: "<<m_pid<<" will be killed..."<<endl;
    if (m_pid != 0)
    {
        kill(m_pid, 9);
		waitpid(m_pid, NULL, 0);
		cout<<"process: "<<m_pid<<" has been killed..."<<endl;
        m_pid = 0;
    }
#endif
}

int vistek_process::_open_process()
{
    boost::filesystem::path w = boost::filesystem::path(m_workpath);
#ifdef WIN32
    boost::filesystem::path p = w / (m_execname + " " + m_arguments);
    STARTUPINFO si = {sizeof (si)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = m_create_new_window;

    wstring cmd = p.native();
    DWORD creationFlag = 0;
    if (m_create_new_window)
    {
        creationFlag |= CREATE_NEW_CONSOLE;
    }
    wcout << "begin to exec command: " << cmd << endl;
    //cout << "begin to exec command: " << cmd.c_str() << endl;
    bool result = CreateProcess(NULL, (LPWSTR) cmd.c_str(), NULL, NULL, FALSE, creationFlag, NULL, w.native().c_str(), &si, &m_proc_info);
    if (result)
    {
        return v_OK;
    }
    else
    {
        return v_ERR_PROCESS_OPEN_FAILURE;
    }
#else
    boost::filesystem::path p = w / (m_execname);
    vector<string> arg_list;
    arg_list.push_back(p.leaf().string());
    v_str_split(m_arguments, arg_list, " ");
    char* argv[64] = {0};

    for (int i = 0; i < arg_list.size(); i++)
    {
        argv[i] = (char*) arg_list[i].c_str();
    }
    __pid_t pid = vfork();
    if (pid < 0)
    {
        cout<<"fork error; failed to create process;"<<endl;
        exit(0);
    }
    else if (pid == 0)
    {
        chdir(p.parent_path().string().c_str());
        execv(p.string().c_str(), argv);
    }
    else
    {
        m_pid = pid;
        cout<<"process: "<<m_pid<<" has been created..."<<endl;
        return v_OK;
    }
#endif
    return v_ERR_Not_Supported;
}

int vistek_process::start_once(int timeout /*= 2000*/)
{
    _close_process();
    int ret = _open_process();
    if (ret == v_OK)
    {
		if (timeout > 0)
		{
#ifdef WIN32
			DWORD result = WaitForSingleObject(m_proc_info.hProcess, timeout);
			if (result == WAIT_OBJECT_0)
			{
				DWORD ec = 0;
				if (GetExitCodeProcess(m_proc_info.hProcess, &ec))
				{
					cout << "KILL Proc returns " << ec << endl;
					if (ec == 128)
					{
						return v_OK;
					}
					return ec;
				}
				return v_OK;
			}
			ret = v_ERR_TIMEOUT;
#endif
		}
    }
    return ret;
}
#ifdef WIN32
char kill_process_format[16] = "/F /IM %s /T";
char kill_process_by_id_format1[16] = "/PID %d /F";
char kill_process_by_id_format[64] = "cmd /c ping localhost -n %d & taskkill /PID %d /F";

extern "C" int v_kill_process(const string& process_name, int timeout /*= 1000*/)
{
    boost::filesystem::path cmdpath(getenv("ComSpec"));
    char kill_process_arg[64] = {0};
    std::sprintf(kill_process_arg, kill_process_format, process_name.c_str());
    vistek_process vp("killtask", cmdpath.parent_path().string(), "taskkill.exe", kill_process_arg);
    return vp.start_once(timeout);
}
extern "C" int v_kill_process_by_id(int pid, int timeout /*= 0*/)
{
	boost::filesystem::path cmdpath(getenv("ComSpec"));
	char kill_process_arg[64] = { 0 };
	std::sprintf(kill_process_arg, kill_process_format, timeout + 1, pid);
	vistek_process vp("killtask", cmdpath.parent_path().string(), "taskkill.exe", kill_process_arg);
	return vp.start_once(timeout);
}
#else
char kill_process_format[16] = "-1 %s";

extern "C" int v_kill_process(const string& process_name, int timeout /*= 1000*/)
{
    boost::filesystem::path cmdpath("/usr/bin");
    char kill_process_arg[64] = {0};
    std::sprintf(kill_process_arg, kill_process_format, process_name.c_str());
    vistek_process vp("killtask", cmdpath.parent_path().string(), "killall", kill_process_arg);
    return vp.start_once(timeout);
}
extern "C" int v_kill_process_by_id(int pid, int timeout)
{
	return v_OK;
}
#endif

