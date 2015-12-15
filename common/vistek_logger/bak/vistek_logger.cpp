#include "stdafx.h"
#include "vistek_logger.h"
#include <fstream>
#include <memory>
#include <vistek_lock_defs.h>
#include <boost/date_time.hpp>
#include "vistek_file_util.h"
#include "log_color.h"
#include <deque>
#include <functional>
#include "pre_boost_basic.h"
#include "vistek_time_util.h"
using namespace std;
using namespace boost::filesystem;

path g_log_path("log");

std::mutex vistek_logger::s_vistek_logger_init_mutex;
std::mutex vistek_logger::s_vistek_log_action_queue_mutex;
deque<std::function<void()>> vistek_logger::s_vistek_log_action_queue;
int vistek_logger::s_vistek_logger_init_flag = 0;
thread vistek_logger::s_trd_proc_log;
std::timed_mutex vistek_logger::s_vistek_logger_proc_mtx;

std::condition_variable_any vistek_logger::s_vistek_logger_proc_cond;



bool _check_log_path()
{
	boost::system::error_code ec;

	if (!boost::filesystem::exists(g_log_path, ec))
	{
		bool ret = boost::filesystem::create_directories(g_log_path, ec);
		return ret;
	}
	return true;
}
bool _g_use_io_service = true;
boost::asio::io_service _g_log_service;
shared_ptr<boost::asio::io_service::work> _g_log_worker(new boost::asio::io_service::work(_g_log_service));
condition_variable _g_log_cond;
mutex _g_log_init_mtx;
thread* _g_log_thread = 0;
bool vistek_logger::_vistek_logger_init_()
{
	if (_g_log_thread == 0)
	{
		v_lock(lk, _g_log_init_mtx);
		if (_g_log_thread == 0)
		{
			bool ret = _check_log_path();
			if (ret)
			{
				if (_g_use_io_service)
				{
					_g_log_thread = new thread(&vistek_logger::g_vistek_logger_run_io_service);
				}
				else
				{
					_g_log_thread = new thread(&vistek_logger::g_vistek_logger_proc_log_action);
				}
				_g_log_thread->detach();
				return true;
			}
		}
	}
	return _g_log_thread != 0;
}


void vistek_logger::_vistek_logger_clean_()
{
	if (_g_use_io_service)
	{
		_g_log_worker.reset();
		_g_log_service.stop();
		/*if (!_g_log_service.stopped())
		{
			_g_log_cond.wait_for(lck, chrono::milliseconds(1000));
		}*/
	}
	else
	{
		v_unique_lock(lck, s_vistek_logger_init_mutex);
		if (s_vistek_logger_init_flag)
		{
			s_vistek_logger_init_flag = 0;
			s_vistek_logger_proc_cond.notify_all();
			_g_log_cond.wait_for(lck, chrono::seconds(2));
		}
	}
}
void run_action(deque<std::function<void()>>& action_list, int64_t& count)
{
	
	if (!action_list.empty())
	{
		count += action_list.size();
		for (auto& act : action_list)
		{
			act();
		}
	}
}
void vistek_logger::_process_log_(bool bWrite, vistek_log_type lt, const string& source, string&& content)
{
	if (_vistek_logger_init_())
	{
		string date_str = v_current_date_string();
		function<void()> action = [source, content, bWrite, lt, date_str]()
		{
			if (bWrite)
			{
				_write(date_str, source, content);
			}
			print(lt, content);
		};
		if (_g_use_io_service)
		{
			_g_log_service.dispatch(action);
		}
		else
		{
			v_unique_lock(lck, s_vistek_log_action_queue_mutex);
			s_vistek_log_action_queue.push_back(action);
			lck.unlock();
			s_vistek_logger_proc_cond.notify_one();
		}
	}
}

void vistek_logger::g_vistek_logger_run_io_service()
{
	boost_error_code ec;
	_g_log_service.run(ec);
	v_unique_lock(lk, s_vistek_logger_proc_mtx);
	_g_log_cond.notify_all();
	cout << "vistek_logger terminated. (io_service mode)" << endl;
}



void vistek_logger::g_vistek_logger_proc_log_action()
{
	int64_t count = 0;
	int64_t round = 0;
	deque<std::function<void()>> temp;
	while (s_vistek_logger_init_flag)
	{
		v_unique_lock(lk, s_vistek_logger_proc_mtx);
		s_vistek_logger_proc_cond.wait_for(lk, chrono::milliseconds(1000));
		if (count % 1000000)
		{
			deque<std::function<void()>> temp1;
			temp.swap(temp1);
		}
		{
			v_unique_lock(lck, s_vistek_log_action_queue_mutex);
			temp.swap(s_vistek_log_action_queue);
		}
		run_action(temp, count);
		temp.clear();
		if (!s_vistek_logger_init_flag)
		{
			break;
		}
		if (round % 300 == 0)
		{
			_clean_log_by_time_();
		}
		round++;
	}
	v_unique_lock(lck, s_vistek_log_action_queue_mutex);
	run_action(s_vistek_log_action_queue, count);
	_g_log_cond.notify_all();
	cout << "vistek_logger terminated. (normal thread mode)" << endl;
}

void vistek_logger::_clean_log_by_time_()
{
	boost_error_code ec;
	int64_t day30_seconds = 0 - 24 * 3600 * 30;
	string log_limit = v_get_date_string(day30_seconds);
	fsys::directory_iterator dir(g_log_path, ec);
	for (auto f : dir)
	{
		if (fsys::is_directory(f.path(), ec))
		{
			string fn = f.path().leaf().string();
			if (fn.length() == 10)
			{
				if (fn < log_limit)
				{
					try
					{
						fsys::remove_all(f, ec);
					}
					catch (exception& ex)
					{
						cout << "failed to delete log dir : " << fn << endl;
					}
				}
			}
		}
	}
}


bool vistek_logger::g_vistek_verbose_log = false;

const char* vistek_log_type_names[] =
{
	"ERR",
	"WRN",
	"INFO",
	"VERBOSE",
	"VERBOSE(1)",
	"VERBOSE(2)",
	"VERBOSE(3)"
};

VISTEK_LOGGER_API void vlog_set_verbose(bool flag)
{
	vistek_logger::g_vistek_verbose_log = flag;
}

void vistek_logger::input(vistek_log_type lt, const std::string& filename, int line, const std::string& funcName, const std::string& source, bool bWrite, const std::string& content)
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
	stringstream ss;
	ss.imbue(locale(std::cout.getloc(), tf));
	ss << "[" << vistek_log_types_to_str(lt) << "]" << pt << ": " << filename << "(" << line << "): " << funcName << ": " << content;
	string output_str = ss.str();
	if (bWrite)
	{
		_write(v_current_date_string(), source, output_str);
	}
	print(lt, output_str);
	//v_smart_lock(lck, g_mtx_std_out);
	//switch (lt)
	//{
	//case v_LOG_INFO:
	//	cout << green << output_str << white << endl;
	//	break;
	//case v_LOG_ERR:
	//	cout << red << output_str << white << endl;
	//	break;
	//case v_LOG_WRN:
	//	cout << yellow << output_str << white << endl;
	//	break;
	//default:
	//	cout << blue << output_str << white << endl;
	//	break;
	//}
}
void vistek_logger::_write(const string& date_str, const string& source, const string& output_str)
{
	boost_error_code ec;
	path filename(g_log_path);
	filename /= date_str;
	if (!fsys::exists(filename, ec))
	{
		if (!fsys::create_directories(filename, ec))
		{
			return;
		}
	}
	filename /= (source + ".log");
	try
	{
		ofstream fs(filename.c_str(), ios::app);
		if (!fs.bad() && fs.is_open())
		{
			fs << output_str << endl;
		}
	}
	catch (std::exception& e)
	{
		cout << "write log failed; msg: " << e.what() << endl;
	}
}

void vistek_logger::print(vistek_log_type lt, const std::string& content)
{
	//v_smart_lock(lck, g_mtx_std_out);
	switch (lt)
	{
	case v_LOG_INFO:
		cout << green << content << white << endl;
		break;
	case v_LOG_ERR:
		cout << red << content << white << endl;
		break;
	case v_LOG_WRN:
		cout << yellow << content << white << endl;
		break;
	default:
		cout << blue << content << white << endl;
		break;
	}
}

vistek_log_type vistek_logger::str_to_vistek_log_types(const std::string& str)
{
	for (int i = 0; i < sizeof (vistek_log_type_names) / sizeof (vistek_log_type_names[0]); i++)
	{
		if (strcmp(str.c_str(), vistek_log_type_names[i]) == 0)
		{
			return static_cast <vistek_log_type>(i);
		}
	}
	throw runtime_error("Failed to convert a string to OSType");
}

const char* vistek_logger::vistek_log_types_to_str(vistek_log_type os)
{
	int index = (int)os;
	int count = sizeof (vistek_log_type_names) / sizeof (vistek_log_type_names[0]);
	if (index < count)
	{
		return vistek_log_type_names[index];
	}
	return "";
}
