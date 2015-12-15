#include "stdafx.h"
#include "msg_processer.h"
msg_processer::msg_processer()
{
}

msg_processer::~msg_processer()
{
}

int msg_processer::process_msg(char *msg, const long &msg_len, char* out_data, long *out_data_len)
{
	if (msg && 0 < msg_len)
	{
		msg_xml_parser msg_parser(msg);
#ifdef _DEBUG
		//vvlog_i("begin params:" << msg_parser.device_id().c_str() << msg_parser.uri().c_str() << (void*)msg_parser.params().c_str() << out_data << out_data_len)
#endif
		m_msg_proc(msg_parser.device_id().c_str(), msg_parser.uri().c_str(), (void*)msg_parser.params().c_str(), out_data, out_data_len);
		msg_xml_builder msg_builder();
		//vvlog_i("end params:" << msg_parser.device_id().c_str() << msg_parser.uri().c_str() << (void*)msg_parser.params().c_str() << out_data << out_data_len)
		if (out_data && 0 < *out_data_len)
		{
			return v_OK;
		}
		else
		{
			return v_ERR_Bad_XML;
		}
	}
}

int msg_processer::subscribe_msg_action(MSG_PROC_HANDLE &&handler)
{
	m_msg_proc = handler;
	return v_OK;
}

