#ifndef __VISTEK_LOGGER_H__
#define __VISTEK_LOGGER_H__
#include <mutex>
#include <string>
#include "log_color.h"
#include "vistek_lock_defs.h"
#include "boost/date_time.hpp"
#include "data_cache_ex.h"
#include <thread>
#include <condition_variable>
#define V_CHECK_RESULT(ret, msg) if( (ret) != v_OK ) { vlog_e((msg)); return ret; }
#define V_CHECK_RESULT_EC(ret, msg) if( (ret) != v_OK ) { std::stringstream ss; ss<<msg<<"; ec = "<< ret; vlog_e(ss.str()); return ret; }
#define V_CHECK_RESULT_THROW(ret, msg) if( (ret) != v_OK ) { throw std::exception((msg)); }
//extern bool g_vistek_verbose_log;
enum vistek_log_type
{
	v_LOG_ERR,
	v_LOG_WRN,
	v_LOG_INFO, 
	v_LOG_VERBOSE,
	v_LOG_VERBOSE1,
	v_LOG_VERBOSE2,
	v_LOG_VERBOSE3,
};
#ifdef STATICLIB
#define VISTEK_LOGGER_API 
#else
#ifdef _WIN32 //windows
	#ifdef VISTEK_LOGGER_EXPORTS
#define VISTEK_LOGGER_API __declspec(dllexport)
	#else
#define VISTEK_LOGGER_API __declspec(dllimport)
	#endif
#else
#define VISTEK_LOGGER_API 
#endif
#endif
class VISTEK_LOGGER_API vistek_logger
{
	static int s_vistek_logger_init_flag;
	static mutex s_vistek_logger_init_mutex;
	static timed_mutex s_vistek_logger_proc_mtx;
	static condition_variable_any s_vistek_logger_proc_cond;
	static mutex s_vistek_log_action_queue_mutex;
	static deque<std::function<void()>> s_vistek_log_action_queue;
	static void g_vistek_logger_proc_log_action();
	static void g_vistek_logger_run_io_service();
	static thread s_trd_proc_log;
public:
	static bool _vistek_logger_init_();
	static void _vistek_logger_clean_();
private:
	static void _write(const std::string& datestr, const std::string& source, const std::string& output_str);
	template<typename T>
	static void append(std::stringstream& ss, T&& val)
	{
		ss << val;
	}
	template<typename T, typename... Args>
	static void append(std::stringstream& ss, T&& val,  Args&&... args)
	{
		append(ss, val);
		append(ss, std::forward<Args>(args)...);
	}
	static vistek_log_type str_to_vistek_log_types(const std::string& str);

	static const char* vistek_log_types_to_str(vistek_log_type os);
	static void _clean_log_by_time_( );
	static void _process_log_(bool bWrite, vistek_log_type lt, const string& source, string&& content);
public:
	//static data_cache_ex<string, shared_ptr<mutex>> g_mtx_map;
	//static vistek_smart_locker g_mtx_log;
	//static vistek_smart_locker g_mtx_log_path;
	//static vistek_smart_locker g_mtx_std_out;
	//static void write_err(const std::string& source, const std::string& content);

	//static void write(vistek_log_type lt, const std::string& source, const std::string& method, const std::string& content);

	static bool g_vistek_verbose_log;
	static void print(vistek_log_type lt, const std::string& content);
	template<typename T, typename... Args>
	static void print_args(vistek_log_type lt, T&& val, Args&&... args)
	{
        std::stringstream ss;
		append(ss, val, std::forward<Args>(args)...);
		_process_log_(false, lt, "", move(ss.str()));
	}
	static void input(vistek_log_type lt, const std::string& filename, int line, const std::string& funcName, const std::string& source, bool bWrite, const std::string& content);
	template<typename T, typename... Args>
	static void input_args(vistek_log_type lt, const std::string& filename, int line, const std::string& funcName, const std::string& source, bool bWrite, T&& val, Args... args)
	{
		if (lt == v_LOG_VERBOSE)
		{
			if (!g_vistek_verbose_log)
			{
				return;
			}
		}
		boost::posix_time::ptime pt = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_facet* tf = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%s");
        std::stringstream ss;
        
		ss.imbue(std::locale(std::cout.getloc(), tf));
		ss << "[" << vistek_log_types_to_str(lt) << "]" << pt << ": " << filename << "(" << line << "): " << funcName << ": ";
		append(ss, val, std::forward<Args>(args)...);
		_process_log_(bWrite, lt, source, move(ss.str()));
	}

	//static void write_log(const std::string& source, const std::string& content, bool bWrite);
	template<typename T, typename... Args>
	static void write_log_args(vistek_log_type lt, const std::string& source, bool bWrite, T&& val, Args... args)
	{
		boost::posix_time::ptime pt = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_facet* tf = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%s");
		std::stringstream ss;
		ss.imbue(std::locale(std::cout.getloc(), tf));
		ss << "[" << vistek_log_types_to_str(v_LOG_INFO) << "]" << pt << ": ";
		append(ss, val, std::forward<Args>(args)...);
		_process_log_(bWrite, lt, source, move(ss.str()));
	}

};
extern "C" VISTEK_LOGGER_API void vlog_set_verbose(bool flag);
//typedef vistek_logger vlogger;
#define vlog_v(...) vistek_logger::input_args(v_LOG_VERBOSE, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, true, __VA_ARGS__);
#define vlog_i(...) vistek_logger::input_args(v_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, true, __VA_ARGS__);
#define vlog_w(...) vistek_logger::input_args(v_LOG_WRN, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, true, __VA_ARGS__);
#define vlog_e(...) vistek_logger::input_args(v_LOG_ERR, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, true, __VA_ARGS__);
#define vlog_s(...) vistek_logger::write_log_args(v_LOG_INFO, __LOG_SOURCE__, true, __VA_ARGS__);

#define vshow_v(...) vistek_logger::input_args(v_LOG_VERBOSE, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, false, __VA_ARGS__);
#define vshow_i(...) vistek_logger::input_args(v_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, false, __VA_ARGS__);
#define vshow_w(...) vistek_logger::input_args(v_LOG_WRN, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, false, __VA_ARGS__);
#define vshow_e(...) vistek_logger::input_args(v_LOG_ERR, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, false, __VA_ARGS__);
#define vshow_s(...) vistek_logger::write_log_args(v_LOG_INFO, __LOG_SOURCE__, false, __VA_ARGS__);

#define vprint_i(...) vistek_logger::print_args(v_LOG_INFO, __VA_ARGS__);
#define vprint_w(...) vistek_logger::print_args(v_LOG_WRN, __VA_ARGS__);
#define vprint_e(...) vistek_logger::print_args(v_LOG_ERR, __VA_ARGS__);
#define vprint_v(...) vistek_logger::print_args(v_LOG_VERBOSE1, __VA_ARGS__);

//#define vLOG_ERR(source, text) vistek_logger::write(v_LOG_ERR, __FILE__, __LINE__, __FUNCTION__, (source), (text));
//#define vLOG_WRN(source, text) vistek_logger::write(v_LOG_WRN, __FILE__, __LINE__, __FUNCTION__, (source), (text));
//#define vLOG_INFO(source, text) vistek_logger::write_log((source), (text));
#endif