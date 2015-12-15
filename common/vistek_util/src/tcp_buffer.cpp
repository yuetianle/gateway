#include "stdafx.h"
#include "../include/tcp_buffer.h"



tcp_buf_ptr tcp_buffer_allocator::write_some( uint8_t* data, int& size )
{
	uint32_t index = 0;
	if (m_free_index.pop(index))
	{
		int writenSize = size > m_sigmentSize ? m_sigmentSize : size;
		uint8_t* output = m_buffer.data() + index;
		memcpy(output, data, writenSize);
		tcp_buf_ptr ret(new tcp_buffer(output, m_sigmentSize, writenSize,
			std::bind(&tcp_buffer_allocator::free_buffer, shared_from_this(), index)));
		size -= writenSize;
		return ret;
	}
	return nullptr;
}


packet_transport_type tcp_buffer::get_transport_type() const
{
	return packet_transport_type::TCP;
}

packet_payload_type tcp_buffer::get_payload_type() const
{
	return packet_payload_type::ppt_UNKNOWN;
}
