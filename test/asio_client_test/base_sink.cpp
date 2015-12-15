#include "stdafx.h"
#include "base_sink.h"

void base_sink::proc_input()
{
	deque<basic_buff_ptr> temp;
	while (m_open_flag)
	{
		{
			v_lock(write_lck, mtx_msg_queue);
			if (m_msg_queue.size() > 0)
			{
				temp.swap(m_msg_queue);
			}
		}
		for (auto& x : temp)
		{
			process_internal(x);
		}
		temp.clear();
		chrono::system_clock::time_point tp = chrono::system_clock::now() + chrono::milliseconds(500);
		v_unique_lock(lck, mtx_input);
		m_cond_input.wait_for(lck, chrono::milliseconds(100));
		if (!m_open_flag)
		{
			break;
		}
	}
	for (auto& x : m_msg_queue)
	{
		process_internal(x);
	}
	m_msg_queue.clear();
}

int base_sink::input(basic_buff_ptr buf)
{
	v_lock(lck, mtx_msg_queue);
	m_msg_queue.push_back(buf);
	return v_OK;
}

base_sink::base_sink()
: m_open_flag(false)
{

}

base_sink::~base_sink()
{
	
}

int base_sink::close()
{
	m_open_flag = false;
	{
		v_unique_lock(lck, mtx_input);
		m_cond_input.notify_all();
	}
	if (m_trd_input.valid())
	{
		m_trd_input.get();
	}
	return v_OK;
}
