// asio_client_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tcp_buffer.h"
#include "file_sink.h"
#include "md5_sink.h"
using namespace std;
boost::asio::io_service svc;
string g_msg("abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.");
typedef boost::asio::ip::tcp::socket tcp_sock;
typedef shared_ptr<boost::asio::ip::tcp::socket> tcp_sock_ptr;
int proc_asio(boost::asio::io_service& svc, int num)
{
	auto id = this_thread::get_id();
	boost::asio::io_service::work w(svc);

	cout << "asio #" << num << " started..." << " trd id = " << id << endl;
	svc.run();
	cout << "asio #" << num << " stopped..." << endl;
	return 0;
}

int send_message(int num)
{
	boost::system::error_code ec;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string("172.20.10.133", ec), 9977);
	boost::asio::ip::tcp::socket tcp(svc);
	tcp.connect(ep, ec);
	if (!ec)
	{
		///cout << "id: " << num << " connected..." << endl;
		tcp.send(boost::asio::buffer((uint8_t*)g_msg.data(), g_msg.size()), 0, ec);
		if (!ec)
		{
			cout << "id: " << num << " message has been sent ..." << endl;
		}
		else
		{
			cout << "id: " << num << " failed to send message; error" << ec.message() << endl;
		}
	}
	else
	{
		cout << "id: " << num << " failed to connect; error: " << ec.message() << endl;
	}
	return (int)ec.value();
}
shared_ptr<tcp_buffer_allocator> tcp_mgr(new tcp_buffer_allocator(16384, 4096));

void handle_read(const boost::system::error_code& error, size_t bytes_transferred, const tcp_buf_ptr& buffer, const tcp_sock_ptr& tcp, const base_sink_ptr& writer)
{

	if (!error)
	{

		buffer->set_datasize(bytes_transferred);
		writer->input(buffer);
		tcp_buf_ptr newbuf = 0;
		do
		{
			newbuf = tcp_mgr->get_buffer();
			if (newbuf != nullptr)
			{
				tcp->async_receive(boost::asio::buffer(newbuf->data(), newbuf->capacity()), boost::bind(&handle_read, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, newbuf, tcp, writer));
				break;
			}
			else
			{
				this_thread::sleep_for(chrono::milliseconds(40));
				cout << "!";
			}

		} while (true);
	}
	else
	{
		cout << "ERROR: " << error.message() << "; session will be closed!!!!!!; error_code" << error.value() << endl;
		writer->close();
	}
}

int test_multi_client_send(int count = 100)
{
	for (int i = 0; i < count; i++)
	{
		async(launch::async, bind(&send_message, i));
	}
	return v_OK;
}
void recv_file()
{
	boost::system::error_code ec;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string("172.20.10.133", ec), 9977);
	tcp_sock_ptr tcp(new tcp_sock(svc));
	UUIDEntity id;
	base_sink_ptr writer(new md5_sink());
	writer->open();
	tcp->connect(ep, ec);
	if (!ec)
	{
		tcp_buf_ptr buf = tcp_mgr->get_buffer();
		tcp->async_read_some(boost::asio::buffer(buf->data(), buf->capacity()), boost::bind(&handle_read, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, buf, tcp, writer));
	}
}
int main(int argc, char* argv[])
{
	for (int i = 0; i < 4; i++)
	{
		async(launch::async, bind(&proc_asio, std::ref(svc), i));
	}
	for (int i = 0; i < 4; i++)
	{
		async(launch::async, bind(&recv_file));
	}
	while (cin.get() != '\n')
	{
		continue;
	}
	svc.stop();
	return 0;
}

