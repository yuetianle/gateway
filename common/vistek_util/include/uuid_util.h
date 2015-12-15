#ifndef _UUID_UTIL_H_
#define _UUID_UTIL_H_

#pragma once

#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
using namespace std;
using namespace boost::uuids;

class v_uuid  : public uuid
{
private:
	static random_generator rgen; //随机生成器
	static string_generator sgen; //随机生成器
public:
	v_uuid(): uuid(rgen()){}//缺省构造函数，生成随机UUID

	v_uuid(int):uuid(nil_uuid()){}//0值的uuid构造函数

	v_uuid(const char*str) :uuid(sgen(str)){}//字符串构造函数
	v_uuid(const string& str) :uuid(sgen(str)){}//字符串构造函数

	v_uuid(const uuid&u, const char* str) : uuid(name_generator(u)(str)){}//名字生成器构造函数
	explicit v_uuid(const uuid& u):uuid(u){} //拷贝构造函数
	
	operator uuid()//转换到uuid类型
	{ return static_cast<boost::uuids::uuid&>(*this);}

	operator uuid() const //常函数，转换到const uuid类型
	{ return static_cast<const uuid&>(*this);}
	operator string()//转换到string类型
	{
		return this->tostring();
	}
	string tostring()
	{
		return boost::lexical_cast<string>(*this);
	}
};
#endif