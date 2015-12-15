
#include "vistek_vod_defs.h"
#include "vistek_signal.h"

typedef struct vsdk_play_handle
{
	void* _hwnd;
	vsdk_media_sample_callback_fp _sample_callback;
	vsdk_message_callback_fp _msg_callback;
	void* _user_data;
}vistek_play_handle_t;

typedef struct vsdk_stream_info
{
	char ip[64];
	int port;
	char did[64];
	int channel;
	int streamIndex;
	char userID[128];
	int userLevel;
}vsdk_stream_info_t;