#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <deque>
#include <string>
#include "vistek_lock_defs.h"
typedef function<void()> vistek_action;
using namespace std;
#define VAT_USE_SPIN_LOCK

class vistek_action_thread
{
public:
	typedef std::shared_ptr<vistek_action_thread> ptr;
protected:
	int m_run_flag = 0;
	int m_run_interval = 0;
	std::string m_owner;
	std::thread trd_action;
	vistek_smart_locker lck_action_list;
	std::deque<pair<std::string, vistek_action>> m_action_list;
	std::mutex mtx_action;
	std::condition_variable_any cond_action;
private:
	vistek_action_thread(const std::string owner);
public:
	virtual ~vistek_action_thread();
public:
	static ptr create_vistek_action_thread(const std::string& ownerName, int runInterval = 1000);
	void push(const std::string& taskName, vistek_action&& action);
	void set_interval(int ms)
	{
		if (ms > 0)
		{
			m_run_interval = ms;
		}
	}
protected:
	virtual int _init();
	virtual int _cleanup();
	virtual void _proc_action();
};
