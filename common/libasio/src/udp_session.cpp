#include "pch.h"
#include "../include/udp_session.h"


shared_ptr<boost::asio::io_service> udp_session::_s_shared_service(new boost::asio::io_service);

void udp_session::s_init_thread_pool(int threadpool_size)
{
	static std::once_flag oc;
	std::call_once(oc, [&]()
	{
		auto svc = _s_shared_service;
		int size = threadpool_size <= 0 ? std::thread::hardware_concurrency() : threadpool_size;
		for (int i = 0; i < size; ++i)
		{
			thread trd([svc]()
			{
				boost::system::error_code ec;
				boost::scoped_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(*svc));
				svc->run(ec);
				cout << "udp_session thread terminated. " << endl;
			});
			trd.detach();
		}
	});
}

void udp_session::s_clean_thread_pool()
{
	static std::once_flag oc;
	std::call_once(oc, [&]()
	{
		_s_shared_service->stop();
	});
}

udp_session::udp_session(shared_ptr<udp::socket> &skt, boost::asio::ip::udp::endpoint ep, int timeout_ms /*= 10000*/, int recv_buffer_size /*= 0x1000*/, int send_buffer_size /*= 0x1000*/)
	: m_socket(skt)
	, m_remote_ep(move(ep))
	, m_strand(m_socket->get_io_service())
	, m_timeout_ms(timeout_ms)
	, m_recv_buff_size(recv_buffer_size)
	, m_send_buff_size(send_buffer_size)
	, m_local_port(0)
{

}

udp_session::~udp_session()
{
}



shared_ptr<boost::asio::io_service> udp_client::_s_shared_service(new boost::asio::io_service);

void udp_client::s_init_thread_pool(int threadpool_size)
{
	static std::once_flag oc;
	std::call_once(oc, [&]()
	{
		auto svc = _s_shared_service;
		int size = threadpool_size <= 0 ? std::thread::hardware_concurrency() : threadpool_size;
		for (int i = 0; i < size; ++i)
		{
			thread trd([svc]()
			{
				boost::system::error_code ec;
				boost::scoped_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(*svc));
				svc->run(ec);
				cout << "udp_client thread terminated. " << endl;
			});
			trd.detach();
		}
	});
}

void udp_client::s_clean_thread_pool()
{
	static std::once_flag oc;
	std::call_once(oc, [&]()
	{
		_s_shared_service->stop();
	});
}

udp_client::udp_client(int local_port /*= 0*/, bool bUseSharedService /*= true*/, int timeout_ms /*= 10000*/, int recv_buffer_size /*= 0x80000*/, int send_buffer_size /*= 0x80000*/)
	: m_use_shared_service(bUseSharedService)
	, m_client_service(bUseSharedService ? _s_shared_service : make_shared<boost::asio::io_service>())
	, m_socket(*m_client_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), local_port))
	, m_strand(m_socket.get_io_service())
	, m_timeout_ms(timeout_ms)
	, m_recv_buff_size(recv_buffer_size)
	, m_send_buff_size(send_buffer_size)
{
	m_client_work = make_shared<boost::asio::io_service::work>(*m_client_service);
	if (m_use_shared_service)
	{
		s_init_thread_pool(0);
	}
	else
	{
		auto svc = m_client_service;
		thread trd([svc]()
		{
			boost_error_code ec;
			svc->run(ec);
#ifdef _DEBUG
			cout << "udp_client::m_own_service has been terminated" << endl;
#endif
		});
		trd.detach();
	}
	m_local_port = m_socket.local_endpoint().port();
	resize_recv_buffer(m_recv_buff_size);
	resize_send_buffer(m_send_buff_size);
}

bool udp_client::resize_recv_buffer(size_t newsize)
{
	boost::system::error_code ec;
	boost::asio::ip::udp::socket::receive_buffer_size rbz(0);
	m_socket.get_option(rbz, ec);
	if (!ec)
	{
#ifdef _DEBUG
		cout << "Default UDP Receive Buffer = " << rbz.value() << endl;
#endif
		m_socket.set_option(boost::asio::ip::udp::socket::receive_buffer_size(newsize), ec);
		if (!ec)
		{
			m_socket.get_option(rbz, ec);
#ifdef _DEBUG
			cout << "resize UDP Receive Buffer OK. Buffer Size = " << rbz.value() << endl;
#endif
			return true;
		}
	}
#ifdef _DEBUG
	cout << "Failed to Set UDP RECV Buffer Size. Error: " << ec.message() << endl;
#endif
	return false;
}

bool udp_client::resize_send_buffer(size_t newsize)
{
	boost::system::error_code ec;
	boost::asio::ip::udp::socket::send_buffer_size sbz(0);
	m_socket.get_option(sbz, ec);
	if (!ec)
	{
#ifdef _DEBUG
		cout << "Default UDP Send Buffer = " << sbz.value() << endl;
#endif
		m_socket.set_option(boost::asio::ip::udp::socket::send_buffer_size(newsize), ec);
		if (!ec)
		{
			m_socket.get_option(sbz, ec);
#ifdef _DEBUG
			cout << "resize UDP Send Buffer OK. Buffer Size = " << sbz.value() << endl;
#endif
			return true;
		}
	}
#ifdef _DEBUG
	cout << "Failed to Set UDP Send Buffer Size. Error: " << ec.message() << endl;
#endif
	return false;
}
