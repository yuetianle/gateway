#include "stdafx.h"
#include "msg_xml_parser.h"


msg_xml_parser::msg_xml_parser(const string &xml_msg)
	:m_xml_msg(xml_msg), m_is_invaild(false)
{
	_parser();
}


msg_xml_parser::~msg_xml_parser()
{
}

int msg_xml_parser::_parser()
{
	pugi::xml_parse_result xml_ret = m_doc.load_string(m_xml_msg.c_str());
	if (pugi::xml_parse_status::status_ok == xml_ret.status)
	{
		for (auto request_item = m_doc.begin(); request_item != m_doc.end(); request_item++)
		{
			string device_id = request_item->attribute(DEVICE_ID_ATTR_NAME).as_string();
			string request_mesg_id = request_item->attribute(DEVICE_MESG_ID_ATTR_NAME).as_string();
			string res_type = request_item->attribute(DEVICE_RES_TYPE_ATTR_NAME).as_string();
			string manufacture_name = request_item->attribute(DEVICE_MANU_ATTR_NAME).as_string();
			string uri = (request_item->child(DEVICE_URI_NODE_NAME).text().as_string());
			MSG_HEADER msg_header = make_tuple(request_mesg_id, device_id, res_type, manufacture_name);
			//m_uri = request_node.child(DEVICE_URI_NODE_NAME).text().as_string();
			string params = request_item->child(DEVICE_EXT_CONTENT_NODE_NAME).text().as_string();
			REQ_CONTENT req_content;
			req_content.uri = uri;
			req_content.params = params;
			req_content.msg_header = msg_header;
			m_req_content_vec.push_back(req_content);
			m_is_invaild = false;
		}
		return v_OK;
		//pugi::xml_node request_node = m_doc.find_node(v_request_search_node);
		//if (request_node)
		//{
		//	string device_id = request_node.attribute(DEVICE_ID_ATTR_NAME).as_string();
		//	string request_mesg_id = request_node.attribute(DEVICE_MESG_ID_ATTR_NAME).as_string();
		//	string res_type = request_node.attribute(DEVICE_RES_TYPE_ATTR_NAME).as_string();
		//	string manufacture_name = request_node.attribute(DEVICE_MANU_ATTR_NAME).as_string();
		//	for (auto item = request_node.begin(); item != request_node.end(); item++)
		//	{
		//		if (v_str_compare_no_case(item->name(), "uri"))
		//		{
		//			m_uri.append(request_node.child(DEVICE_URI_NODE_NAME).text().as_string());
		//			m_uri.append(";");
		//		}
		//	}
		//	m_msg_header = make_tuple(request_mesg_id, device_id, res_type, manufacture_name);
		//	//m_uri = request_node.child(DEVICE_URI_NODE_NAME).text().as_string();
		//	m_params = request_node.child(DEVICE_EXT_CONTENT_NODE_NAME).text().as_string();
		//	m_is_invaild = false;
		//	return v_OK;
		//}
	}
	m_is_invaild = true;
	return v_ERR_Bad_XML;
}
