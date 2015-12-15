#include "stdafx.h"
#include "rtp_buffer.h"

using namespace std;

size_t rtp_buffer_manager::get_buffer_capacity() const
{
	return m_buffer_capacity;
}

size_t rtp_buffer_manager::get_segment_number() const
{
	return m_segment_number;
}

size_t rtp_buffer_manager::get_mtu() const
{
	return m_mtu;
}

uint8_t* rtp_buffer_manager::get_data()
{
	return m_buffer.data();
}

void rtp_buffer_manager::release_buffer( int index )
{
	//if(index>0 && index < m_buffer_capacity)
	{
		//lock_guard<recursive_mutex> lck(m_mtx);
		m_stack->push(index);
	}
}

std::shared_ptr<rtp_buffer> rtp_buffer_manager::get_buffer()
{
	int index = -1;
	//lock_guard<recursive_mutex> lck(m_mtx);
	if(!m_stack->empty())
	{
		if(m_stack->pop(index))
		{
            auto this_ptr = shared_from_this();
			std::shared_ptr<rtp_buffer> result(new rtp_buffer(this_ptr, index));
			_count++;
			return result;
		}
	}
	return nullptr;
}

rtp_buffer_manager::rtp_buffer_manager( int segments, int mtu /*= 1536*/ )
: m_mtu(mtu), m_segment_number(segments), m_buffer_capacity(mtu*segments), m_buffer(m_buffer_capacity), m_stack(new boost::lockfree::stack<int>(segments))
{
	/*auto x = new boost::lockfree::stack<int>(segments);
	m_stack.reset(x);*/
	for(size_t i = 0; i < m_buffer_capacity; i += m_mtu)
	{
		m_stack->push(i);
	}

	int count = s_instance_count.fetch_add(1) + 1;
	cout<<"++++ rtp_buffer_manager [ "<<count<<" ]"<<endl;
}

rtp_buffer_manager::~rtp_buffer_manager()
{
	int count = s_instance_count.fetch_sub(1) - 1;
	cout<<"---- rtp_buffer_manager [ "<<count<<" ]"<<endl;
}

atomic<int> rtp_buffer_manager::s_instance_count(0);

int rtp_buffer_manager::_count=0;

extern bool compare_seq_num( int compareBase, const rtp_buff_ptr& rpt1, const rtp_buff_ptr& rpt2 )
{
	if(rpt1 == nullptr)
	{
		return true;
	}
	if(rpt2 == nullptr)
	{
		return false;
	}
	uint16_t seq1 = rpt1->get_rtp_sn() + compareBase;
	uint16_t seq2 = rpt2->get_rtp_sn() + compareBase;
	return seq1 < seq2;
}

uint8_t* rtp_buffer::data() const
{
	return m_rtp_data;
}

int rtp_buffer::datasize() const
{
	return m_rtp_size;
}

rtp_buffer::rtp_buffer( std::shared_ptr<rtp_buffer_manager>& host, int index )
	: m_host(host), m_tcp_buffer(nullptr)
{
	deleteCount = 0;
	m_host = host;
	m_index = index;
	m_rtp_size = 0;
	m_sn = 0;
	m_rtp_data = (uint8_t*)(m_host->get_data() + index);
}

rtp_buffer::rtp_buffer( tcp_buf_ptr tcp )
	: m_host(nullptr), m_tcp_buffer(tcp)
{
	m_index = 0;
	m_sn = 0;
	m_rtp_size = m_tcp_buffer->datasize() - 4;
	m_rtp_data = m_tcp_buffer->data() + 4;
	m_sn = (int)((uint16_t)boost::asio::detail::socket_ops::network_to_host_short(get_rtp_header()->SN));
}

rtp_buffer::~rtp_buffer( void )
{
	if(m_host!=nullptr)
	{
		m_host->release_buffer(m_index);
	}
}

packet_transport_type rtp_buffer::get_transport_type() const
{
	return packet_transport_type::RTP;
}

packet_payload_type rtp_buffer::get_payload_type() const
{
	return packet_payload_type::ppt_UNKNOWN;
}
