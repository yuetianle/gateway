// pch.cpp : source file that includes just the standard includes
// libasio.pch will be the pre-compiled header
// pch.obj will contain the pre-compiled type information
// TODO: reference any additional headers you need in pch.h and not in this file
#include "pch.h"
#ifdef _WIN32
#pragma comment(lib, "vistek_util.lib")
#pragma comment(lib, "vistek_logger.lib")
#endif