#ifndef __VISTEK_TIME_UTIL_H__
#define __VISTEK_TIME_UTIL_H__
#include <memory>
#include <chrono>
#include <iostream>
#include <boost/date_time.hpp>
#include <math.h>

const boost::posix_time::ptime g_time_base = boost::posix_time::ptime(boost::gregorian::date(2000, 1, 1), boost::posix_time::time_duration(0, 0, 0));
const boost::posix_time::ptime g_time_base1 = boost::posix_time::ptime(boost::gregorian::date(1900, 1, 1), boost::posix_time::time_duration(0, 0, 0));

inline int64_t v_get_playback_timestamp_by_tick_ms(int64_t tick_ms)
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
	
	int64_t base_ticks = (g_time_base - epoch).total_milliseconds();

	int64_t ts = tick_ms - base_ticks;

	return ts;
}
inline int64_t v_get_playback_timestamp_explicitly()
{
	boost::posix_time::ptime current = boost::posix_time::microsec_clock::local_time();
	int64_t total_ms = (current - g_time_base).total_milliseconds();
	return total_ms;
}
inline int64_t v_get_playback_timestamp()
{
	int64_t ts = v_get_playback_timestamp_explicitly();
	int64_t x = (int64_t)((double)ts / 1000 + 0.5) * 1000;
	return x;
}
inline boost::posix_time::ptime v_get_ptime_from_playback_timestamp(int64_t pts)
{
	boost::posix_time::ptime temp = g_time_base + boost::posix_time::milliseconds(pts);
//	auto pt1 = boost::posix_time::microsec_clock::local_time();
//	auto pt0 = boost::posix_time::microsec_clock::universal_time();
//	auto d = pt1 - pt0;
//#ifdef _DEBUG
//	int timezone = d.hours();
//#endif
//	temp += d;
	return temp;
}
inline int64_t v_get_playback_timestamp_from_ptime(const boost::posix_time::ptime& pt)
{
	int64_t total_ms = (pt - g_time_base).total_milliseconds();
	int64_t x = (int64_t)((double)total_ms / 1000 + 0.5) * 1000;
	return x;
}
inline boost::posix_time::ptime v_ptime_from_str(const std::string& timestr)
{
	try
	{
		//auto pt1 = boost::posix_time::microsec_clock::local_time();
		//auto pt0 = boost::posix_time::microsec_clock::universal_time();
		//auto d = pt1 - pt0;
		//int timezone = d.hours();

		boost::posix_time::ptime pt = boost::posix_time::time_from_string(timestr);
		return pt;
	}
	catch (std::exception& e)
	{
		std::cout << "get time stamp from str failed. error: " << e.what() << std::endl;
	}
	return boost::posix_time::ptime();

}
inline int64_t v_get_playback_timestamp_from_timestr(const std::string& timestr)
{
	if (timestr == "")
	{
		return 0;
	}
	try
	{
		boost::posix_time::ptime pt = boost::posix_time::time_from_string(timestr);
		auto ticks = (pt - g_time_base).total_milliseconds() / 1000 * 1000;
		return ticks;
	}
	catch (std::exception& e)
	{
		std::cout << "get time stamp from str failed. error: " << e.what() << std::endl;
	}
	return 0;
	
}
inline std::string v_get_timestr_from_playback_timestamp(int64_t ticks)
{
	boost::posix_time::ptime pt = g_time_base + boost::posix_time::milliseconds(ticks);
	//std::stringstream ss1;
	//boost::posix_time::time_facet* tf1 = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S"); //2013-09-28 18:00:00.000
	//ss1.imbue(std::locale(std::cout.getloc(), tf1));
	//ss1 << pt;
	std::string str = boost::posix_time::to_iso_extended_string(pt);
	str.replace(10, 1, " ");
	return str;
}

inline std::string v_ptime_to_string(const boost::posix_time::ptime& time)
{
	std::stringstream ss;
	//boost::posix_time::ptime pt  = time + boost::posix_time::hours(8);
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S"); //2013-09-28 18:00:00.000
	ss.imbue(std::locale(std::cout.getloc(), facet));
	ss << time;
	return ss.str();
}

inline std::string v_get_date_string(int64_t diff_seconds)
{
	boost::posix_time::ptime time = boost::posix_time::second_clock::local_time();
	if (diff_seconds != 0)
	{
		time = time + boost::posix_time::seconds(diff_seconds);
	}
	std::stringstream ss;
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet("%Y-%m-%d"); //2013-09-28
	ss.imbue(std::locale(std::cout.getloc(), facet));
	ss << time;
	return move(ss.str());
}

inline std::string v_current_date_string()
{
	boost::posix_time::ptime time = boost::posix_time::second_clock::local_time();
	std::stringstream ss;
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet("%Y-%m-%d"); //2013-09-28
	ss.imbue(std::locale(std::cout.getloc(), facet));
	ss << time;
	return move(ss.str());
}
inline std::string v_convert_stdtime_to_shorttime(const std::string& stdTimeStr)
{
	if (stdTimeStr.length() >= 19)
	{
		const char* str_iter = stdTimeStr.c_str();
		std::stringstream ss;
		ss.write(str_iter, 4);
		str_iter += 5;
		ss.write(str_iter, 2);
		str_iter += 3;
		ss.write(str_iter, 2);
		str_iter += 3;
		ss.write(str_iter, 2);
		str_iter += 3;
		ss.write(str_iter, 2);
		str_iter += 3;
		ss.write(str_iter, 2);
		return move(ss.str());
	}
	return "";
}
inline std::string v_convert_shorttime_to_stdtime(const std::string& shortTimeStr)
{
	if (shortTimeStr.length() >= 14)
	{
		const char* str_iter = shortTimeStr.c_str();
		std::stringstream ss;
		ss.write(str_iter, 4);
		str_iter += 4;
		ss << "-";
		ss.write(str_iter, 2);
		str_iter += 2;
		ss << "-";
		ss.write(str_iter, 2);
		str_iter += 2;
		ss << " ";
		ss.write(str_iter, 2);
		str_iter += 2;
		ss << ":";
		ss.write(str_iter, 2);
		str_iter += 2;
		ss << ":";
		ss.write(str_iter, 2);
		return move(ss.str());
	}
	return "";
}
inline std::string v_get_current_date_string()
{
	boost::posix_time::ptime time = boost::posix_time::second_clock::local_time();
	std::stringstream ss;
	//boost::posix_time::ptime pt  = time + boost::posix_time::hours(8);
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet("%Y-%m-%d 00:00:00"); //2013-09-28 18:00:00.000
	ss.imbue(std::locale(std::cout.getloc(), facet));
	ss << time;
	return ss.str();

}
inline std::string v_ptime_date_to_string(const boost::posix_time::ptime& time)
{
	std::stringstream ss;
	//boost::posix_time::ptime pt  = time + boost::posix_time::hours(8);
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet("%Y-%m-%d 00:00:00"); //2013-09-28 18:00:00.000
	ss.imbue(std::locale(std::cout.getloc(), facet));
	ss << time;
	return ss.str();
}

inline std::string v_get_current_time_str()
{
	boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
	return v_ptime_to_string(pt);
}

inline std::string v_get_current_time_with_ms_diff_str(int64_t duration)
{
	boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time() + boost::posix_time::milliseconds(duration);
	std::string str = boost::posix_time::to_iso_extended_string(pt);
	str.replace(10, 1, " ");
	return str;
	//return v_ptime_to_string(pt);
}

inline int64_t v_get_timestamp_seconds()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
inline int64_t v_get_timestamp_milliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
inline int64_t v_get_timestamp_microseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
inline int64_t v_get_timestamp_nanoseconds()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline int64_t v_get_duration_seconds( int64_t ts )
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch() - std::chrono::seconds(ts)).count();
}
inline int64_t v_get_duration_milliseconds(int64_t ts)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - std::chrono::milliseconds(ts)).count();
}
inline int64_t v_get_duration_microseconds( int64_t ts )
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch() - std::chrono::microseconds(ts)).count();
}
inline int64_t v_get_duration_nanoseconds( int64_t ts )
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch() - std::chrono::nanoseconds(ts)).count();
}
inline int32_t v_get_timezone()
{
	auto pt1 = boost::posix_time::microsec_clock::local_time();
	auto pt0 = boost::posix_time::microsec_clock::universal_time();
	auto d = pt1 - pt0;
	int timezone = d.hours();
	return timezone;
}

#endif