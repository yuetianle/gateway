#ifndef _MSG_PROCESSER_H_
#define _MSG_PROCESSER_H_
#pragma once
#include "msg_xml_parser.h"
#include "msg_xml_builder.h"
typedef std::function<int (const char*, const char*, void *, char*, long *)> MSG_PROC_HANDLE;/* << */
class msg_processer
{
public:
	msg_processer();
	~msg_processer();
public:
	int process_msg(char *msg, const long &msg_len, char* out_data, long *out_data_len);
	int subscribe_msg_action(MSG_PROC_HANDLE &&handler);
private:
	pugi::xml_document m_xml_msg;
	MSG_PROC_HANDLE m_msg_proc;
};
#endif

