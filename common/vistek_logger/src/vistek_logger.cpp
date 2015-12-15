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

path g_log_path = boost::filesystem::current_path() / "log";

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
void vistek_logger::process_log(bool bWrite, vistek_log_type lt, string content)
{
	string date_str = v_current_date_string();
	auto self = shared_from_this();
	if (bWrite)
	{
		m_strand_log.post(bind(&vistek_logger::_write_ex, self, m_source_name, move(date_str), content));
	}
	
	m_strand_print.post(bind(&vistek_logger::_print_ex, self, lt, content));
}



void vistek_logger::_clean_log_by_time_()
{
	cout << "=============== begin check log ===============" << endl;
	boost_error_code ec;
	int64_t day30_seconds = 0 - 24 * 3600 * 30;
	string log_limit = v_get_date_string(day30_seconds);
	fsys::directory_iterator dir(g_log_path, ec);
	int count = 0;
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
						count++;
					}
					catch (exception& ex)
					{
						cout << "failed to delete log dir : " << fn << endl;
					}
				}
			}
		}
	}
	cout << "=============== end check log (" << count << ") have been deleted ===============" << endl;
}


const char* vistek_log_type_names[] =
{
	"ERR",
	"WRN",
	"INFO",
	"TEXT",
	"VERBOSE",
	"VERBOSE(1)",
	"VERBOSE(2)",
	"VERBOSE(3)"
};

VISTEK_LOGGER_API void vlog_set_verbose(bool flag)
{
	vistek_logger::s_vistek_verbose_log = flag;
}
class filewriterc
{
	FILE* m_fp = 0;
	vector<char> m_buffer;
public:
	filewriterc(const string& fp, size_t buffersize = 0x1000)
		: m_buffer(buffersize)
	{
		m_fp = fopen(fp.c_str(), "a");
		if (m_fp)
		{
			std::setvbuf(m_fp, m_buffer.data(), _IOLBF, m_buffer.size());
		}
	}
	~filewriterc()
	{
		fclose(m_fp);
		m_fp = 0;
	}
	size_t write(const char* buff, int size)
	{
		size_t sz = fwrite(buff, 1, size, m_fp);
		return sz;
	}
	int flush()
	{
		return std::fflush(m_fp);
	}

};
typedef shared_ptr<filewriterc> fswriter_ptr;
map<string, fswriter_ptr> fs_map;

void vistek_logger::_write_ex(const std::string& source, std::string date_str, std::string output_str)
{
	boost_error_code ec;
	path filename(g_log_path);
	filename /= date_str;
	if (!fsys::exists(filename, ec))
	{
		_clean_log_by_time_();
		if (!fsys::create_directories(filename, ec))
		{
			return;
		}
	}
	filename /= (source + ".log");
	shared_ptr<FILE> fp(fopen(filename.string().c_str(), "a"), &std::fclose);
	if (fp != nullptr)
	{
		//string s = output_str.append("\r\n");
		fwrite(output_str.c_str(), 1, output_str.length(), fp.get());
	}
}

void vistek_logger::_write(const std::string& source, std::string date_str, string output_str)
{
	boost_error_code ec;
	path filename(g_log_path);
	filename /= date_str;
	if (!fsys::exists(filename, ec))
	{
		_clean_log_by_time_();
		if (!fsys::create_directories(filename, ec))
		{
			return;
		}
	}

	fswriter_ptr writer = 0;
	auto iter = fs_map.find(source);
	if (iter != fs_map.end())
	{ 
		writer = iter->second;
	}
	filename /= (source + ".log");
	if (!boost::filesystem::exists(filename, ec))
	{
		if (iter != fs_map.end())
		{
			fs_map.erase(iter);
			writer = 0;
		}
		_clean_log_by_time_();
	}
	if (writer == 0)
	{
		//writer = make_shared<ofstream>(filename.c_str(), ios::app);
		writer = make_shared<filewriterc>(filename.string());
		fs_map.insert(make_pair(source, writer));
	
	}
	try
	{
		if (writer != 0)
		{
			//if (!writer->bad() && writer->is_open())
			{
				writer->write(output_str.data(), output_str.length());
				/*if (v_get_duration_seconds(m_write_ts) > 10)
				{
					writer->flush();
					m_write_ts = v_get_timestamp_seconds();
				}*/
			}
		}
	}
	catch (std::exception& e)
	{
		cout << "write log failed; msg: " << e.what() << endl;
	}
}

void vistek_logger::_print_ex(vistek_log_type lt, const std::string& content)
{
#ifdef WIN32
	FILE* out_handle = stdout;
	static void* stdhdl = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (lt)
	{
	case v_LOG_INFO:
		SetConsoleTextAttribute(stdhdl, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		break;
	case v_LOG_ERR:
		out_handle = stderr;
		SetConsoleTextAttribute(stdhdl, FOREGROUND_INTENSITY | FOREGROUND_RED);
		break;
	case v_LOG_WRN:
		out_handle = stderr;
		SetConsoleTextAttribute(stdhdl, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case v_LOG_TEXT:
		SetConsoleTextAttribute(stdhdl, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case v_LOG_VERBOSE:
		SetConsoleTextAttribute(stdhdl, FOREGROUND_INTENSITY);
		break;
	case v_LOG_VERBOSE1:
		SetConsoleTextAttribute(stdhdl, FOREGROUND_GREEN);
		break;
	case v_LOG_VERBOSE2:
		SetConsoleTextAttribute(stdhdl, FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	default:
		SetConsoleTextAttribute(stdhdl, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		break;
	}
	fprintf_s(stdout, "%s", content.c_str());
	SetConsoleTextAttribute(stdhdl, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	switch (lt)
	{
	case v_LOG_INFO:
		printf(LGREEN "%s" WHITE, content.c_str());
		break;
	case v_LOG_ERR:
		printf(LRED "%s" WHITE, content.c_str());
		break;
	case v_LOG_WRN:
		printf(LYELLOW "%s" WHITE, content.c_str());
		break;
	case v_LOG_TEXT:
		printf(LWHITE "%s" WHITE, content.c_str());
		break;
	default:
		printf(LBLUE "%s" WHITE, content.c_str());
		break;
	}
#endif
}

void vistek_logger::_print(vistek_log_type lt, const std::string& content)
{
	//v_smart_lock(lck, g_mtx_std_out);
	switch (lt)
	{
	case v_LOG_INFO:
		cout << green << content << white << endl;
		break;
	case v_LOG_ERR:
		std::cerr << red << content << white << endl;
		break;
	case v_LOG_WRN:
		std::cerr << yellow << content << white << endl;
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

void vistek_logger::_try_init_(int concurency)
{
	static std::once_flag oc;
	std::call_once(oc, [&]()
	{
		auto svc = s_log_service;
		for (int i = 0; i < concurency; ++i)
		{
			thread trd([svc]()
			{
				boost::system::error_code ec;
				svc->run(ec);
				cout << "vistek_logger log thread terminated. " << endl;
			});
			trd.detach();
		}
		{
			auto svc1 = s_print_service;
			thread trd([svc1]()
			{
				boost::system::error_code ec;
				svc1->run(ec);
				cout << "vistek_logger print thread terminated. " << endl;
			});
			trd.detach();
		}

	});

	//if (!s_init_flag)
	//{
	//	v_lock(lck, s_init_mtx);
	//	if (!s_init_flag)
	//	{
	//		
	//	s_init_flag = 1;
	//	}
	//}
}

vistek_logger::ptr vistek_logger::get_instance(const log_key& key)
{
	ptr p = 0;
	bool bCreate = false;
	string src = std::get<0>(key);
	int size = s_log_cache.find_or_insert(key, p, [&src, &key, &bCreate](ptr& pp)
	{
		auto logger = make_shared<vistek_logger>(src);
		pp = logger;
		bCreate = true;
	});
	if (bCreate)
	{
		cout << "******************** current logger count = " << size << "; new logger: " << src << endl;
	}
	return p;
}

shared_ptr<boost::asio::io_service> vistek_logger::s_log_service(new boost::asio::io_service);
shared_ptr<boost::asio::io_service::work> vistek_logger::s_log_worker(new boost::asio::io_service::work(*s_log_service));

shared_ptr<boost::asio::io_service> vistek_logger::s_print_service(new boost::asio::io_service);
shared_ptr<boost::asio::io_service::work> vistek_logger::s_print_worker(new boost::asio::io_service::work(*s_print_service));

bool vistek_logger::s_vistek_verbose_log = false;
data_cache<vistek_logger::log_key, vistek_logger::ptr> vistek_logger::s_log_cache;


