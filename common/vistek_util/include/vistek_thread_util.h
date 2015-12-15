#ifndef vistek_thread_h__
#define vistek_thread_h__

#include <memory>
#include <deque>
#include <string>
#include <tuple>
#include <thread>
#include <chrono>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "vistek_lock_defs.h"
using namespace std;

inline void v_close_thread(std::thread* trd, int& signal, std::mutex& mtx, std::condition_variable& cond)
{
	if (trd != 0)
	{
		if (trd->joinable())
		{
			std::unique_lock<std::mutex> lk(mtx);
			signal = 0;
			cond.notify_all();
			lk.unlock();
			trd->join();
		}
		delete trd;
	}
};
//typedef std::tuple<int, std::timed_mutex, std::condition_variable_any> trd_param;

class vistek_thread
{
public:

	typedef std::shared_ptr<vistek_thread> ptr;
	struct trd_param
	{
		int run_flag = 0;
		std::mutex mtx;
		std::condition_variable_any cond;
	};

	typedef std::function<void(trd_param& param)> trd_proc_func;
protected:
	int m_run_interval = 0;
	std::string m_owner;
	std::thread m_trd;
	mutable trd_param m_param;
	trd_proc_func m_func;
	//int& m_run_flag;
	//std::timed_mutex& m_lck;
	//std::condition_variable_any& m_cond;
public:
	vistek_thread(const std::string& owner)
		: m_owner(owner)//: m_trd_param(0, std::timed_mutex(), std::condition_variable_any()), m_run_flag(get<0>(m_trd_param)), m_lck(get<1>(m_trd_param)), m_cond(get<2>(m_trd_param))
	{

	}
	virtual ~vistek_thread()
	{
		_cleanup();
	}
	int start()
	{
		return _init();
	}
	bool is_started() const
	{
		return m_param.run_flag != 0;
	}
public:
	vistek_thread::trd_param& param() const { return m_param; }

	static ptr create_vistek_thread(const std::string& ownerName, trd_proc_func&& func, bool bStart = true)
	{
		ptr p(new vistek_thread(ownerName));
		p->m_func = func;
		if (bStart)
		{
			p->start();
		}
		return p;
	}
protected:

	virtual int _init()
	{
		v_unique_lock(l, m_param.mtx);
		if (!m_param.run_flag)
		{
			m_param.run_flag = 1;
			m_trd = std::thread(m_func, std::ref(m_param));
		}
		return v_OK;
	}
	virtual int _cleanup()
	{
		thread trd;
		v_unique_lock(l, m_param.mtx);
		if (m_param.run_flag)
		{
			m_param.run_flag = 0;
			m_param.cond.notify_all();
			trd = move(m_trd);
		}
		l.unlock();
		//trd.detach();
		if (trd.joinable())
		{
			trd.join();
		}
		return v_OK;
	}
};

#endif // vistek_thread_h__
