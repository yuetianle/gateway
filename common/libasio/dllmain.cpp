// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "tcp_session.h"
#ifdef _WIN32

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//async(launch::async, &tcp_session::s_init_thread_pool, 16);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//tcp_session::s_clean_thread_pool();
		break;
	}
	return TRUE;
}
#endif
