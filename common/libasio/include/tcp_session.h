//////////////////////////////////////////////////////////////////////////
// tcp_session.h
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Created By: FJW
// Last modification: 2015/05/23
// Copyright (c) 2013-2015 Vistek 
// Usage: 
//////////////////////////////////////////////////////////////////////////
#ifndef tcp_session_h__
#define tcp_session_h__
#include "libasio_exports.h"
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/coroutine/coroutine.hpp>
using namespace std;
using boost::asio::ip::tcp;
class LIBASIO_API tcp_session : public std::enable_shared_from_this<tcp_session>
{
	static shared_ptr<boost::asio::io_service> _s_shared_service;
	
public:
	static void s_init_thread_pool(int threadpool_size);
	static void s_clean_thread_pool();
public:
	typedef shared_ptr<tcp_session> ptr;
	typedef std::promise<int> coro_promise;
	typedef std::shared_ptr<coro_promise> coro_promise_ptr;
	typedef boost::asio::steady_timer coro_timer;
	typedef std::shared_ptr<coro_timer> coro_timer_ptr;
	typedef boost::asio::handler_type<boost::asio::yield_context, void(int)>::type async_result_handler;
protected:
	bool m_is_client = false;
	bool m_use_shared_service;
	shared_ptr<boost::asio::io_service> m_client_service;
	shared_ptr<boost::asio::io_service::work> m_client_work;

	tcp::socket m_socket;
	tcp::endpoint m_remote_ep;
	coro_timer_ptr m_timer;
	boost::asio::io_service::strand m_strand;
	int m_timeout_ms;

	size_t m_recv_buff_size;
	size_t m_send_buff_size;
public:
	explicit tcp_session(bool bUseSharedService, int timeout_ms, int recv_buffer_size = 0x1000, int send_buffer_size = 0x1000);
	explicit tcp_session(const string& ip, int port, bool bUseSharedService, int timeout_ms, int recv_buffer_size = 0x1000, int send_buffer_size = 0x1000);
	explicit tcp_session(tcp::socket sock, int timeout_ms, int recv_buffer_size = 0x1000, int send_buffer_size = 0x1000);
	virtual ~tcp_session();

	tcp::socket& socket() { return m_socket; }
	boost::asio::io_service::strand& strand() { return m_strand; }
protected:
	typedef function<void(const coro_promise_ptr& prom, const coro_timer_ptr& ptimer, boost::asio::yield_context)> coro_action;
	int _run_sync_action(coro_action operation_action, int timeout_ms)
	{
		auto self = shared_from_this();
		auto prom = make_shared<coro_promise>();
		coro_timer_ptr ptimer(new coro_timer(*_s_shared_service));
		boost_error_code ec;
		ptimer->expires_from_now(chrono::milliseconds(timeout_ms), ec);
		boost::asio::spawn(m_strand, [this,self, prom, ptimer](boost::asio::yield_context yield)
		{
			boost::system::error_code ec;
			ptimer->async_wait(yield[ec]);
			if (ec)
			{
				return;
			}
			else if (ptimer->expires_from_now() <= std::chrono::milliseconds(0))
			{
				try
				{
					if (prom != nullptr)
					{
						//if (prom->get_future().valid())
						{
							prom->set_value(v_ERR_TIMEOUT);
						}
					}
				}
				catch (std::future_error & fe)
				{
					cout << fe.what() << endl;
				}
				catch (exception& e)
				{
					cout << e.what() << endl;
				}
			}
		});
		
		boost::asio::spawn(m_strand, std::bind(operation_action, prom, ptimer, std::placeholders::_1));

		int result = prom->get_future().get();
		return result;
	}
	void _start_async();
	void _stop_async();
	virtual void _spawn_handle_timeout(const coro_timer_ptr& ptimer, const coro_promise_ptr& prom);
	virtual void _close()
	{
		boost_error_code ec;
		if (m_socket.is_open())
		{
			m_socket.close(ec);
		}
		if (m_timer)
		{
			m_timer->cancel(ec);
		}
	}
public:
	virtual int connect_y(const string& content, boost::asio::yield_context yield);
	virtual int connect();
	virtual int connect_ex(const string& content);

	virtual void spawn_timeout(int expired_ms)
	{
		reset_timer(expired_ms);
		_spawn_handle_timeout(m_timer, nullptr);
	}
	virtual int reset_timer(int expired_ms)
	{
		if (m_timer)
		{
			m_timer->expires_from_now(chrono::milliseconds(expired_ms));
		}
		return v_OK;
	}
	bool set_no_delay(bool val)
	{
		boost::system::error_code ec;
		m_socket.set_option(boost::asio::ip::tcp::no_delay(val), ec);
		if (!ec)
		{
			//cout << "Set Socket No Delay To " << val << " OK" << endl;
			return true;
		}
		return false;
	}

	bool resize_send_buffer(size_t newsize)
	{
		boost::system::error_code ec;
		boost::asio::ip::tcp::socket::send_buffer_size sbz(0);
		m_socket.get_option(sbz, ec);
		if (!ec)
		{
			//cout << "Default TCP Send Buffer = " << sbz.value() << endl;
			m_socket.set_option(boost::asio::ip::tcp::socket::send_buffer_size(newsize), ec);
			if (!ec)
			{
				m_socket.get_option(sbz, ec);
				//cout << "resize TCP Send Buffer OK. Buffer Size = " << sbz.value() << endl;
				return true;
			}
		}
		//cout << "Failed to Set TCP Send Buffer Size. Error: " << ec.message() << endl;
		return false;
	}

	bool resize_recv_buffer(size_t newsize)
	{
		boost::system::error_code ec;
		boost::asio::ip::tcp::socket::receive_buffer_size rbz(0);
		m_socket.get_option(rbz, ec);
		if (!ec)
		{
			//cout << "Default TCP Receive Buffer = " << rbz.value() << endl;
			m_socket.set_option(boost::asio::ip::tcp::socket::receive_buffer_size(newsize), ec);
			if (!ec)
			{
				m_socket.get_option(rbz, ec);
				//cout << "resize TCP Receive Buffer OK. Buffer Size = " << rbz.value() << endl;
				return true;
			}
		}
		//cout << "Failed to Set TCP RECV Buffer Size. Error: " << ec.message() << endl;
		return false;
	}
};
#endif // tcp_session_h__
