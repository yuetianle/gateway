#ifndef ptz_base_h__
#define ptz_base_h__

#pragma once
#include <chrono>
#include <mutex>
#include <iostream>
#include "vistek_time_util.h"
#include "vistek_string_util.h"
struct ptz_session
{
	std::string _ptz_user_name;
	int _ptz_user_level;
	int64_t _last_operation_ts;
	std::mutex mtx_lock;
	int _lock_timeout_sec = 15;

	ptz_session(const std::string& un, int level)
		: _ptz_user_name(un), _ptz_user_level(level), _last_operation_ts(v_get_timestamp_seconds())
	{

	}
	ptz_session()
		: _ptz_user_name(""), _ptz_user_level(0), _last_operation_ts(v_get_timestamp_seconds())
	{

	}
	bool get_owner(string& owner, int64_t& remain_seconds)
	{
		int64_t timespan = v_get_duration_seconds(_last_operation_ts);
		remain_seconds = (int64_t)_lock_timeout_sec - timespan;
		if (remain_seconds > 0)
		{
			owner = _ptz_user_name;
			return true;
		}
		else
		{
			owner = "";
			remain_seconds = 0;
		}
		return false;
	}
	

	bool request(const std::string& un, int level, int lock_time_sec)
	{
		int ret = false;
		std::lock_guard<std::mutex>lck(mtx_lock);
		if (un == _ptz_user_name)
		{
			_last_operation_ts = v_get_timestamp_seconds();
			if (_lock_timeout_sec < lock_time_sec)
			{
				_lock_timeout_sec = lock_time_sec;
			}
			if (lock_time_sec == 0)
			{
				_ptz_user_name = "";
				_ptz_user_level = 0;
			}
			ret = true;
		}
		else
		{

			int64_t timespan = v_get_duration_seconds(_last_operation_ts);
			if (timespan >= (int64_t)_lock_timeout_sec)
			{
				_ptz_user_level = 0;
				_last_operation_ts = v_get_timestamp_seconds();
				_lock_timeout_sec = 0;
				ret = true;
			}
			if (level >= _ptz_user_level)
			{
				if (_ptz_user_name == "" && v_str_compare_no_case(un, "ffffffff-ffff-ffff-ffff-ffffffffffff"))
				{
					ret = false;
				}
				else
				{
					_ptz_user_name = un;
					_ptz_user_level = level;
					_last_operation_ts = v_get_timestamp_seconds();
					_lock_timeout_sec = lock_time_sec;
					ret = true;
				}
			}
		}
		return ret;
	}
};
class ptz_base
{
protected:
	ptz_session m_session;
public:
	virtual bool get_owner(string& owner, int64_t& remain_seconds)
	{
		return m_session.get_owner(owner, remain_seconds);
	}

	virtual bool request(const std::string& uid, int level, int lock_time_sec = 15)
	{
		bool ret = m_session.request(uid, level, lock_time_sec);
		return ret;
	}
	virtual int set_position(double x, double y, double w, double h, int scale) = 0;
	virtual bool has_3D_position() = 0;
	virtual int move(double pan, double tilt) = 0;
	virtual int zoom_in() = 0;
	virtual int zoom_out() = 0;
	virtual int iris_open() = 0;
	virtual int iris_close() = 0;
	virtual int focus_forward() = 0;
	virtual int focus_backward() = 0;
	virtual int stop() = 0;
	virtual int control_light(bool bOpen) = 0;
	virtual int control_wiper(bool bOpen) = 0;
	virtual int control_heater(bool bOpen) = 0;

	virtual int add_preset(int index) = 0;
	virtual int remove_preset(int index) = 0;
	virtual int goto_preset(int index) = 0;
};

typedef std::shared_ptr<ptz_base> ptz_ptr;
#endif // ptz_base_h__
