#ifndef _MSG_XML_BUILDER_H_
#define _MSG_XML_BUILDER_H_
#pragma once
#include "device_data_node.h"
class msg_xml_builder
{
public:
	msg_xml_builder();
	~msg_xml_builder();
public:
	int add_msg_header(header_type head_type, const string &msg_id, const string &dev_id, const string &res_type);
	int add_msg_content(const char *content, const long content_len);
private:
	pugi::xml_document m_doc;
};
#endif
