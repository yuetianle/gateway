#include "stdafx.h"
#include "vistek_instance_counter.h"
#include "vistek_lock_defs.h"
#include "vistek_error_code.h"

vistek_instance_counter::vistek_instance_counter(void)
{
}


vistek_instance_counter::~vistek_instance_counter(void)
{
}


int vistek_instance_counter::inc( const string& key )
{
	v_lock(lck, m_mtx);
	auto iter = m_count_map.find(key);
	if(iter != m_count_map.end())
	{
		return ++(iter->second);
	}
	else
	{
		m_count_map.insert(pair<string,int>(key, 1));
		return 1;
	}
}


int vistek_instance_counter::dec( const string& key )
{
	v_lock(lck, m_mtx);
	auto iter = m_count_map.find(key);
	if(iter != m_count_map.end())
	{
		return --(iter->second);
	}
	return 0;
}


void vistek_instance_counter::init()
{
	if(g_instance_counter==nullptr)
	{
		v_lock(lck,g_mtx_vistek_instance_counter);
		if(g_instance_counter==nullptr)
		{
			g_instance_counter.reset(new vistek_instance_counter());
		}
	}
}

shared_ptr<vistek_instance_counter> const vistek_instance_counter::instance_counter()
{
	return g_instance_counter;
}

std::mutex vistek_instance_counter::g_mtx_vistek_instance_counter;

shared_ptr<vistek_instance_counter> vistek_instance_counter::g_instance_counter;

