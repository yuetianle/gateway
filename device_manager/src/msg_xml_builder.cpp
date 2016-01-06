#include "stdafx.h"
#include "msg_xml_builder.h"


msg_xml_builder::msg_xml_builder()
{
}


msg_xml_builder::~msg_xml_builder()
{
}

int msg_xml_builder::add_msg_header(header_type head_type, const string &msg_id, const string &dev_id, const string &res_type)
{
	pugi::xml_node req_node;
	pugi::xml_node res_node;
	switch (head_type)
	{
	case req_header:
		req_node = m_doc.append_child(DEVICE_REQUEST_NODE_NAME);
	case res_header:
		res_node = m_doc.append_child(DEVICE_RESPONSE_NODE_NAME);
	}
	if (req_node)
	{
		req_node.append_attribute(DEVICE_ID_ATTR_NAME).set_value(dev_id.c_str());
		req_node.append_attribute(DEVICE_MESG_ID_ATTR_NAME).set_value(msg_id.c_str());
		req_node.append_attribute(DEVICE_RES_TYPE_ATTR_NAME).set_value(res_type.c_str());
	}
	if (res_node)
	{
		res_node.append_attribute(DEVICE_ID_ATTR_NAME).set_value(dev_id.c_str());
		res_node.append_attribute(DEVICE_MESG_ID_ATTR_NAME).set_value(msg_id.c_str());
		res_node.append_attribute(DEVICE_RES_TYPE_ATTR_NAME).set_value(res_type.c_str());
	}
	return 0;
}
