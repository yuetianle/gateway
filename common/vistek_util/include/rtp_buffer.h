#ifndef __rtp_buffer_h__
#define __rtp_buffer_h__
#pragma once
#include <boost/lockfree/stack.hpp>
#include "packet_buffer.h"
#include "rtp_def.h"
#include "tcp_buffer.h"
#include <functional>

using namespace std;
class rtp_buffer;
typedef std::shared_ptr<rtp_buffer> rtp_buff_ptr;

class rtp_buffer_manager : public std::enable_shared_from_this<rtp_buffer_manager>
{
	static atomic<int> s_instance_count;
public:
	rtp_buffer_manager(int segments, int mtu = 1536);
	~rtp_buffer_manager();
public:
	static int _count;
	std::shared_ptr<rtp_buffer> get_buffer();
	void release_buffer(int index);
	uint8_t* get_data();
	size_t get_mtu() const;
	size_t get_segment_number() const;
	size_t get_buffer_capacity() const;
private:
	size_t m_mtu;		//mtu
	size_t m_segment_number;	//num of mtu
	size_t m_buffer_capacity;
	//shared_ptr<std::vector<uint8_t>> m_buffer;
	std::vector<uint8_t> m_buffer;
	std::shared_ptr<boost::lockfree::stack<int>> m_stack;
};

class rtp_buffer : public packet_buffer
{
public:
	static bool unique(const rtp_buff_ptr& rpt1, const rtp_buff_ptr& rpt2)
	{
		bool ret = rpt1->m_index == rpt2->m_index;
		if(ret)
		{
			cout<<"Found the same rtp buffer in container"<<endl;
		}
		return ret;
	}
	static bool compare_little(const rtp_buff_ptr& rpt1, const rtp_buff_ptr& rpt2)
	{
		int diff = rpt1->m_sn - rpt2->m_sn;
		if(diff >= 65500)
		{
			return true;
		}
		else if(diff <= -65500)
		{
			return false;
		}
		else
		{
			return diff < 0;
		}
	}
public:
	rtp_buffer(std::shared_ptr<rtp_buffer_manager>& host, int index);
		

	rtp_buffer(tcp_buf_ptr tcp);
		
	
	virtual ~rtp_buffer(void);
private:
	std::shared_ptr<rtp_buffer_manager> m_host;
	tcp_buf_ptr m_tcp_buffer;
	int m_index;
	int m_sn;
	clock_t m_timestamp;
	int m_rtp_size;
	uint8_t* m_rtp_data;
	int deleteCount;
public:
	void write(uint8_t* data, int size)
	{
		memcpy(get_rtp_data(), data, size);
		complete(size);
	}
	void complete(int size)
	{
		m_timestamp = ::clock();
		m_rtp_size = size;
		m_sn = (int)((uint16_t)boost::asio::detail::socket_ops::network_to_host_short(get_rtp_header()->SN));
	}
public: ///properties
	virtual uint8_t* data() const;

	virtual int datasize() const;

	virtual size_t capacity() const override
	{
		if (m_host != nullptr)
		{
			return m_host->get_mtu();
		}
		return 0;
	}

	inline RTP_Header* get_rtp_header()
	{
		return (RTP_Header*)m_rtp_data;
	}

	inline Playload_Header* get_payload_header()
	{
		return (Playload_Header*)((uint8_t*)m_rtp_data + RTP_HEADER_SIZE);
	}

	inline FU_Header* get_fu_header()
	{
		return (FU_Header*)((uint8_t*)m_rtp_data + RTP_HEADER_SIZE + 1);
	}

	inline int get_rtp_sn() const
	{
		return  m_sn;
	}

	inline uint32_t get_rtp_timestamp()
	{
		return (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(get_rtp_header()->RTP_TS);
	}

	inline uint8_t* get_rtp_data() const
	{
		return (uint8_t*)m_rtp_data;
	}

	inline int get_rtp_size() const
	{
		return m_rtp_size;
	}

	inline uint8_t* get_nalu_data()
	{
		if(get_payload_header()->type <= 23)
		{
			return (uint8_t*)m_rtp_data + RTP_HEADER_SIZE + 1;
		}
		else
		{
			return  (uint8_t*)m_rtp_data + RTP_HEADER_SIZE + 2;
		}
	}

	inline size_t get_nalu_size()
	{
		return m_rtp_size - (get_nalu_data() - (uint8_t*)m_rtp_data);
	}

	inline size_t get_capacity() const
	{
		return m_host->get_mtu();
	}

	clock_t get_sys_timestamp() { return m_timestamp; }

	virtual packet_transport_type get_transport_type() const;

	virtual packet_payload_type get_payload_type() const;
};


extern bool compare_seq_num(int compareBase, const rtp_buff_ptr& rpt1, const rtp_buff_ptr& rpt2);

#endif