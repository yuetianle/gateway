#include "stdafx.h"
#include "packet_buffer.h"


int pkt_allocator::get_free_index()
{
	int index = -1;
	v_smart_lock(lck, mtx_free_index_list);
	if (!free_index_list.empty())
	{
		index = free_index_list.top();
		free_index_list.pop();
	}
	return index;
}

void pkt_allocator::free_buffer(int index)
{
	if (index >= 0 && index < capacity)
	{
		v_smart_lock(lck, mtx_free_index_list);
		free_index_list.push(index);
	}
	else
	{
		cout << "never should be step to here" << endl;
	}
}

pkt_allocator::pkt_allocator(size_t s, size_t n)
: segment_size(s), segment_num(n), capacity(s*n), buffer(capacity), free_list(n)
{
	for (int i = 0; i < capacity; i += segment_size)
	{
		//free_list.push(i);
		free_index_list.push(i);
	}
}

pkt_allocator::~pkt_allocator()
{

}

packet_payload_type string_buffer::get_payload_type() const
{
	return ppt_STRING;
}

packet_transport_type string_buffer::get_transport_type() const
{
	return packet_transport_type::NONE;
}

uint8_t* string_buffer::data() const
{
	return (uint8_t*)m_content.data();
}

size_t string_buffer::capacity() const
{
	return m_content.capacity();
}

int string_buffer::datasize() const
{
	return m_content.length();
}

string_buffer::string_buffer(const std::string& str) : m_content(str)
{

}



int vistek_io_buffer::datasize() const
{
	return m_datasize;
}

size_t vistek_io_buffer::capacity() const
{
	return m_buffer.size();
}

uint8_t* vistek_io_buffer::data() const
{
	return (uint8_t* const)m_buffer.data();
}

packet_payload_type vistek_io_buffer::get_payload_type() const
{
	return m_ppt;
}

void vistek_io_buffer::set_datasize(size_t size)
{
	m_datasize = size;
}

vistek_io_buffer::vistek_io_buffer(size_t capacity, packet_transport_type ptt, packet_payload_type ppt /*= ppt_UNKNOWN*/) 
	: m_buffer(capacity), m_ppt(ppt), m_datasize(0)
{
	m_ptt = ptt;
}

void vistek_io_buffer::resize_buffer(size_t size)
{
	m_buffer.resize(size);
}
