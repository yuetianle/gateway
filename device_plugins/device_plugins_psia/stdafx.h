// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "pre_std_basic.h"
#include "pre_boost_basic.h"
#include "pre_asio.h"
#include "pre_regex.h"
#include "pre_lockfree.h"
#include "vistek_common.h"
#include "vistek_error_code.h"
#include "vistek_lock_defs.h"
#include "vistek_vod_defs.h"
#include "vistek_media_defs.h"
#include "vistek_record_defs.h"
#include "vistek_time_util.h"
#include "vistek_device_defs.h"
#include "vistek_thread_util.h"
#include "vistek_string_util.h"
#include "uuid_util.h"
#include "pugixml.hpp"
#include "pugiconfig.hpp"
#ifndef __LOG_SOURCE__
#define __LOG_SOURCE__ "device_gateway_psia"
#endif

#include "vistek_logger.h"
#include "base64.h"

// TODO: reference additional headers your program requires here
