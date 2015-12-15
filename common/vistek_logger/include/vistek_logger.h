#ifndef __VISTEK_LOGGER_H__
#define __VISTEK_LOGGER_H__
#include <mutex>
#include <string>
#include "log_color.h"
#include "vistek_lock_defs.h"
#include "boost/asio.hpp"
#include "boost/date_time.hpp"
#include "data_cache_ex.h"
#include <thread>
#include <condition_variable>
#include "data_cache.h"
#define V_CHECK_RESULT(ret, msg) if( (ret) != v_OK ) { vlog_ee((msg)); return ret; }
#define V_CHECK_RESULT_EC(ret, msg) if( (ret) != v_OK ) { std::stringstream ss; ss<<msg<<"; ec = "<< ret; vlog_ee(ss.str()); return ret; }
#define V_CHECK_RESULT_THROW(ret, msg) if( (ret) != v_OK ) { throw std::exception((msg)); }
//extern bool g_vistek_verbose_log;
enum vistek_log_type
{
	v_LOG_ERR,
	v_LOG_WRN,
	v_LOG_INFO, 
	v_LOG_TEXT,
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
using namespace std;
class VISTEK_LOGGER_API vistek_logger : public enable_shared_from_this<vistek_logger>
{
public:
	typedef tuple<string> log_key;

	typedef shared_ptr<vistek_logger> ptr;
	static bool s_vistek_verbose_log;
	static shared_ptr<boost::asio::io_service> s_log_service;
	static shared_ptr<boost::asio::io_service::work> s_log_worker;
	static shared_ptr<boost::asio::io_service> s_print_service;
	static shared_ptr<boost::asio::io_service::work> s_print_worker;

	//static int s_init_flag;
	//static mutex s_init_mtx;
	static data_cache<log_key, ptr> s_log_cache;
	static void _try_init_(int concurency);
private:
	boost::asio::io_service::strand m_strand_log;
	boost::asio::io_service::strand m_strand_print;
	int64_t m_write_ts = 0;
	std::string m_source_name;
	std::stringstream m_test_stream;

	std::stringstream& text_stream() { return m_test_stream; }
public:

	static ptr get_instance(const log_key& key);
	vistek_logger(const std::string& source_name)
		: m_source_name(source_name), m_strand_log(*s_log_service), m_strand_print(*s_print_service)
	{
		_try_init_(4);
	}
private:
	static vistek_log_type str_to_vistek_log_types(const std::string& str);
	static const char* vistek_log_types_to_str(vistek_log_type os);
	
	static void _clean_log_by_time_();
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
private:

	void _print(vistek_log_type lt, const std::string& content);
	void _print_ex(vistek_log_type lt, const std::string& content);

	void _write(const std::string& source, std::string datestr, std::string output_str);
	void _write_ex(const std::string& source, std::string datestr, std::string output_str);


public:
	void process_log(bool bWrite, vistek_log_type lt, string content);
	template<typename T, typename... Args>
	void print_args(vistek_log_type lt, T&& val, Args&&... args)
	{
        std::stringstream ss;
		append(ss, val, std::forward<Args>(args)...);
		process_log(false, lt, move(ss.str()));
	}
	std::stringstream& append_log_head(std::stringstream& ss, vistek_log_type lt)
	{
		boost::posix_time::time_facet* tf = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%s");
		ss.imbue(std::locale(std::cout.getloc(), tf));

		boost::posix_time::ptime pt = boost::posix_time::microsec_clock::local_time();
		ss << "[" << vistek_log_types_to_str(lt) << "]" << pt << ": ";
		return ss;
	}
	std::stringstream& append_log_trace(std::stringstream& ss, const std::string& filename, int line, const std::string& funcName)
	{
		ss << filename << "(" << line << "): " << funcName << ": ";
		return ss;
	}
	void input(vistek_log_type lt, const std::string& filename, int line, const std::string& funcName, const std::string& source, bool bWrite, const std::string& content);
	template<typename T, typename... Args>
	void input_args(vistek_log_type lt, const std::string& filename, int line, const std::string& funcName, const std::string& source, bool bWrite, T&& val, Args... args)
	{
		if (lt == v_LOG_VERBOSE)
		{
			if (!s_vistek_verbose_log)
			{
				return;
			}
		}
        std::stringstream ss;
		append_log_head(ss, lt);
		ss << filename << "(" << line << "): " << funcName << ": ";
		append(ss, val, std::forward<Args>(args)...);
		ss << endl;
		process_log(bWrite, lt, move(ss.str()));
	}

	//static void write_log(const std::string& source, const std::string& content, bool bWrite);
	template<typename T, typename... Args>
	void write_args(vistek_log_type lt, const std::string& source, bool bWrite, T&& val, Args... args)
	{
		
		std::stringstream ss;
		append_log_head(ss, lt);
		append(ss, val, std::forward<Args>(args)...);
		ss << endl;
		process_log(bWrite, lt, move(ss.str()));
	}
	template<typename T, typename... Args>
	static void write_log_args(vistek_log_type lt, const std::string& source, bool bWrite, T&& val, Args... args)
	{
		auto logger = get_instance(log_key(source));
		if (logger != nullptr)
		{
			logger->write_args(lt, source, bWrite, val, std::forward<Args>(args)...);
		}
	}
};
extern "C" VISTEK_LOGGER_API void vlog_set_verbose(bool flag);
//typedef vistek_logger vlogger;
#define vlog_ii(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->input_args(v_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, true, __VA_ARGS__);
#define vlog_ww(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->input_args(v_LOG_WRN, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, true, __VA_ARGS__);
#define vlog_ee(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->input_args(v_LOG_ERR, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, true, __VA_ARGS__);
#define vlog_vv(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->input_args(v_LOG_VERBOSE, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, true, __VA_ARGS__);
#define vlog_ss(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->write_args(v_LOG_INFO, __LOG_SOURCE__, true, __VA_ARGS__);

#define vshow_ii(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->input_args(v_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, false, __VA_ARGS__);
#define vshow_ww(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->input_args(v_LOG_WRN, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, false, __VA_ARGS__);
#define vshow_ee(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->input_args(v_LOG_ERR, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, false, __VA_ARGS__);
#define vshow_vv(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->input_args(v_LOG_VERBOSE, __FILE__, __LINE__, __FUNCTION__, __LOG_SOURCE__, false, __VA_ARGS__);
#define vshow_ss(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->write_args(v_LOG_INFO, __LOG_SOURCE__, false, __VA_ARGS__);

#define vprint_ii(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->print_args(v_LOG_INFO, __VA_ARGS__);
#define vprint_ww(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->print_args(v_LOG_WRN, __VA_ARGS__);
#define vprint_ee(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->print_args(v_LOG_ERR, __VA_ARGS__);
#define vprint_vv(...) vistek_logger::get_instance(vistek_logger::log_key(__LOG_SOURCE__))->print_args(v_LOG_VERBOSE1, __VA_ARGS__);

#define vlog_trace(_src_, lt, bShowHead, bTrace, bWrite, _appendstream) { const vistek_logger::ptr& _temp_1_ = vistek_logger::get_instance(vistek_logger::log_key(_src_));\
									if(_temp_1_) {	\
									std::stringstream _temp_2_;\
									if(bShowHead) _temp_1_->append_log_head(_temp_2_, lt); \
									if(bTrace) _temp_1_->append_log_trace(_temp_2_, __FILE__, __LINE__, __FUNCTION__);\
									_temp_2_ << _appendstream << endl;\
									_temp_1_->process_log(bWrite, lt, move(_temp_2_.str()));}}


#define vvlog_i(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_INFO, true, true, true, _appendstream);
#define vvlog_w(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_WRN, true, true, true, _appendstream);
#define vvlog_e(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_ERR, true, true, true, _appendstream);
#define vvlog_v(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_VERBOSE, true, true, true, _appendstream);
#define vvlog_s(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_TEXT, false, false, true, _appendstream);

#define vvshow_i(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_INFO, true, true, false, _appendstream);
#define vvshow_w(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_WRN, true, true, false, _appendstream);
#define vvshow_e(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_ERR, true, true, false, _appendstream);
#define vvshow_v(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_VERBOSE, true, true, false, _appendstream);
#define vvshow_s(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_TEXT, true, true, false, _appendstream);

#define vvprint_i(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_INFO, false, false, false, _appendstream);
#define vvprint_w(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_WRN, false, false, false, _appendstream);
#define vvprint_e(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_ERR, false, false, false, _appendstream);
#define vvprint_v(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_VERBOSE, false, false, false, _appendstream);
#define vvprint_s(_appendstream) vlog_trace(__LOG_SOURCE__, v_LOG_TEXT, false, false, false, _appendstream);

//#define vLOG_ERR(source, text) vistek_logger::write(v_LOG_ERR, __FILE__, __LINE__, __FUNCTION__, (source), (text));
//#define vLOG_WRN(source, text) vistek_logger::write(v_LOG_WRN, __FILE__, __LINE__, __FUNCTION__, (source), (text));
//#define vLOG_INFO(source, text) vistek_logger::write_log((source), (text));
#endif