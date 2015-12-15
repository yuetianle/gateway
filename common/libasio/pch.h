// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define DYNAMIC_LIB_NAME "libasio"

#include "pre_std_basic.h"
#include "pre_boost_basic.h"

#include "pre_asio.h"
#include "pre_lockfree.h"
#include "pre_regex.h"
#include "vistek_common.h"
#include "vistek_error_code.h"
#include "vistek_lock_defs.h"
#include "vistek_vod_defs.h"
#include "vistek_media_defs.h"
#include "vistek_instance_counter.h"
#include "data_cache_ex.h"

#ifndef __LOG_SOURCE__
#define __LOG_SOURCE__ DYNAMIC_LIB_NAME
#endif
#include "vistek_logger.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#endif
#pragma warning(disable : 4996)




// TODO: reference additional headers your program requires here
#include <boost/asio/spawn.hpp>