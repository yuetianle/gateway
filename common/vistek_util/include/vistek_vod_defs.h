#ifndef vistek_vod_defs_h__
#define vistek_vod_defs_h__


#include <string>
#include "boost/signals2.hpp"
#include <memory>
#include <functional>
#define VISTEK_MFX_MAKEFOURCC(A,B,C,D)    ((((int)A))+(((int)B)<<8)+(((int)C)<<16)+(((int)D)<<24))
#include "pre_boost_basic.h"

typedef enum vsdk_play_direction
{
	vpd_Forward,
	vpd_Backward
}vsdk_play_direction_t;
struct fetch_options
{
	vsdk_play_direction direction;
	int keyframe_only_flag;
};
typedef enum record_open_mode
{
	record_open_none = 0,
	record_open_read = 1,
	record_open_write = 2,
	record_open_ate = 4
}record_open_mode_t;

typedef enum vsdk_picture_formats
{
	vpf_None,
	vpf_BMP,
	vpf_PNG,
	vpf_GIF,
	vpf_JPEG2000,
	vpf_JPEGLS,
	vpf_TIFF,
	vpf_RAW_YUV420P,
	vpf_RAW_RGB565,
	vpf_RAW_BGRA,
	vpf_RAW_BGR24,
	vpf_RAW_H264IFrame
}vsdk_picture_formats_t;
//typedef enum { vset_BMP, vset_PNG, vset_JPEG } video_slice_encoding_type;

struct video_slice_context
{
	int slice_w = 0;
	int slice_h = 0;
	int slice_num = 0;
	vsdk_picture_formats img_fmt = vpf_PNG;
	vsdk_play_direction direction;
	std::string time_begin = "";
	std::string time_end = "";
	double pos = 0;
	int64_t current_ts = 0;
	std::string ip = "";
	int port = 0;
	std::string did = "";
	int channel = 0;
	int streamIndex = 0;
	std::string userID;
	int userLevel = 0;
};

typedef struct playback_status
{
	int fps_den;
	int fps_num;
	double fetch_pos = 0;
	double pos = 0;
	double seek_pos = 0;
	double speed = 1;
	double last_speed = 0;
	int64_t play_timestamp;
	int64_t fetch_timestamp;
	bool is_recording;
	bool is_paused;
	vsdk_play_direction fetch_dir;
	vsdk_play_direction last_fetch_dir;
	playback_status()
	{
		is_paused = false;
		is_recording = false;
		fps_den = 1;
		fps_num = 25;
		pos = 0;
		seek_pos = -1;
		speed = 1;
		last_speed = 1;
		play_timestamp = 0;
		fetch_timestamp = 0;
		fetch_dir = vpd_Forward;
		last_fetch_dir = vpd_Forward;
	}
}playback_status_t;

typedef enum class vistek_media_type
{
	MFX_CODEC_AVC = VISTEK_MFX_MAKEFOURCC('A', 'V', 'C', ' '),
	MFX_CODEC_MPEG2 = VISTEK_MFX_MAKEFOURCC('M', 'P', 'G', '2'),
	MFX_CODEC_VC1 = VISTEK_MFX_MAKEFOURCC('V', 'C', '1', ' ')
} vistek_media_type_t;

typedef enum decoder_type
{
	DEC_NONE = 0,
	DEC_FF_SOFT = 1,
	DEC_INTEL = 2,
	DEC_DXVA = 4,
	DEC_HIK = 256,
	DEC_DH
}decoder_type_t;

typedef enum vistek_media_transfer_type
{
	v_By_ICE = 0,
	v_BY_RTP,
	v_BY_TCP,
	v_BY_RTP_OVER_TCP
}vistek_media_transfer_type_t;

typedef enum session_status
{
	v_SESSION_DEAD = -1,
	v_SESSION_READY = 0,
	v_SESSION_PLAY = 1,
}session_status_t;

typedef enum vistek_play_status
{
	v_PLAY_NONE = 0,
	v_PLAY_READY = 1,
	v_PLAY_START = 2,
	v_PLAY_PAUSE = 4
}vistek_play_status_t;


typedef enum vistek_decode_stream_mode
{
	v_STREAM_MODE,
	v_FRAME_MODE
}vistek_decode_stream_mode_t;



typedef enum vistek_codec_type
{
	H264 = 0,
	MJPEG = 1,
	Other
}vistek_codec_type_t;

typedef enum vsdk_frame_data_type
{
	fdt_NONE = 0,
	fdt_RAW = 1,
	fdt_D3DSurface9 = 2,
	fdt_YV12 = 4,
	fdt_IYUV = 8
}vsdk_frame_data_type_t;

typedef struct vsdk_frame_data
{
	int width;
	int height;
	int format;
	int buffer_size;
	int fps_num;
	int fps_den;
	int bit_rate;
	vsdk_frame_data_type framedata_type;
	int64_t time_stamp;
	void* data_ptr;
}vsdk_frame_data_t;


typedef struct vsdk_raw_frame_data
{
	std::vector<uint8_t> buffer;
	uint8_t *data[4];
	int linesize[4];
	int pix_fmt = 0;
	int pix_fmt_extra = 0;
	int width = 0;
	int height = 0;
	int buffer_size = 0;
	uint8_t** get_data()
	{
		return data;
	}
	uint8_t* buf_ptr()
	{
		return buffer.data();
	}
	int capacity()
	{
		return buffer.size();
	}
	void reserve(int size)
	{
		buffer.resize(size);
	}
	vsdk_raw_frame_data()
		: buffer_size(0), buffer(0)
	{
		
		memset(data, 0, sizeof(data));
		memset(linesize, 0, sizeof(linesize));
	}
	vsdk_raw_frame_data(int buff_size)
		: buffer_size(buff_size), buffer(buff_size)
	{

		memset(data, 0, sizeof(data));
		memset(linesize, 0, sizeof(linesize));
	}
}vsdk_raw_frame_data_t;

typedef void* vsdk_handle;
#ifdef WIN32
//typedef void* (*Frame_Data_Output)(vistek_handle streamID, vistek_media_sample* vms, void* pUserData);
#else
//typedef void* __attribute__((__cdecl__))Frame_Data_Output(vistek_handle streamID, frame_data_t* fi);
//typedef void* (*Frame_Data_Output)(vistek_handle streamID, frame_data_t* fi);
#endif
enum vsdk_media_type_main
{
	mtm_NONE = 0,
	mtm_VIDEO = 1,
	mtm_AUDIO = 2
};

enum vsdk_media_type_sub
{
	mts_NONE = 0,
	mts_RTP = 1,
	mts_RTPTCP = 2,
	mts_EncodedData = 4,
	mts_DecodedData = 8,
	mts_Image = 16,
	mts_Window = 32
};
typedef struct vsdk_image_data
{
	int size;
	int width;
	int height;
	int picType;
	int64_t timestamp;
	union
	{
		double pos;
		uint8_t reserved[128];
	}extra;
	uint8_t* data;
}vsdk_image_data_t;


typedef struct vsdk_encoded_data
{
	uint8_t* buffer;
	size_t buffer_size;
	int payload_type;
}vsdk_encoded_data_t;



typedef struct vsdk_sample_raw
{
	uint8_t* buffer;
	size_t buffer_size;
}vsdk_sample_raw_t;

typedef struct vsdk_frame_h264
{
	uint8_t* buffer;
	size_t buffer_size;
	int frame_type;
	int fps_num;
	int fps_den;
	int64_t timestamp;
}vsdk_frame_h264_t;

typedef struct vsdk_audio_data
{
	uint8_t* buffer;
	size_t buffer_size;
	int sample_rate;
	int sample_bits;
	int channel_count;
}vsdk_audio_data_t;


typedef struct vsdk_media_sample
{
	vsdk_media_type_main _main_type;
	vsdk_media_type_sub _sub_type;
	union
	{
		vsdk_frame_data _frame_data;
		vsdk_audio_data _audio_data;
		vsdk_image_data _image_data;
		vsdk_sample_raw _raw_data;
		vsdk_frame_h264 _h264_data;
		uint8_t _reserved[256];
	}_sample;
	int _payload_type;
	union
	{
		uint32_t _ps_rate;
		int64_t _pts;
		uint8_t _reserved[248];
	}_extra;
}vsdk_media_sample_t;

typedef void*(*vsdk_media_sample_callback_fp)(vsdk_handle streamID, vsdk_media_sample* fi, void* userData);
typedef boost::signals2::signal<void*(vsdk_handle streamID, vsdk_media_sample* frame_info, void* pUserData)> vsdk_media_sample_signal;
typedef vsdk_media_sample_signal::slot_function_type vsdk_media_sample_callback;


typedef enum vistek_renderer_type
{
	vmr_BMP,
	vmr_INTEROP_BMP,
	vmr_D3D,
	vmr_SDL
}vistek_renderer_type_t;
typedef std::function<void(int, uint8_t*, int, int, int, vistek_renderer_type)> func_render_callback;


typedef struct media_shared_info
{
	char frame_data_shared_name[128];
	vsdk_frame_data_t frame_data;
}media_shared_info_t;


#ifdef WIN32
typedef void(__cdecl * Render_Callback)(int slot_id, uint8_t* buffer, int w, int h, int pixfmt, vistek_renderer_type rt);
#else
typedef void(*Render_Callback)(int slot_id, uint8_t* buffer, int w, int h, int pixfmt, vistek_renderer_type rt);
#endif

typedef std::pair<int64_t, int64_t> vistek_time_slice; // from 2000-01-01 00:00:00
typedef std::vector<vistek_time_slice> vistek_time_slice_list;
typedef std::shared_ptr<vistek_time_slice_list> vistek_time_slice_list_ptr;
typedef enum vistek_playback_command
{
	vpc_FetchNext,
	vpc_FetchPrevious
}vistek_playback_command_t;

typedef boost::signals2::signal<void(double pos)> play_pos_notify_signal;
typedef boost::signals2::signal<void(const std::string& pos)> play_time_notify_signal;

#ifdef WIN32
typedef void(__cdecl * fp_img_encoding_callback)(uint8_t* buffer, int size, const char* param, void* userData);
#else
typedef void(* fp_img_encoding_callback)(uint8_t* buffer, int size, void* param, void* userData);
#endif
typedef std::function<void(uint8_t* buffer, int size, const char* param, void* userData)> fn_img_encoding_callback;

#ifdef WIN32
typedef void(__cdecl * fp_video_slice_callback)(void* hdl, vsdk_image_data* img, double pos, void* pUserData, int reserved);
#else
typedef void(*fp_video_slice_callback)(void* hdl, uint8_t* buffer, int size, int w, int h, int picType, double pos, int64_t timestamp, int reserved);
#endif
typedef std::function<void(void* hdl, vsdk_image_data* img, double pos, void* pUserData, int reserved)> video_slice_callback;

#include "vistek_record_defs.h"

typedef std::tuple<fetch_options&, std::vector<record_frame_object>&, double&> fetch_parameter;
/// definations for playback source
#endif // vistek_vod_defs_h__
