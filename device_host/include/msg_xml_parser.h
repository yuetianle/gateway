#ifndef _MSG_XML_PARSER_H_
#define _MSG_XML_PARSER_H_
#pragma once
#include "device_data_node.h"
typedef tuple <int, string, string, int, int, string, int, string, string> DEVICE_INFO;
typedef tuple<string, string, string,string> MSG_HEADER;

typedef struct REQUEST_CONTENT
{
	string uri;
	string params;
	MSG_HEADER msg_header;

	string msg_id() { return std::get<0>(msg_header); }
	string device_id() { return std::get<1>(msg_header); }
	string res_type() { return std::get<2>(msg_header); }
	string manufacture(){ return get<3>(msg_header); }
}REQ_CONTENT;
typedef vector<REQ_CONTENT> REQ_CONTENT_VEC;
class msg_xml_parser
{
public:
	msg_xml_parser(const string &xml_msg);
	~msg_xml_parser();
public:
	//string device_list_xml() { return m_params; }
	bool is_invaild() { return m_is_invaild; }
	int request_cnt() { return m_req_content_vec.size(); }
	REQ_CONTENT_VEC& requests() { return m_req_content_vec; }
private:
	int _parser();
private:
	//string m_uri;
	//string m_params;
	//MSG_HEADER m_msg_header;
	REQ_CONTENT_VEC m_req_content_vec;
	string m_xml_msg;
	pugi::xml_document m_doc;
	bool m_is_invaild;
};

#endif
