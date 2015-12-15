#include "stdafx.h"
#include "md5_sink.h"


md5_sink::md5_sink()
{
}


md5_sink::~md5_sink()
{
}

int md5_sink::process_internal(basic_buff_ptr buf)
{
	m_md5.update((char*)buf->data(), buf->datasize());
	return v_OK;
}

int md5_sink::open()
{
	if (!m_open_flag)
	{
		m_open_flag = true;
		m_trd_input = async(launch::async, bind(&md5_sink::proc_input, this));
	}
	return v_OK;
}

int md5_sink::close()
{
	base_sink::close();
	m_md5.finalize();
	cout << "MD5 = " << m_md5.hexdigest() << endl;
	return v_OK;
}
