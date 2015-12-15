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
	static random_generator rgen; //���������
	static string_generator sgen; //���������
public:
	v_uuid(): uuid(rgen()){}//ȱʡ���캯�����������UUID

	v_uuid(int):uuid(nil_uuid()){}//0ֵ��uuid���캯��

	v_uuid(const char*str) :uuid(sgen(str)){}//�ַ������캯��
	v_uuid(const string& str) :uuid(sgen(str)){}//�ַ������캯��

	v_uuid(const uuid&u, const char* str) : uuid(name_generator(u)(str)){}//�������������캯��
	explicit v_uuid(const uuid& u):uuid(u){} //�������캯��
	
	operator uuid()//ת����uuid����
	{ return static_cast<boost::uuids::uuid&>(*this);}

	operator uuid() const //��������ת����const uuid����
	{ return static_cast<const uuid&>(*this);}
	operator string()//ת����string����
	{
		return this->tostring();
	}
	string tostring()
	{
		return boost::lexical_cast<string>(*this);
	}
};
#endif