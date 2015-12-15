#ifndef __vistek_instance_counter_h__
#define __vistek_instance_counter_h__
#pragma once
using namespace std;

class vistek_instance_counter
{
private:
	vistek_instance_counter(void);
	mutex m_mtx;
	map<string,int> m_count_map;
	static shared_ptr<vistek_instance_counter> g_instance_counter;

	static mutex g_mtx_vistek_instance_counter;
public:
	~vistek_instance_counter(void);
	int inc(const string& key);
	int dec(const string& key);
	static  shared_ptr<vistek_instance_counter> const instance_counter();
	static void init();
};
typedef vistek_instance_counter visteck_ic;
typedef shared_ptr<visteck_ic> visteck_ic_ptr;
#endif