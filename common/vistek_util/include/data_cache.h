#ifndef data_cache_h__
#define data_cache_h__

#pragma once
using namespace std;
#include "vistek_lock_defs.h"
template<typename K, typename V>
class data_cache
{
	map<K, V> m_data_cache;
	mutex mtx_data_cache;
public:
	typedef K key_type;
	typedef V val_type;
public:
	data_cache()
	{

	}

	virtual ~data_cache()
	{
		//v_lock(lck, mtx_data_cache);
		//m_data_cache.clear();
	}
	map<K, V> get_copy()
	{
		map<K, V> result;
		{
			v_lock(lck, mtx_data_cache);
			result = m_data_cache;
		}
		return move(result);
	}
	vector<V> get_array()
	{
		vector<V> result; 
		v_lock(lck, mtx_data_cache);
		for (auto& x : m_data_cache)
		{
			result.push_back(x.second);
		}
		return move(result);
	}

	int size()
	{
		int result = 0;
		{
			v_lock(lck, mtx_data_cache);
			result = m_data_cache.size();
		}
		return result;
	}

	int insert(K key, V val, bool bReplace = true)
	{
		v_lock(lck, mtx_data_cache);
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
		v_lock(lck, mtx_data_cache);
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
		v_lock(lck, mtx_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			m_data_cache.erase(iter);
		}
		return m_data_cache.size();
	}
	void remove_all()
	{
		v_lock(lck, mtx_data_cache);
		m_data_cache.clear();
	}
	void clear()
	{
		v_lock(lck, mtx_data_cache);
		m_data_cache.clear();
	}
	bool find(K key, V& val)
	{
		v_lock(lck, mtx_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			val = iter->second;
			return true;
		}
		return false;
	}

	///Deprecated
	int find_or_insert_old(const K& key, V& val, function<V()> creator)
	{
		v_lock(lck, mtx_data_cache);
		auto iter = m_data_cache.find(key);
		if (iter != m_data_cache.end())
		{
			val = iter->second;
		}
		else
		{
			auto x = creator();
			val = x;
			m_data_cache.insert(make_pair(key, val));
		}
		return m_data_cache.size();
	}
	int find_or_insert(const K& key, V& val, function<void(V&)> creator)
	{
		v_lock(lck, mtx_data_cache);
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
};

#endif // data_cache_h__
