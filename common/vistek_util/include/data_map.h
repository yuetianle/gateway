#ifndef data_cache_ex_h__
#define data_cache_ex_h__

#pragma once
#include "vistek_lock_defs.h"
#include "vistek_error_code.h"
using namespace std;
template<typename K, typename V>
class data_map
{
	map<K, V> m_data_cache;
public:
	data_map()
	{

	}
	virtual ~data_map()
	{
	}
	map<K, V> get_copy()
	{
		map<K, V> result;
		result = m_data_cache;
		return move(result);
	}
	vector<V> get_array()
	{
		vector<V> result;
		{
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
			result = m_data_cache.size();
		}
		return result;
	}
	int find_or_insert(K key, V& val, function<V && ()> creator)
	{
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			val = *iter;
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
	int insert(K key, V val, bool bReplace = true)
	{
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
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			m_data_cache.erase(iter);
		}
		return m_data_cache.size();
	}
	void remove_all()
	{
		m_data_cache.clear();
	}
	bool contains(K key)
	{
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			return true;
		}
		return false;
	}
	bool find(K key, V& val)
	{
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
