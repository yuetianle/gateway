#ifndef md5_sink_h__
#define md5_sink_h__

#pragma once
#include "base_sink.h"
#include "md5.h"
class md5_sink : public base_sink
{
	MD5 m_md5;
public:
	md5_sink();
	~md5_sink();

	virtual int process_internal(basic_buff_ptr buf);

	virtual int open();

	virtual int close();
};

#endif // md5_sink_h__
