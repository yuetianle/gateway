// device_mesg_test.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "tcp_buffer.h"
using namespace std;
boost::asio::io_service svc;
string g_msg("abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.");
typedef boost::asio::ip::tcp::socket tcp_sock;
typedef shared_ptr<boost::asio::ip::tcp::socket> tcp_sock_ptr;

int send_message(int num)
{
	boost::system::error_code ec;
	//boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), 30000);
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string("172.16.0.10"), 30000);
	//boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string("172.0.0.1"), 30000);
	boost::asio::ip::tcp::socket tcp(svc);
	svc.run();
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

int test_multi_client_send(int count = 100)
{
	for (int i = 0; i < count; i++)
	{
		async(launch::async, bind(&send_message, i));
	}
	return v_OK;
}

int main(int argc, char* argv[])
{
	test_multi_client_send(1);
	cin.get();
	return 0;
}


