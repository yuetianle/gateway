#ifndef _BASE_DEVICE_MEDIA_H_
#define _BASE_DEVICE_MEDIA_H_
#pragma once
#include "base_device_data_types_defs.h"
class base_device;
class base_device_media
{
public:
	base_device_media(base_device *device);
	~base_device_media();
	/*
	 * @brief 获取实时视频流播放地址.
	 * @param [in] stream_id:码流的ID.
	 * @return success:0 else return error_code.
	 */
	virtual int get_stream_urls(const int &stream_id, char *out_data, long *out_len) = 0;
protected:
	data_cache_ex<int, string> m_video_stream_url_lists;/*<< <stream_id:url>*/
	base_device *m_device;
};
#endif

