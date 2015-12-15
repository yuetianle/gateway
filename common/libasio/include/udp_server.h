#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include "udp_session.h"


class udp_server : public enable_shared_from_this<udp_server>
{
public:
	udp_server(const string& ip, int port, int concurrency_num = 1);
	udp_server(int port, int concurrency_num = 1);
	virtual ~udp_server();

	virtual int init()
	{
		_init_threads();
		boost::system::error_code ec;
		m_socket->open(m_local_ep.protocol(), ec);
		if (ec){ return ec.value(); }
		m_socket->set_option(boost::asio::ip::udp::socket::reuse_address(true), ec);
		if (ec){ return ec.value(); }
		m_socket->bind(m_local_ep, ec);
		if (!ec)
		{
			_accept_one();
		}
		return ec.value();
	}
	virtual void cleanup()
	{
		boost::system::error_code ec;
		m_socket->cancel(ec);
		m_socket->close(ec);
	}
	virtual void send_sync(const string& msg, const string& remoteIP, int remotePort)
	{
		boost::system::error_code ec;
		boost::asio::ip::udp::endpoint ep(boost::asio::ip::address_v4::from_string(remoteIP), remotePort);
		m_socket->send_to(boost::asio::buffer(msg.data(), msg.size()), ep, 0, ec);
	}

	int send_async(const string& msg, const string& remoteIP, int remotePort)
	{
		auto ep = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(remoteIP), remotePort);
		int segment_size = 65200;
		auto sf = shared_from_this();
		if (msg.size() < segment_size)
		{
			boost::asio::spawn(m_strand, [sf, this, msg, ep](boost::asio::yield_context yield)
			{
				boost::system::error_code ec;
				m_socket->async_send_to(boost::asio::buffer(msg.data(), msg.size()), ep, yield[ec]);
			});
		}
		else
		{
			boost::asio::spawn(m_strand, [sf, this, msg, ep, segment_size](boost::asio::yield_context yield)
			{
				boost_error_code ec;
				int msg_len = msg.size();
				for (int i = 0; i < msg_len; i += segment_size)
				{
					int remain_size = msg_len - i;
					int sent_size = remain_size>segment_size ? segment_size : remain_size;
					m_socket->async_send_to(boost::asio::buffer(msg.data() + i, sent_size), ep, yield[ec]);
					if (ec)
					{
						break;
					}
				}
			});
		}
		return v_OK;
	}

	int send_async(uint8_t* data, int size, const string& remoteIP, int remotePort)
	{
		auto ep = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(remoteIP), remotePort);
		int segment_size = 65200;
		auto sf = shared_from_this();
		if (size < segment_size)
		{
			boost::asio::spawn(m_strand, [sf, this, data, size, ep](boost::asio::yield_context yield)
			{
				boost::system::error_code ec;
				m_socket->async_send_to(boost::asio::buffer(data, size), ep, yield[ec]);
			});
		}
		else
		{
			boost::asio::spawn(m_strand, [sf, this, data, size, ep, segment_size](boost::asio::yield_context yield)
			{
				boost_error_code ec;
				int msg_len = size;
				for (int i = 0; i < msg_len; i += segment_size)
				{
					int remain_size = msg_len - i;
					int sent_size = remain_size>segment_size ? segment_size : remain_size;
					m_socket->async_send_to(boost::asio::buffer(data + i, sent_size), ep, yield[ec]);
					if (ec)
					{
						break;
					}
				}
			});
		}
		return v_OK;
	}
protected:
	void _accept_one()
	{
		auto sft = shared_from_this();
		boost::asio::spawn(m_strand, [sft, this](boost::asio::yield_context yield)
		{
			boost_error_code ec;
			boost::asio::ip::udp::endpoint sender_ep;
			size_t sz = 0;
			for (;;)
			{
				sz = m_socket->async_receive_from(boost::asio::buffer(m_io_buffer.data(), m_io_buffer.size()), sender_ep, yield[ec]);
				if (ec)
				{
					break;
				}
				string msg((char*)m_io_buffer.data(), sz);
				string remote_addr = sender_ep.address().to_string();
				if (remote_addr != "0.0.0.0" && msg != "")
				{
					udp_session::ptr udp_chl = 0;
					if (m_channel_cache.find(sender_ep, udp_chl))
					{
						std::async(launch::async, bind(&udp_session::_deliver_data, udp_chl, msg, ec));
					}
					else
					{
						if (!ec)
						{
							udp_chl.reset(new udp_session(m_socket, sender_ep));
							m_channel_cache.insert(sender_ep, udp_chl);
							std::async(launch::async, bind(&udp_session::_deliver_data, udp_chl, msg, ec));
						}
					}
				}
			}
		});
	}
protected:
	virtual void _init_threads();
	virtual void _clean_threads();


protected:
	data_cache_ex<boost::asio::ip::udp::endpoint, udp_session::ptr> m_channel_cache;

	shared_ptr<boost::asio::io_service> m_service;
	shared_ptr<boost::asio::io_service::work> m_worker;
	shared_ptr<boost::asio::ip::udp::socket> m_socket;
	boost::asio::io_service::strand m_strand;

	boost::asio::ip::udp::endpoint m_local_ep;
	int m_concurrency_num;
	std::vector<std::thread> _thread_array;

	vector<uint8_t> m_io_buffer;
};


#endif//UDP_SERVER_H