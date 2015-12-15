#include "stdafx.h"
#include "device_host.h"

const string host_lists_file_name = "host_lists.xml";
int main(int argc, char *argv[])
{
	if (2 != argc)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	string manufacture = argv[1];
	shared_ptr<device_host>	 host(new device_host(manufacture));
	for (;;)
	{
		if (!host->is_start())
		{
			if (v_OK != host->start())
			{
				vvlog_w("host start fail");
			}
			else
			{
				//device_host::write_host_lists(host_lists_file_name);
				vvlog_i("host start success");
				//FILE *file = nullptr;
				//file = fopen(host_lists_file_name.c_str(), "w+");
			}
		}
		this_thread::sleep_for(chrono::microseconds(1));
	}
}