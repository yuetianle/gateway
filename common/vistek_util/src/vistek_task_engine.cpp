#include "stdafx.h"
#include "../include/vistek_task_engine.h"
#include "vistek_perf_marker.h"
using namespace std;


vistek_action_thread::vistek_action_thread(const std::string owner) : m_owner(owner)
{

}


vistek_action_thread::~vistek_action_thread()
{
	_cleanup();
}

vistek_action_thread::ptr vistek_action_thread::create_vistek_action_thread(const string& ownerName, int runInterval /*= 5000 */)
{
	cout << "begin create_vistek_action_thread(" << ownerName << ")" << endl;
	vistek_action_thread* ptr = new vistek_action_thread(ownerName);
	ptr->m_run_interval = runInterval;
	ptr->_init();
	ptr->set_interval(runInterval);
	cout << "end create_vistek_action_thread(" << ownerName << ")" << endl;
	shared_ptr<vistek_action_thread> result(ptr);
	return move(result); 
}

void vistek_action_thread::push(const string& taskName, vistek_action&& action)
{
	v_smart_lock(lck, lck_action_list); 
	cond_action.notify_one();
	m_action_list.push_back(pair<string, vistek_action>(taskName, action));
}

void g_vistek_action_thread_proc_action(int& run_flag, vistek_smart_locker& lck_action_list, deque<pair<string, vistek_action>>& action_list)
{
	deque<pair<string, vistek_action>> temp;
	while (run_flag)
	{
		this_thread::sleep_for(chrono::seconds(1));
		if (run_flag)
		{
			v_smart_lock(lck, lck_action_list);
			if (!action_list.empty())
			{
				action_list.swap(temp);
			}
		}
		if (!temp.empty())
		{
			for (auto& x : temp)
			{
				if (x.second != nullptr)
				{
					try
					{
#ifdef _DEBUG
						vperf_begin(task_marker, d);
#endif
						x.second();
#ifdef _DEBUG
						vperf_end(task_marker, d, x.first);
#endif
					}
					catch (std::exception& e)
					{
						cout << "failed to run task: " << x.first << "; err: " << e.what() << endl;
					}
				}
			}
			temp.clear();
		}
	}
}

int vistek_action_thread::_init()
{
	v_unique_lock(lk, mtx_action);
	if (!m_run_flag)
	{
		m_run_flag = 1;
		//async(launch::any, [&]()
		//{
			//trd_action = thread(&g_vistek_action_thread_proc_action, std::ref(m_run_flag), std::ref(mtx_action_list), std::ref(m_action_list));
		lk.unlock();
		trd_action = thread(bind(&vistek_action_thread::_proc_action, this));
		//});
	}
	return v_OK;
}

int vistek_action_thread::_cleanup()
{
	/*thread tmp;
	{
		v_unique_lock(lk, mtx_action);
		m_run_flag = 0;
		cond_action.notify_one();
		tmp = move(trd_action);
	}
	if (tmp.joinable())
	{
		tmp.join();
	}*/
	{
		//v_unique_lock(lk, mtx_action);
		m_run_flag = 0;
		cond_action.notify_one();
	}
	//trd_action.detach();
	if (trd_action.joinable())
	{
		trd_action.join();
	}
	return v_OK;
}

void vistek_action_thread::_proc_action()
{
	deque<pair<string, vistek_action>> temp;
	while (m_run_flag)
	{
		v_unique_lock(lk, mtx_action);
		cond_action.wait_for(lk, chrono::milliseconds(m_run_interval));
		//this_thread::sleep_for(chrono::milliseconds(m_run_interval));
		if (m_run_flag)
		{
			v_smart_lock(lck, lck_action_list);
			if (!m_action_list.empty())
			{
				m_action_list.swap(temp);
			}
		}
		if (!temp.empty())
		{
			for (auto& x : temp)
			{
				if (x.second != nullptr)
				{
					try
					{
#ifdef _DEBUG
						vperf_begin(task_marker, d);
#endif
						x.second();
#ifdef _DEBUG
						vperf_end(task_marker, d, x.first);
#endif
					}
					catch (std::exception& e)
					{
						cout << "failed to run task: " << x.first << "; err: " << e.what() << endl;
					}
				}
			}
			temp.clear();
		}
	}
	cout << "action thread of " << m_owner << "will be terminated. " << endl;
}
