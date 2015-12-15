// meg_queue_test.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "vistek_process.h"
#include <boost\interprocess\ipc\message_queue.hpp>
#include <boost\interprocess\shared_memory_object.hpp>
#include <boost\serialization\binary_object.hpp>
#include <boost\archive\text_iarchive.hpp>
#include <boost\archive\text_oarchive.hpp>
#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost\serialization\access.hpp>
#include <boost\serialization\string.hpp>
using namespace boost::interprocess;
struct MESG_CONTEXT
{
	friend class boost::serialization::access;
	char device_msg[100];
	char device_id[20];
	//string device_msg;
	//string device_id;
	int msg_level;
	MESG_CONTEXT()
	{
		memset(device_msg, 0, sizeof(device_msg));
		memset(device_id, 0, sizeof(device_id));
	}
	/*template < class Archive >
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & device_msg;
		ar & device_id;
		ar & msg_level;
	}
	*/
};
int _tmain(int argc, _TCHAR* argv[])
{

	vistek_process *proc_1 = new vistek_process("lee", "D:\\github\\gateway\\solution_vs\\__solution\\__output\\x86\\Debug\\gateway", "device_host-1d.exe", "1 \"221\" 0 \"hikipc\" 1 \"172.16.1.190\" 80 \"admin\" \"12345\"");
	proc_1->start();
	char receive_queue_name[10] = { 0 };
	const int max_message_num = 1000;
	const int max_message_content_len = 1000;
	sprintf_s((char*)receive_queue_name, sizeof(receive_queue_name), "id_%u", proc_1->process_info().dwProcessId);
	long msg_len = sizeof(MESG_CONTEXT);

	if (message_queue::remove((char*)receive_queue_name))
	{
		printf("delete queue success");
	}
	//message_queue send_queue(open_or_create, (const char*)receive_queue_name, 1000, sizeof(MESG_CONTEXT));
	message_queue send_queue(open_or_create, (const char*)receive_queue_name, 1000, sizeof(MESG_CONTEXT));
	MESG_CONTEXT msg;
	//msg.device_id = "221";
	//msg.device_msg = "http://172.16.1.190:80/device/meida?func=get_stream_url";
	sprintf_s((char*)msg.device_id, sizeof(msg.device_id),"%s", "221");
	sprintf_s((char*)msg.device_msg, sizeof(msg.device_msg),"%s", "http://172.16.1.190:80/device/meida?func=get_stream_url");
	msg.msg_level = 1;
	//ostringstream out_temp;
	//ostringstream out_temp_text;
	//boost::archive::binary_oarchive out_data(out_temp);
	////boost::archive::text_oarchive out_text_data(out_temp_text);
	//out_data << msg;
	////out_text_data << msg;
	//std::string out_save = out_temp.str();
	////std::string out_save_text = out_temp_text.str();
	int count = 0;
	while (true)
	{
		send_queue.try_send(&msg, sizeof(msg), 1);
		printf("sub proc_id:%u id:%s level:%d send %s\n", proc_1->process_info().dwProcessId, msg.device_id, msg.msg_level, msg.device_msg);
		//send_queue.try_send((char*)out_save.c_str(), out_save.length(), 1);
		//send_queue.try_send((char*)out_save_text.c_str(), out_save_text.length(), 1);
		this_thread::sleep_for(chrono::seconds(5));
	}
	return 0;
}

