#ifndef __RTP_DEF_H__
#define __RTP_DEF_H__
#pragma once
#include <memory>
#include <boost/asio.hpp>

#define RTP_HEADER_SIZE 12
typedef struct rtp_over_tcp_header
{
	uint8_t start_code;
	uint8_t channel;
	uint16_t size;
	rtp_over_tcp_header()
	{
		start_code = 0x24;
		channel = 0;
		size = 0;
	}
}rtp_over_tcp_header_t;
typedef struct RTP_Header
{
	uint8_t		CC			: 4;
	uint8_t		Extension	: 1;
	uint8_t		Padding		: 1;
	uint8_t		Version		: 2; 

	uint8_t		PT			: 7;
	uint8_t		Marker		: 1;
	uint16_t	SN;			//16bits
	uint32_t	RTP_TS;
	uint32_t	SSRC;
	RTP_Header(uint8_t payloadType, uint32_t ssrc)
	{
		Version = 2;
		Padding = 0;
		Extension = 0;
		CC = 0;

		Marker = 0;
		PT = payloadType;

		SN = 0;
		RTP_TS = 0;
		set_ssrc(ssrc);
	}
	uint16_t get_sn()
	{
		return boost::asio::detail::socket_ops::network_to_host_short(SN);
	}
	void set_sn(uint16_t sn)
	{
		SN = boost::asio::detail::socket_ops::network_to_host_short(sn);
	}
	uint32_t get_ts()
	{
		return (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(RTP_TS);
	}
	void set_ts(uint32_t ts)
	{
		RTP_TS = (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(ts);
	}

	uint32_t get_ssrc()
	{
		return (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(SSRC);
	}

	void set_ssrc(uint32_t ssrc)
	{
		SSRC = (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(ssrc);
	}
}rtp_header;

typedef struct Playload_Header
{
	uint8_t		type	: 5;
	uint8_t		nri		: 2;
	uint8_t		f		: 1;
}payload_header;

typedef struct FU_Header
{
	uint8_t		type		: 5;
	uint8_t		r			: 1;	// must be 0
	uint8_t		e			: 1;	// end flag
	uint8_t		s			: 1;	// start flag
}fu_header;
typedef struct RTCP_SDES
{
	uint8_t binary[256];
	size_t bin_size = 0;
	string user_name;
	string common_name;
	RTCP_SDES()
	{
		memset(binary, 0, sizeof(binary));
	}

	void set_name(const string& uname, const string& cname)
	{
		memset(binary, 0, sizeof(binary));
		bin_size = 0;
		user_name = uname;
		common_name = cname;
		binary[0] = 1;
		bin_size++;
		binary[bin_size] = user_name.length();
		bin_size++;
		memcpy(binary + bin_size, user_name.c_str(), user_name.length());
		bin_size += user_name.length();
		binary[bin_size] = 2;
		bin_size++;
		binary[bin_size] = common_name.length();
		bin_size++;
		memcpy(binary + bin_size, common_name.c_str(), common_name.length());
		bin_size += common_name.length();
		binary[bin_size] = 0;
		bin_size++;
		/*int rem = bin_size % 4;
		if (rem > 0)
		{
			bin_size += (4 - rem);
		}*/
	}
};
typedef struct RTCP_Header
{
	uint8_t		PCount		: 5;
	uint8_t		Padding		: 1;
	uint8_t		Version		: 2; 

	uint8_t		PType;
	uint16_t	Length;			//16bits
	uint32_t	SenderSSRC;
	RTCP_Header()
	{
		Version = 2;
		Padding = 0;
		PCount = 1;
		PType = 201;
		Length = 0;
		SenderSSRC = 0;
	}
	uint16_t get_length()
	{
		uint16_t tmp = boost::asio::detail::socket_ops::network_to_host_short(Length);

		return (tmp + 1) * 4;
		//return (uint16_t)boost::asio::detail::socket_ops::network_to_host_short(Length);
	}
	void set_length(uint16_t len)
	{
		static int ssrc_size = 4;
		uint16_t temp = (len + ssrc_size) / 4; 
		if (len % 4 > 0)
		{
			temp++;
		}
		Length = (uint16_t)boost::asio::detail::socket_ops::network_to_host_short(temp);
	}
	uint32_t get_ssrc()
	{
		return (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(SenderSSRC);
	}

	void set_ssrc(uint32_t ssrc)
	{
		SenderSSRC = (uint32_t)boost::asio::detail::socket_ops::network_to_host_long(ssrc);
	}
}rtcp_header;

typedef struct RTCP_RR
{
	RTCP_Header heaer;
	uint32_t	resource_ssrc;
	uint32_t	fraction_cumulative;
	uint16_t	sequence_cycle;
	uint16_t	sequence_max;
	uint32_t	interarrival_jitter;
	uint16_t	last_sr_timestamp1;
	uint16_t	last_sr_timestamp2;
	uint32_t	deley_last_sr_timestamp;
	RTCP_Header header_sd;
	uint8_t		sdes[256];
	size_t GetSize()
	{
		int actualsize = sizeof(RTCP_RR) - 256 + header_sd.get_length() - sizeof(RTCP_Header);
		return actualsize;
	}
}rtcp_recvreport;

typedef struct NTPTimestamp
{
	uint32_t ts_msw;
	uint32_t ts_lsw;
}ntp_timestamp;

typedef struct NTPTimestamp_middle
{
	uint16_t ts_ntp;
	uint16_t ts_rtp_mid1;
	uint16_t ts_rtp_mid2;
	uint16_t ts_low;
}ntp_timestamp_mid;
typedef struct RTCP_SR
{
	RTCP_Header header_sr;
	union
	{
		NTPTimestamp ntpTS;
		NTPTimestamp_middle ntpTS_mid;
	};
	uint32_t	rtpTS;
	uint32_t	packetCount;
	uint32_t	octetCount;
	RTCP_Header header_sd;
	uint8_t		sdes[256];
	size_t GetSize()
	{
		int actualsize = sizeof(RTCP_SR) - 256 + header_sd.get_length() - sizeof(RTCP_Header);
		return actualsize;
	}
	
	static uint64_t get_ntp_ts()
	{
		int64_t time = std::chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
		static uint64_t n = (uint64_t)pow(10, 9);
		uint64_t ns = time % n;
		uint64_t s = time / n;
		uint64_t t = ns << 32;
		t /= 1000000000;
		t |= ((70LL * 365 + 17) * 24 * 60 * 60 + s) << 32;
		return t;
	}
}rtcp_sendreport;


struct rtcp_sdes_packet
{
	RTCP_Header header;
	RTCP_SDES body;
	//rtcp_sdes_packet()
	//{
	//	header.set_ssrc(0);
	//	header.PType = 200; //Source Description
	//}
	void update(uint32_t ssrc, const string& uname, const string& cname)
	{
		header.set_ssrc(ssrc);
		header.PType = 202; //Source Description
		header.PCount = 1;
		body.set_name(uname, cname);
		header.set_length(body.bin_size);
	}
	size_t get_length()
	{
		return header.get_length();
	}
	static uint64_t get_ntp_ts()
	{
		int64_t time = std::chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
		static uint64_t n = (uint64_t)pow(10, 9);
		uint64_t ns = time % n;
		uint64_t s = time / n;
		uint64_t t = ns << 32;
		t /= 1000000000;
		t |= ((70LL * 365 + 17) * 24 * 60 * 60 + s) << 32;
		return t;
	}
};
struct rtcp_sr_packet
{
	RTCP_Header header_sr;
	union
	{
		NTPTimestamp ntp_ts;
		NTPTimestamp_middle ntp_ts_mid;
	};
	uint32_t	rtp_ts;
	uint32_t	packet_count;
	uint32_t	octet_count;
	rtcp_sdes_packet sdes;
	void update(uint32_t ssrc, uint64_t nts, uint32_t rts, uint32_t packetCount, uint32_t bytesCount, const string& uname, const string& cname)
	{
		header_sr.PCount = 0;
		header_sr.PType = 200;
		header_sr.set_ssrc(ssrc);
		header_sr.set_length(20); // (ssrc + ntp_ts + rtp_ts + pc + oc) / 4 = 24 / 4 = 6
		//int64_t ntpts = v_get_timestamp_seconds();
		ntp_ts.ts_msw = boost::asio::detail::socket_ops::host_to_network_long(nts >> 32);
		ntp_ts.ts_lsw = boost::asio::detail::socket_ops::host_to_network_long(nts & 0xffffffff);
		rtp_ts = boost::asio::detail::socket_ops::host_to_network_long(rts);
		packet_count = boost::asio::detail::socket_ops::host_to_network_long(packetCount);
		octet_count = boost::asio::detail::socket_ops::host_to_network_long(bytesCount);
		
		sdes.update(ssrc, uname, cname);
	}
	
	size_t get_length()
	{
		int actualsize = sizeof(RTCP_Header) + 8 + 4 + 4 + 4 + sdes.get_length();
		return actualsize;
	}

	static uint64_t get_ntp_ts()
	{
		int64_t time = std::chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
		static uint64_t n = (uint64_t)pow(10, 9);
		uint64_t ns = time % n;
		uint64_t s = time / n;
		uint64_t t = ns << 32;
		t /= 1000000000;
		t |= ((70LL * 365 + 17) * 24 * 60 * 60 + s) << 32;
		return t;
	}
};

struct rtcp_packet
{
	RTCP_SR sr;
	rtcp_sdes_packet sdes;

};
//template <typename Handler>
//class RTP_Alloc_Handler
//{
//private:
//	Handler m_handler;
//	RTP_Buffer_Manager& m_alloc_host;
//public:
//	RTP_Alloc_Handler(RTP_Buffer_Manager& host, Handler handler)
//		: m_alloc_host(host), m_handler(handler)
//	{
//
//	}
//
//	template <typename Arg1, typename Arg2>
//	void operator()(Arg1 arg1, Arg2 arg2)
//	{
//		m_handler(arg1, arg2);
//	}
//};
//template <typename Handler>
//inline RTP_Alloc_Handler<Handler> make_custom_alloc_handler(
//	RTP_Buffer_Manager& a, Handler h)
//{
//	return RTP_Alloc_Handler<Handler>(a, h);
//}

#endif