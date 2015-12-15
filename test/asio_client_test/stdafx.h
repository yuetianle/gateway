// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lockfree/stack.hpp>
#include "uuid_util.h"
#include "vistek_error_code.h"
#include "vistek_lock_defs.h"
#include <filesystem>
// TODO: reference additional headers your program requires here
