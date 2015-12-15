#include "stdafx.h"
#include "vistek_media_defs.h"
#include "boost/asio.hpp"
#include "pre_regex.h"
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

std::vector<std::shared_ptr<media_info>> media_info::parse_media_info_from_sdp(const std::string& sdp)
{
	std::vector<std::shared_ptr<media_info>> results(0);
	REGEX reg("m=video \\d+ RTP/AVP \\d+\r\n(((?!m=).)*)");
	SMATCH m;
	if (REGEX_SEARCH(sdp, m, reg))
	{
		if (m.size() > 1)
		{
			std::string video_info = m[1].str();
			media_info_ptr mip(new media_info(video_info));
			mip->payload_type = mip->rtp_map.pt_code;
			results.push_back(mip);
		}
	}

	REGEX reg1("m=audio \\d+ RTP/AVP (\\d+)\r\n(((?!m=).)*)");
	SMATCH m1;
	if (REGEX_SEARCH(sdp, m1, reg1))
	{
		if (m1.size() > 2)
		{
			int pt = atoi(m1[1].str().c_str());
			string audio_info = m1[2].str();
			media_info_ptr mip(new media_info(audio_info));
			mip->payload_type = pt;
			mip->rtp_map.pt_code = pt;
			mip->rtp_map.pkt_payload_type = (packet_payload_type)(400 + pt);
			results.push_back(mip);
		}
	}
	return results;
}

//<session>
//<sdp><![CDATA[]]></sdp>
//<media_list media_count="1">
//<media ssrc="0" codec="h264" clock="90000">
//<ext>test</ext>
//</media>
//</media_list>
//</session>

//½âÎösdp ÐèÒª×¢Òâload_string ²ÎÊý2 parse_default
std::vector<simple_media_info_ptr> simple_media_info::parse_media_info_from_xml(const std::string& xml)
{
	vector<simple_media_info_ptr> result;
	xml_document doc;
	xml_parse_result res = doc.load_string(xml.c_str());
	if (res.status == xml_parse_status::status_ok)
	{
		xml_node session = doc.child("session");
		if (session.empty())
			return result;
		int count = 0;
		for (auto x : session.child("media_list"))
		{
			simple_media_info_ptr ptr(new simple_media_info());
			ptr->ssrc = x.attribute("ssrc").as_uint();
			ptr->clock = x.attribute("clock").as_uint();
			ptr->codec = x.attribute("codec").as_string();
			ptr->ext = x.child("ext").child_value();
			ptr->media_index = count++;
			ptr->is_video = v_str_compare_no_case(ptr->codec, "h264") || v_str_compare_no_case(ptr->codec, "mp4") || v_str_compare_no_case(ptr->codec, "mjpeg");
			result.push_back(ptr);
		}
	}
 	return result;
}

std::string simple_media_info::save_simple_media_list_to_xml(const std::vector<std::shared_ptr<simple_media_info>>& media_list, const std::string& sdp)
{
	xml_document doc;
	xml_node root = doc.append_child("session");
	string str_temp;
	root.append_child("sdp").append_child(pugi::xml_node_type::node_cdata).set_value(sdp.c_str());
	xml_node media_list_node = root.append_child("media_list");
	int media_count = media_list.size();
	media_list_node.append_attribute("media_count").set_value(media_count);
	for (auto& x : media_list)
	{
		xml_node media_node = media_list_node.append_child("media");
		media_node.append_attribute("ssrc").set_value(x->ssrc);
		media_node.append_attribute("codec").set_value(x->codec.c_str());
		media_node.append_attribute("clock").set_value(x->clock);
		media_node.append_child("ext").append_buffer(x->ext.c_str(), x->ext.size());
	}
	stringstream ss;
	doc.save(ss);
	return std::move(ss.str());
}

/*
<session>
<sdp><![CDATA[...]]></sdp>
<media_list media_count="1">
	<media ssrc="0" codec="H264" clock="90000" media_index="0">
		<ext sps="Z2QAKKwXKgHgCJ+WEAAAcIAAFfkIQA==" pps="aP44sA=="/>
	</media>
</media_list>
</session>
*/
std::string media_info::save_media_list_to_xml(const vector<media_info_ptr>& media_list, const string& sdp)
{
	//auto sdp = GetSessionInfo("SDP");
	xml_document doc;
	xml_node root = doc.append_child("session");
	xml_node sdp_node = root.append_child("sdp").append_child(pugi::xml_node_type::node_cdata);
	sdp_node.set_value(sdp.c_str());
	xml_node media_list_node = root.append_child("media_list");
	int media_count = media_list.size();
	media_list_node.append_attribute("media_count").set_value((int)media_list.size());
	int count = 0;
	for (auto& x : media_list)
	{
		xml_node media_node = media_list_node.append_child("media");
		media_node.append_attribute("ssrc").set_value(x->ssrc);
		media_node.append_attribute("codec").set_value(x->rtp_map.codec.c_str());
		media_node.append_attribute("clock").set_value(x->rtp_map.clock_rate);
		media_node.append_attribute("media_index").set_value(count);
		if (x->fmtp.payload_type != 0)
		{
			xml_node ext_node = media_node.append_child("ext");
			ext_node.append_attribute("sps").set_value(x->fmtp.sps.c_str());
			ext_node.append_attribute("pps").set_value(x->fmtp.pps.c_str());
		}
		count++;
	}
	stringstream ss;
	doc.save(ss);
	return move(ss.str());
}

media_info::media_info(const std::string& info)
{
	frame_rate = 0;
	ssrc = 0;
	REGEX reg("a=control:(\\S+)");
	SMATCH m;
	if (REGEX_SEARCH(info, m, reg))
	{
		if (m.size() > 1)
		{
			control = m[1].str();
		}
	}
	REGEX reg1("a=rtpmap:([\\S ]+)");
	if (REGEX_SEARCH(info, m, reg1))
	{
		if (m.size() > 1)
		{
			rtp_map.update(m[1].str());
		}
	}
	REGEX reg2("a=fmtp:([\\S\\s]+)\r\n");
	if (REGEX_SEARCH(info, m, reg2))
	{
		if (m.size() > 1)
		{
			fmtp.update(m[1].str());
		}
	}
	REGEX reg3("a=framerate:(\\d+)");
	if (REGEX_SEARCH(info, m, reg3))
	{
		sscanf(m[1].str().c_str(), "%f", &frame_rate);
	}
}

uint32_t rtp_header_t::get_ssrc()
{
	return (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(SSRC);
}

uint32_t rtp_header_t::get_ts()
{
	return (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(RTP_TS);
}

uint16_t rtp_header_t::get_sn()
{
	return boost::asio::detail::socket_ops::network_to_host_short(SN);
}

uint16_t rtsp_interleaved_header::get_size()
{
	return boost::asio::detail::socket_ops::network_to_host_short(size);
}

int rtp_map_info::update(const std::string& map)
{
	REGEX r("(\\d+) (\\S+?)/(\\d+)");
	REGEX r_c("\/(\\d)$");
	SMATCH m;
	if (REGEX_SEARCH(map, m, r))
	{
		if (m.size() > 3)
		{
			pt_code = atoi(m[1].str().c_str());
			codec = m[2].str();
			if (codec == "H264")
			{
				pkt_payload_type = ppt_H264;
			}
			else if (codec == "PCMA")
			{
				pkt_payload_type = ppt_G711A;
			}
			else if (codec == "PCMU")
			{
				pkt_payload_type = ppt_G711U;
			}
			clock_rate = atoi(m[3].str().c_str());
			if (REGEX_SEARCH(map, m, r_c))
			{
				if (m.size() > 1)
				{
					channel_count = atoi(m[1].str().c_str());
				}
			}
			return v_OK;
		}
	}
	return v_ERR_RTSP_BAD_SDP;
}

int fmtp_info::update(const std::string& text)
{
	REGEX reg_pt("(\\d+) (.+)");
	REGEX reg_pm("packetization-mode=(\\d+)");
	REGEX reg_pli("profile-level-id=(\\d+)");
	REGEX reg_sps("sprop-parameter-sets=(.+),(\\S+)[;| |\r\n]");
	//REGEX reg("(\\d+) packetization-mode=(\\d+);profile-level-id=(\\d+);sprop-parameter-sets=(\\S+),(\\S+)");
	SMATCH m, m_pt, m_pm, m_pli, m_sps;
	if (REGEX_SEARCH(text, m, reg_pt))
	{
		if (m.size() > 2)
		{
			sscanf(m[1].str().c_str(), "%d", &payload_type);
			std::string temp = m[2].str();
			if (REGEX_SEARCH(temp, m_pm, reg_pm))
			{
				if (m_pm.size() > 1)
				{
					sscanf(m_pm[1].str().c_str(), "%d", &pack_mode);
				}
			}
			if (REGEX_SEARCH(temp, m_pli, reg_pli))
			{
				if (m_pli.size() > 1)
				{
					sscanf(m_pli[1].str().c_str(), "%d", &profile_level_id);
				}
			}
			if (REGEX_SEARCH(temp, m_sps, reg_sps))
			{
				if (m_sps.size() > 2)
				{
					sps = m_sps[1].str();
					pps = m_sps[2].str();
					sps_bin = base64_dec_bin(sps);
					pps_bin = base64_dec_bin(pps);
				}
			}
		}
	}

	return v_OK;
}
