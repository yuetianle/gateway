#ifndef _DEVICE_DATA_NODE_
#define _DEVICE_DATA_NODE_
#pragma once
#pragma region XML_DEF
#include "pugixml.hpp"
#include "pugiconfig.hpp"
enum header_type
{
	req_header = 0,
	res_header = 1
};
#ifndef SEARCH_NODE_DEF
#define SEARCH_NODE_DEF(v_name, NODE_NAME) \
inline bool v_##v_name##_search_node(pugi::xml_node item_node)\
{\
 return v_str_compare_no_case(item_node.name(), NODE_NAME);\
}
#endif

SEARCH_NODE_DEF(request, "request")
SEARCH_NODE_DEF(cmd, "cmd")
#pragma region NODE
#ifndef DEVICE_EXT_CONTENT_NODE_NAME
#define DEVICE_EXT_CONTENT_NODE_NAME "ext_params"
#endif

#ifndef DEVICE_URI_NODE_NAME
#define DEVICE_URI_NODE_NAME "uri"
#endif

#ifndef DEVICE_REQUEST_NODE_NAME
#define DEVICE_REQUEST_NODE_NAME "request"
#endif

#ifndef DEVICE_RESPONSE_NODE_NAME
#define DEVICE_RESPONSE_NODE_NAME "response"
#endif
#pragma endregion NODE

#pragma region ATTR
#ifndef DEVICE_MESG_ID_ATTR_NAME 
#define DEVICE_MESG_ID_ATTR_NAME "msg_id"
#endif
#ifndef DEVICE_ID_ATTR_NAME 
#define DEVICE_ID_ATTR_NAME "dev_id"
#endif
#ifndef DEVICE_RES_TYPE_ATTR_NAME
#define DEVICE_RES_TYPE_ATTR_NAME "res_type"
#endif
#ifndef DEVICE_MANU_ATTR_NAME
#define DEVICE_MANU_ATTR_NAME "manufacture"
#endif

#pragma endregion ATTR
#pragma endregion XML_DEF

#endif