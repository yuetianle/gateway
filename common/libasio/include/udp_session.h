#ifndef UDP_SESSION_H
#define UDP_SESSION_H

#include "libasio_exports.h"

using namespace std;
using boost::asio::ip::udp;

class LIBASIO_API udp_session
{
	static shared_ptr<boost::asio::io_service> _s_shared_service;
public:
	static void s_init_thread_pool(int threadpool_size);
	static void s_clean_thread_pool();
public:
	typedef shared_ptr<udp_session> ptr;
	///

	udp_session(shared_ptr<udp::socket> &skt, boost::asio::ip::udp::endpoint ep, int timeout_ms = 10000, int recv_buffer_size = 0x1000, int send_buffer_size = 0x1000);
	virtual ~udp_session();

	shared_ptr<udp::socket> socket() { return m_socket; }
	boost::asio::io_service::strand &strand() { return m_strand; }

	boost::asio::ip::udp::endpoint& get_remote_ep()
	{
		return m_remote_ep;
	}
	
	bool resize_send_buffer(size_t newsize)
	{
		boost::system::error_code ec;
		boost::asio::ip::udp::socket::send_buffer_size sbz(0);
		m_socket->get_option(sbz, ec);
		if (!ec)
		{
			//cout << "Default UDP Send Buffer = " << sbz.value() << endl;
			m_socket->set_option(boost::asio::ip::udp::socket::send_buffer_size(newsize), ec);
			if (!ec)
			{
				m_socket->get_option(sbz, ec);
				//cout << "resize UDP Send Buffer OK. Buffer Size = " << sbz.value() << endl;
				return true;
			}
		}
		//cout << "Failed to Set UDP Send Buffer Size. Error: " << ec.message() << endl;
		return false;
	}

	bool resize_recv_buffer(size_t newsize)
	{
		boost::system::error_code ec;
		boost::asio::ip::udp::socket::receive_buffer_size rbz(0);
		m_socket->get_option(rbz, ec);
		if (!ec)
		{
			//cout << "Default UDP Receive Buffer = " << rbz.value() << endl;
			m_socket->set_option(boost::asio::ip::udp::socket::receive_buffer_size(newsize), ec);
			if (!ec)
			{
				m_socket->get_option(rbz, ec);
				//cout << "resize UDP Receive Buffer OK. Buffer Size = " << rbz.value() << endl;
				return true;
			}
		}
		//cout << "Failed to Set UDP RECV Buffer Size. Error: " << ec.message() << endl;
		return false;
	}
protected:
	friend class udp_server;
	void _deliver_data(const string& msg, const boost::system::error_code& error)
	{
		//m_event_message(msg, error.value());
	}
protected:

	shared_ptr<udp::socket> m_socket;
	udp::endpoint m_remote_ep;
	boost::asio::io_service::strand m_strand;
	int m_timeout_ms;
	size_t m_recv_buff_size;
	size_t m_send_buff_size;

	int m_local_port;

};



class LIBASIO_API udp_client
{
	static shared_ptr<boost::asio::io_service> _s_shared_service;
public:
	static void s_init_thread_pool(int threadpool_size);
	static void s_clean_thread_pool();
public:
	typedef shared_ptr<udp_client> ptr;
	udp_client(int local_port = 0, bool bUseSharedService = true, int timeout_ms = 10000, int recv_buffer_size = 0x80000, int send_buffer_size = 0x80000);

	virtual ~udp_client()
	{
	}

	udp::socket& socket() { return m_socket; }
	boost::asio::io_service::strand& strand() { return m_strand; }

	bool resize_send_buffer(size_t newsize);

	bool resize_recv_buffer(size_t newsize);

	void cleanup()
	{
		if (m_socket.is_open())
		{
			m_socket.cancel();
			m_socket.close();
		}
	}
protected:
	bool m_use_shared_service;
	shared_ptr<boost::asio::io_service> m_client_service;
	shared_ptr<boost::asio::io_service::work> m_client_work;
	udp::socket m_socket;
	udp::endpoint m_remote_ep;
	boost::asio::io_service::strand m_strand;
	int m_timeout_ms;
	size_t m_recv_buff_size;
	size_t m_send_buff_size;

	int m_local_port;
};

#endif//UDP_SESSION_H