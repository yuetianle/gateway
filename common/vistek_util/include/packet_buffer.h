#ifndef packet_buffer_h__
#define packet_buffer_h__

#pragma once
//#include "iolib_exports.h"
#include "boost/lockfree/stack.hpp"
#include "vistek_lock_defs.h"
#include "vistek_vod_defs.h"
#include "vistek_media_defs.h"
#include <stack>
#ifdef USE_SPIN_IN_ALLOCATOR
#include "vistek_spin_lock.h"
#endif
using namespace std;
struct pkt_allocator : public std::enable_shared_from_this<pkt_allocator>
{
	size_t segment_size;
	size_t segment_num;
	size_t capacity;
	std::vector<uint8_t> buffer;
	boost::lockfree::stack<int> free_list;
	stack<int> free_index_list;

	vistek_smart_locker mtx_free_index_list;
	pkt_allocator(size_t seg_size, size_t num);
	~pkt_allocator();
	int get_free_index();
	void free_buffer(int index);
};
typedef std::shared_ptr<pkt_allocator> pkt_allocator_ptr;

class packet_buffer;
class packet_buffer
{
protected:
	packet_transport_type m_ptt;
public:
	typedef shared_ptr<packet_buffer> ptr;
	virtual packet_payload_type get_payload_type() const = 0;
	virtual uint8_t* data() const = 0;
	virtual size_t capacity() const = 0;
	virtual int datasize() const = 0;
	virtual packet_transport_type get_transport_type() const 
	{
		return m_ptt;
	};
	virtual void set_transport_type(packet_transport_type val) { m_ptt = val; }
};
typedef std::shared_ptr<packet_buffer> pkt_buf_ptr;

class vistek_io_buffer : public packet_buffer
{
protected:
	std::vector<uint8_t> m_buffer;
	size_t m_datasize;
	packet_payload_type m_ppt;
public:
	typedef std::shared_ptr<vistek_io_buffer> ptr;
	vistek_io_buffer(size_t capacity, packet_transport_type ptt, packet_payload_type ppt = ppt_UNKNOWN);
	void set_datasize(size_t size);

	virtual packet_payload_type get_payload_type() const;
	void set_payload_type(packet_payload_type val) { m_ppt = val; }

	virtual uint8_t* data() const;

	virtual size_t capacity() const;

	virtual int datasize() const;

	virtual void resize_buffer(size_t size);

};


class string_buffer : public packet_buffer
{
private:
	const std::string m_content;
public:
	string_buffer(const std::string& str);
	virtual packet_transport_type get_transport_type() const;

	virtual uint8_t* data() const;

	virtual size_t capacity() const;

	virtual int datasize() const;

	const std::string get_content() const { return m_content; }

	virtual packet_payload_type get_payload_type() const;

};
typedef std::shared_ptr<string_buffer> str_buf_ptr;

typedef function<void(const pkt_buf_ptr&, vsdk_media_sample*)> packet_buffer_sender;
typedef boost::signals2::signal<void(const pkt_buf_ptr& buf, vsdk_media_sample* sample)> packet_buffer_signal;

#endif // packet_buffer_h__
