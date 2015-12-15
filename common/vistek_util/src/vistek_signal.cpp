#include "stdafx.h"
#include "vistek_signal.h"
#include "vistek_lock_defs.h"
using namespace std;
bool vistek_event_loop::is_shutdown() const
{
	return m_is_shutdown;
}

void vistek_event_loop::set_nolock()
{
	m_is_set = true;
	cond_event.notify_all();
}

void vistek_event_loop::set()
{
	v_lock(lck, mtx_event);
	m_is_set = true;
	cond_event.notify_all();
}

void vistek_event_loop::stop()
{
	//v_lock(lck, mtx_operation);
	m_is_shutdown = true;
	if (trd_event_loop!=nullptr)
	{
		{
			try
			{
				//v_lock(lck, mtx_event);
				cond_event.notify_all();
			}
			catch (std::exception& e)
			{
				cout << e.what() << endl;
			}
		}
		if (trd_event_loop->joinable())
		{
			trd_event_loop->join();
		}
		m_notify.disconnect_all_slots();
		trd_event_loop.reset();
	}
	//m_notify.disconnect_all_slots();
}

boost::signals2::connection vistek_event_loop::start(vistek_event_notify_signal::slot_type cb)
{
	stop();
	auto conn = m_notify.connect(cb);
	v_lock(lck, mtx_operation);
	if (trd_event_loop == 0)
	{
		m_is_shutdown = false;
		trd_event_loop.reset(new thread(std::bind(&vistek_event_loop::proc, this)));
	}
	return conn;
}

std::string vistek_event_loop::get_event_name() const
{
	return m_event_name;
}

vistek_event_loop::vistek_event_loop(const std::string& name, int timeout_ms) : m_event_name(name), trd_event_loop(0), m_is_shutdown(false), m_is_set(false), m_timeout_ms(timeout_ms)
{

}

int vistek_event_loop::proc()
{
	m_notify(this, ve_Begin);
	try
	{
		while (!m_is_shutdown)
		{
			std::unique_lock<std::mutex> lck(mtx_event);
			if (m_timeout_ms > 0)
			{
                std::cv_status ret = cond_event.wait_for(lck, std::chrono::milliseconds(m_timeout_ms));
				if (m_is_shutdown)
				{
					break;
				}
				if (ret == std::cv_status::no_timeout)
				{
					if (!m_notify.empty())
					{
						m_notify(this, ve_Event);
					}
				}
				else
				{
					if (!m_notify.empty())
					{
						m_notify(this, ve_Timeout);
					}
				}
				if (!m_is_set)
				{
					continue;
				}
			}
			else
			{
				cond_event.wait(lck);
				if (m_is_shutdown)
				{
					break;
				}
				if (!m_is_set)
				{
					continue;
				}
				if (!m_notify.empty())
				{
					m_notify(this, ve_Event);
				}
			}
			m_is_set = false;
		}
		if (!m_notify.empty())
		{
			m_notify(this, ve_End);
		}
		else
		{
			cout << "failed to send ve_end signal" << endl;
		}
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
	return 0;
}

vistek_event_loop::~vistek_event_loop()
{
	stop();
}
