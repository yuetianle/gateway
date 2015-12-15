#ifndef DS_TCP_SERVER__H_
#define DS_TCP_SERVER__H_

#include "ds_tcp_session.h"
#include "tcp_server.h"
#include "libasio_exports.h"

using namespace std;

typedef function<void(const ds_tcp_session_ptr&, const string&)> ds_session_handler;

class LIBASIO_API ds_tcp_server : public tcp_server
{
public:
	explicit ds_tcp_server(const string& ip, int port, int timeout_ms = 10000, int concurrency_num = 1);
	explicit ds_tcp_server(int port, int timeout_ms = 10000, int concurrency_num = 1);
	virtual ~ds_tcp_server();
	virtual int start();
	virtual int stop();
	uint16_t get_listen_port();
	void subscribe_new_session(ds_session_handler && dsh)
	{
		m_session_handler = dsh;
	}
protected:
	int m_timeout_ms;
	ds_session_handler m_session_handler;
};

typedef shared_ptr<ds_tcp_server> ds_tcp_server_ptr;

#endif//DS_TCP_SERVER__H_