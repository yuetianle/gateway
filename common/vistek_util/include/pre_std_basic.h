
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>
#include <functional>
#include <ctime>
#include <chrono>
#include <fstream>
#include <exception>
#include <list>
#include <stack>
#include <map>
#include <queue>
#include <vector>
#include <set>
#include <tuple>
#ifdef __arch_x86
#undef _GLIBCXX_ATOMIC_BUILTINS
#undef _GLIBCXX_USE_INT128
#endif // __arch_x86
#include <atomic>
#include <algorithm>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers