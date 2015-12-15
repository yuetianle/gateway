#ifndef file_sink_h__
#define file_sink_h__


#pragma once
#include "tcp_buffer.h"
#include <iostream>
#include <filesystem>
#include <deque>
#include "base_sink.h"
using namespace std;
class file_sink : public base_sink
{
	const string m_filename;
	std::fstream m_file;
protected:
	virtual int process_internal(basic_buff_ptr buf) override;
public:
	file_sink(const string& fn);
	~file_sink();
	virtual int open() override;
	virtual int close() override;
};
typedef shared_ptr<file_sink> file_sink_ptr;

#endif // file_sink_h__
