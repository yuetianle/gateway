#pragma once
#include "pugiconfig.hpp"
#include "pugixml.hpp"


#ifndef SEARCH_NODE_DEF
#define SEARCH_NODE_DEF(v_name, NODE_NAME) \
inline bool v_##v_name##_search_node(pugi::xml_node item_node)\
{\
 return v_str_compare_no_case(item_node.name(), NODE_NAME);\
}
#endif
SEARCH_NODE_DEF(channel_lists, "StreamingChannelList")
SEARCH_NODE_DEF(channel_node, "StreamingChannelNode")
class device_host;

typedef vector<string>				stream_urls_lists_vec;
typedef vector<string>::iterator	stream_urls_lists_vec_iter;
class psia_xml_converter
{
public:
	psia_xml_converter(device_host *psia_host, const string &psia_xml);
	~psia_xml_converter();
public:
	string std_xml() const { return m_std_xml; }
	string psia_xml()const { return m_psia_xml; }
private:
	int _parse_channel_xml();
	int _create_stream_url_xml();
private:
	string m_psia_xml;
	string m_std_xml;
	pugi::xml_document m_in_doc;
	pugi::xml_document m_out_doc;
	device_host *m_psia_host;
	stream_urls_lists_vec m_streams_urls;
};

