#ifndef __tcp_buffer_h__
#define __tcp_buffer_h__
#pragma once
#include "packet_buffer.h"
using namespace std;

typedef function<void()> DeallocateFunc;

class tcp_buffer : public packet_buffer
{
public:
	tcp_buffer(uint8_t* data, int capacity, int datasize, DeallocateFunc handler)
		: m_data(data), m_capacity(capacity), m_datasize(datasize), m_deallocate_handler(handler)
	{

	}
	~tcp_buffer()
	{
		m_deallocate_handler();
	}

	uint8_t* data() const { return m_data; }
	int datasize() const { return m_datasize; }
	virtual size_t capacity() const override { return m_capacity; }
	void set_datasize(int val) { m_datasize = val; }

	virtual packet_transport_type get_transport_type() const;

	virtual packet_payload_type get_payload_type() const;

private:
	uint8_t* m_data;
	size_t m_capacity;
	int m_datasize;
	DeallocateFunc m_deallocate_handler;
};

typedef shared_ptr<tcp_buffer> tcp_buf_ptr;

class tcp_buffer_allocator : public enable_shared_from_this<tcp_buffer_allocator>
{
public:
	tcp_buffer_allocator(uint32_t segmentNum, uint32_t segmentSize)
		: m_sigmentNum(segmentNum), m_sigmentSize(segmentSize), m_capacity(segmentSize * segmentNum), m_buffer(m_capacity), m_free_index(segmentNum)
	{
		for (uint32_t i = 0; i < m_capacity; i += m_sigmentSize)
		{
			m_free_index.push(i);
		}
	}

	~tcp_buffer_allocator(void)
	{

	}

	tcp_buf_ptr get_buffer()
	{
		uint32_t index = 0;
		if (m_free_index.pop(index))
		{
			tcp_buf_ptr ret(new tcp_buffer(m_buffer.data() + index, m_sigmentSize, 0,
				std::bind(&tcp_buffer_allocator::free_buffer, shared_from_this(), index)));
			return ret;
		}
		return nullptr;
	}


	tcp_buf_ptr write_some(uint8_t* data, int& size);

	void free_buffer(uint32_t index)
	{
		if (index < m_capacity)
		{
			m_free_index.push(index);
		}
	}
	uint32_t capacity() const { return m_capacity; }
	uint32_t SigmentNum() const { return m_sigmentNum; }
	uint32_t SigmentSize() const { return m_sigmentSize; }

private:
	uint32_t m_sigmentNum;
	uint32_t m_sigmentSize;
	uint32_t m_capacity;
	vector<uint8_t> m_buffer;
	boost::lockfree::stack<uint32_t> m_free_index;
};
typedef shared_ptr<tcp_buffer_allocator> tcp_buffer_allocator_ptr;
#endif