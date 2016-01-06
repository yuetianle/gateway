// stdafx.cpp : source file that includes just the standard includes
// device_host.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
#ifdef _WIN32
#pragma comment(lib,  "vistek_util.lib")
#pragma comment(lib,  "libasio.lib")
#pragma comment(lib,  "vistek_logger.lib")
//#pragma comment(lib,  "device_plugins_loader.lib")
#pragma comment (lib, "python27.lib")
#endif // _WIN32

