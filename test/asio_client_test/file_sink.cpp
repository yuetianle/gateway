#include "stdafx.h"
#include "file_sink.h"


int file_sink::process_internal(basic_buff_ptr buf)
{
	if (buf == nullptr)
	{
		return v_ERR_Bad_Ptr;
	}
	if (!m_file.is_open())
	{
		return v_ERR_Open_File_Failure;
	}
	m_file.write((char*)buf->data(), buf->datasize());
	return v_OK;
}

file_sink::~file_sink()
{
	close();
}

file_sink::file_sink(const string& fn) : m_filename(fn)
{

}

int file_sink::open()
{
	if (!m_open_flag)
	{
		m_file.open(m_filename, ios::out | ios::binary);
		if (!m_file.is_open())
		{
			return v_ERR_Open_File_Failure;
		}
		m_open_flag = true;
		m_trd_input = async(launch::async, bind(&file_sink::proc_input, this));
	}
	return v_OK;
}

int file_sink::close()
{
	base_sink::close();
	m_file.close();
	return v_OK;
}
