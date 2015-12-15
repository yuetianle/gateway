#ifndef _HIKVISION_DEVICE_TYPES_H
#define _HIKVISION_DEVICE_TYPES_H
#pragma  once
#include <string.h>
using namespace std;
#pragma region NODE
#ifndef HIK_NODE_STREAM_URL_LISTS_NAME
#define HIK_NODE_STREAM_URL_LISTS_NAME "stream_url_lists"
#endif
#ifndef HIK_NODE_STREAM_URL_NAME
#define HIK_NODE_STREAM_URL_NAME "stream_url"
#endif
#ifndef HIK_NODE_STREAM_URL_CONTENT_NAME
#define HIK_NODE_STREAM_URL_CONTENT_NAME "stream_url_content"
#endif
#ifndef HIK_NODE_IP_NAME
#define HIK_NODE_IP_NAME "ip"
#endif
#ifndef HIK_NODE_PORT_NAME 
#define HIK_NODE_PORT_NAME "port"
#endif
#ifndef HIK_NODE_CHANNEL_NAME
#define HIK_NODE_CHANNEL_NAME "channel"
#endif
#ifndef HIK_NODE_STREAM_TYPE_NAME
#define HIK_NODE_STREAM_TYPE_NAME "stream_type"
#endif
#ifndef HIK_NODE_CODEC_NAME
#define HIK_NODE_CODEC_NAME "codec"
#endif

#ifndef HIK_NODE_STATUS_NAME
#define HIK_NODE_STATUS_NAME "device_status"
#endif


#pragma endregion NODE
#pragma region NODE_ATTR
#ifndef HIK_STATUS_NODE_ATTR_STATUS_NAME
#define HIK_STATUS_NODE_ATTR_STATUS_NAME "status"
#endif
#ifndef HIK_STATUS_NODE_ATTR_CHANNEL_NAME
#define HIK_STATUS_NODE_ATTR_CHANNEL_NAME "channel"
#endif
#pragma endregion NODE_ATTR

#pragma region STREAM_URL_DEFINES

typedef struct hstream_info
{
	string ip;
	int rtsp_port;/*<< д╛хо554*/
	int channel;
	int stream_type;/*<< 0:main 1:sub */
	int codec_type;/*<< 0:h264 1:jpeg */
	hstream_info()
		:rtsp_port(554), channel(1), stream_type(0), codec_type(0)
	{
	}
}HIKStreamInfo;
typedef vector<HIKStreamInfo> HIKStreamInfoVec;

#pragma endregion STREAM_URL_DEFINES
#endif