#ifndef base_sink_h__
#define base_sink_h__

#pragma once
#include "tcp_buffer.h"
#include <iostream>
#include <filesystem>
#include <deque>
using namespace std;
class base_sink
{
	const string m_filename;
protected:
	bool m_open_flag;
	deque<basic_buff_ptr> m_msg_queue;
	mutex mtx_msg_queue;
	std::condition_variable m_cond_input;
	mutex mtx_input;
	future<void> m_trd_input;
	virtual void proc_input();
	virtual int process_internal(basic_buff_ptr buf) = 0;
public:
	base_sink();
	virtual ~base_sink();
	virtual bool is_open() const { return m_open_flag; }
	virtual int input(basic_buff_ptr buf);

	virtual int open() = 0;
	virtual int close();
};
typedef shared_ptr<base_sink> base_sink_ptr;
#endif // base_sink_h__