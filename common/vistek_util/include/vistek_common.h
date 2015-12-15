#ifndef __VISTEK_COMMON_H__
#define __VISTEK_COMMON_H__

//#ifndef MEMORY_LEAK_DETECT
//#define MEMORY_LEAK_DETECT
//#endif
#ifdef MEMORY_LEAK_DETECT
#define _CRTDBG_MAP_ALLOC   //MemoryLeak Detection
#include <stdlib.h>
#include <crtdbg.h>
#endif
#ifdef WIN32

#else
#ifdef __linux
#define _atoi64(val) strtoll(val, NULL, 10)
#endif
#endif
#if _MSC_VER>=1100
#define VISTEK_NOVTABLE __declspec(novtable)
#else
#define VISTEK_NOVTABLE
#endif

#define WIN32_LEAN_AND_MEAN

#endif