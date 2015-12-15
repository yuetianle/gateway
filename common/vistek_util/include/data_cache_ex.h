#ifndef data_cache_ex_h__
#define data_cache_ex_h__

#pragma once
#include "vistek_lock_defs.h"
#include "vistek_error_code.h"
using namespace std;
template<typename K, typename V>
class data_cache_ex
{
	map<K, V> m_data_cache;
	vistek_smart_locker lck_data_cache;
	
public:
	data_cache_ex()
	{

	}
	virtual ~data_cache_ex()
	{
	}
	map<K, V> get_copy()
	{
		map<K, V> result;
		{
			v_smart_lock(lck, lck_data_cache);
			result = m_data_cache;
		}
		return move(result);
	}
	vector<V> get_array()
	{
		vector<V> result;
		{
			v_smart_lock(lck, lck_data_cache);
			for (auto& x : m_data_cache)
			{
				result.push_back(x.second);
			}
		}
		return move(result);
	}

	int size()
	{
		int result = 0;
		{
			v_smart_lock(lck, lck_data_cache);
			result = m_data_cache.size();
		}
		return result;
	}
	///Deprecated
	int find_or_insert_old(K key, V& val, function<V()> creator)
	{
		v_smart_lock(lck, lck_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			val = iter->second;
		}
		else
		{
			//if (!contains(key))
			{
				val = creator();
				m_data_cache.insert(make_pair(key, val));
			}
		}
		return m_data_cache.size();
	}
	int find_or_insert(K key, V& val, function<void(V&)> creator)
	{
		v_smart_lock(lck, lck_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			val = iter->second;
		}
		else
		{
			creator(val);
			m_data_cache.insert(make_pair(key, val));
		}
		return m_data_cache.size();
	}
	int insert(K key, V val, bool bReplace = true)
	{
		v_smart_lock(lck, lck_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			if (bReplace)
			{
				m_data_cache.erase(iter);
				m_data_cache.insert(pair<K, V>(key, val));
			}
		}
		else
		{
			m_data_cache.insert(pair<K, V>(key, val));
		}
		return m_data_cache.size();
	}
	int remove(K key, V& val)
	{
		v_smart_lock(lck, lck_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			val = iter->second;
			m_data_cache.erase(iter);
		}
		return m_data_cache.size();
	}
	int remove(K key)
	{
		v_smart_lock(lck, lck_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			m_data_cache.erase(iter);
		}
		return m_data_cache.size();
	}
	void remove_all()
	{
		v_smart_lock(lck, lck_data_cache);
		m_data_cache.clear();
	}
	bool contains(K key)
	{
		v_smart_lock(lck, lck_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			return true;
		}
		return false;
	}
	bool find(K key, V& val)
	{
		v_smart_lock(lck, lck_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			val = iter->second;
			return true;
		}
		return false;
	}
};

#endif // data_cache_h__
