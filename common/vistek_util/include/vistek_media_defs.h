#ifndef vistek_media_defs_h__
#define vistek_media_defs_h__

#pragma once
#include "base64.h"

#include "vistek_string_util.h"
typedef enum packet_transport_type
{
	NONE,
	UDP,
	TCP,
	RTP,
	RTPOVERTCP,
	RTPOVERHTTP
};

typedef enum packet_payload_type
{
	ppt_UNKNOWN = 0,
	ppt_H264 = 1,
	ppt_MJPEG,
	ppt_H263,
	ppt_G711U = 400,
	ppt_G711A = 400 + 8,
	ppt_G722 = 400 + 9,
	ppt_G728 = 400 + 15,
	ppt_G729 = 400 + 18,
	ppt_STRING = 65536
};
typedef struct rtp_header_t
{
	uint8_t		CC : 4;
	uint8_t		Extension : 1;
	uint8_t		Padding : 1;
	uint8_t		Version : 2;

	uint8_t		PT : 7;
	uint8_t		Marker : 1;
	uint16_t	SN;			//16bits
	uint32_t	RTP_TS;
	uint32_t	SSRC;
	uint16_t get_sn();
	uint32_t get_ts();
	uint32_t get_ssrc();
};

typedef struct
{
	uint8_t		type : 5;
	uint8_t		r : 1;	// must be 0
	uint8_t		e : 1;	// end flag
	uint8_t		s : 1;	// start flag
}fu_header_t;

typedef struct payload_header_t
{
	uint8_t		type : 5;
	uint8_t		nri : 2;
	uint8_t		f : 1;
	fu_header_t	fu;
};

typedef struct rtsp_interleaved_header
{
	uint8_t magic;
	uint8_t channel;
	uint16_t size;
	rtp_header_t rtp_header;
	uint16_t get_size();
};

struct rtp_map_info
{
	int pt_code;
	packet_payload_type pkt_payload_type;
	int clock_rate;
	std::string codec;
	int channel_count;
	rtp_map_info()
		: pt_code(0), pkt_payload_type(ppt_UNKNOWN), clock_rate(0), codec(""), channel_count(1)
	{

	}
	int update(const std::string& map);


};
struct fmtp_info
{
	int payload_type;
	int pack_mode;
	int profile_level_id;
	std::string sps;
	std::string pps;
	std::vector<uint8_t> sps_bin;
	std::vector<uint8_t> pps_bin;
	
	fmtp_info()
		: payload_type(0), pack_mode(0), profile_level_id(0), sps_bin(0), pps_bin(0)
	{
	}
	int update(const std::string& text);
};
struct simple_media_info
{
	bool is_video;
	int media_index;
	uint32_t ssrc;
	uint32_t clock;
	std::string codec;
	std::string ext;
	simple_media_info()
		: media_index(0), ssrc(0), clock(90000), codec(""), ext("")
	{

	}
	static std::vector<std::shared_ptr<simple_media_info>> parse_media_info_from_xml(const std::string& xml);
	static std::string save_simple_media_list_to_xml(const std::vector<std::shared_ptr<simple_media_info>>& media_list, const std::string& sdp);
};
typedef std::shared_ptr<simple_media_info> simple_media_info_ptr;
struct media_info
{
	static std::vector<std::shared_ptr<media_info>> parse_media_info_from_sdp(const std::string& sdp);
	static std::string save_media_list_to_xml(const std::vector<std::shared_ptr<media_info>>& media_list, const std::string& sdp);

	std::string control;
	rtp_map_info rtp_map;
	fmtp_info fmtp;
	float frame_rate;
	uint32_t ssrc;
	int payload_type = 0;
	media_info(const std::string& info);
};
typedef std::shared_ptr<media_info> media_info_ptr;

#endif // vistek_media_defs_h__
