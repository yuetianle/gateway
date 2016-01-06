#ifndef _MSG_XML_PARSER_H_
#define _MSG_XML_PARSER_H_
#pragma once
#include "device_data_node.h"
typedef tuple <int, string, string, int, int, string, int, string, string> DEVICE_INFO;
typedef tuple<string, string, string,string> MSG_HEADER;
class msg_xml_parser
{
public:
	msg_xml_parser(const string &xml_msg);
	~msg_xml_parser();
public:
	string uri() { return m_uri; }
	string device_list_xml() { return m_params; }
	string params() { return m_params; }
	string msg_id() { return std::get<0>(m_msg_header); }
	string device_id() { return std::get<1>(m_msg_header); }
	string res_type() { return std::get<2>(m_msg_header); }
	string manufacture(){ return get<3>(m_msg_header); }
	MSG_HEADER msg_header() { return m_msg_header; }
	bool is_invaild() { return m_is_invaild; }
private:
	int _parser();
private:
	string m_uri;
	string m_params;
	MSG_HEADER m_msg_header;
	string m_xml_msg;
	pugi::xml_document m_doc;
	bool m_is_invaild;
};

#endif
