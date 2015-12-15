#ifndef DS_TCP_SESSION__H_
#define DS_TCP_SESSION__H_

#include "tcp_session.h"
#include "libasio_exports.h"

using boost::asio::ip::tcp;
using namespace std;

typedef function<void(const string&, int)> ds_session_event_handler;

class LIBASIO_API ds_tcp_session : public tcp_session
{
public:
	explicit ds_tcp_session(const string& ip, int port, int timeout_ms = 10000, bool bUseSharedService = true, int recv_buffer_size = 0x2000, int send_buffer_size = 0x2000);
	explicit ds_tcp_session(tcp::socket sock, int timeout_ms = 10000, int recv_buffer_size = 0x2000, int send_buffer_size = 0x2000);
	virtual ~ds_tcp_session();
	int send_pkt(const string& data);
	virtual int wait_request();
	virtual int cleanup()
	{
		_close();
		return v_OK;
	}
	void reset_msg_delimiter(const string& delimiter)
	{
		m_msg_delimiter = delimiter;
	}
	void subscribe_event_handler(ds_session_event_handler&& handler)
	{
		m_event_handler = handler;
	}
	int reset_timer()
	{
		tcp_session::reset_timer(m_timeout_ms);
		return v_OK;
	}
	int keep_alive()
	{
		/*while(m_socket.is_open())
		{
			string keep_msg = "<!--msg_end-->";
			send_pkt(keep_msg);
			this_thread::sleep_for(chrono::seconds(5));
		}*/
		boost::asio::ip::tcp::socket::keep_alive ka(true);
		m_socket.set_option(ka);
		return v_OK;
	}
protected:
	ds_session_event_handler m_event_handler;
	string m_msg_delimiter = "<!--msg_end-->";
};

typedef shared_ptr<ds_tcp_session> ds_tcp_session_ptr;

#endif//DS_TCP_SESSION__H_