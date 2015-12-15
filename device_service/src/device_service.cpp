// device_service.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "device_host_manager.h"
int main(int argc, char* argv[])
{
	device_host_manager_ptr host_manager_ptr(new device_host_manager());
	for (;;)
	{
		if (host_manager_ptr && !host_manager_ptr->is_start())
		{
			host_manager_ptr->start();
			cout << "come here" << endl;
		}
	}

	return 0;
}
