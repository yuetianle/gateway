#include "stdafx.h"
#include "psia_xml_converter.h"
#include "device_host.h"
psia_xml_converter::psia_xml_converter(device_host *psia_host, const string &psia_xml)
	: m_psia_xml(psia_xml), m_psia_host(psia_host)
{
	m_in_doc.reset();
	m_out_doc.reset();
	pugi::xml_node decl = m_out_doc.prepend_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";
	if (!psia_xml.empty())
	{
		pugi::xml_parse_result xml_ret = m_in_doc.load_string(psia_xml.c_str());
		if (pugi::xml_parse_status::status_ok == xml_ret.status)
		{
			_parse_channel_xml();
			_create_stream_url_xml();
		}
		stringstream ss;
		if (m_out_doc)
		{
			m_out_doc.save(ss);
		}
		m_std_xml = ss.str();
	}

}

psia_xml_converter::~psia_xml_converter()
{
}

int psia_xml_converter::_parse_channel_xml()
{
	pugi::xml_node channel_lists_node = m_in_doc.find_child(v_channel_lists_search_node);
	for (pugi::xml_node channel : channel_lists_node.children("StreamingChannel"))
	{
		char stream_url[1024] = { 0 };
		int id = 0;
		bool b_support_rtsp = false;
		bool bvideo = channel.child("enabled").text().as_bool();
		if (bvideo)
		{
			id = channel.child("id").text().as_int();
			int rstp_port = channel.child("Transport").child("rtspPortNo").text().as_int();
			pugi::xml_node protocol_nodes = channel.child("Transport").child("ControlProtocolList");
			for (pugi::xml_node protocol_node = protocol_nodes.first_child(); protocol_node; protocol_nodes.next_sibling())
			{
				if (v_str_compare_no_case(protocol_node.child("streamingTransport").text().as_string(), "rtsp"))
				{
					b_support_rtsp = true;
					break;
				}
			}
			pugi::xml_node unicast_node = channel.child("Transport").child("Unicast");
			bool b_support_unicast = unicast_node.child("enabled").text().as_bool();
			if (b_support_rtsp && b_support_unicast)
			{
				sprintf_s(stream_url, "rtsp://%s:%d/Streaming/Channels/%d?transportmode=%s&profile=Profile_%d", m_psia_host->device_ip().c_str(), rstp_port, id, "unicast", id);
			}
		}
		if (0 < strlen(stream_url))
		{
			m_streams_urls.push_back(stream_url);
		}
	}
	return v_OK;
}

int psia_xml_converter::_create_stream_url_xml()
{
	pugi::xml_node streams_node = m_out_doc.append_child("stream_url_lists");
	for (stream_urls_lists_vec_iter item = m_streams_urls.begin(); item < m_streams_urls.end(); ++item)
	{
		if (streams_node)
		{
			pugi::xml_node stream_node = streams_node.append_child("stream_url");
			stream_node.text().set((*item).c_str());
		}
	}
	if (!m_streams_urls.empty())
	{
		return v_OK;
	}
	else
	{
		return v_ERR_Bad_XML;
	}
}
