#ifndef record_defs_h__
#define record_defs_h__
#pragma once
#include "vistek_error_code.h"
#include "vistek_media_defs.h"
#include "vistek_time_util.h"
#include "vistek_file_util.h"
using namespace std;

#define MAX_STREAM_COUNT 4

typedef struct record_index_entity
{
	int media_index;
	int64_t frame_ts;
	int64_t frame_pos;
	int frame_size;
	int key_frame_size;
	int file_index;
	record_index_entity()
	{
		media_index = 0;
		frame_ts = 0;
		frame_pos = 0;
		frame_size = 0;
		key_frame_size = 0;
		file_index = 0;
	}
}record_index_entity_t;
struct record_frame_object
{
	record_index_entity rie;
	vector<uint8_t> buffer;
};
typedef shared_ptr<record_index_entity> record_index_entity_ptr;
typedef vector<record_index_entity> record_index_entity_list;
typedef shared_ptr<record_index_entity_list> record_index_list_ptr;
typedef struct record_file_entity
{
	int file_index;
	int media_index;
	string file_name;
	int64_t begin_ts;
	int64_t end_ts;
	record_file_entity()
		: file_index(-1), media_index(-1), file_name(""), begin_ts(0), end_ts(0)
	{

	}
	record_file_entity(int mi, const string& fn, int64_t ts1, int64_t ts2)
		: file_index(-1), media_index(mi), file_name(fn), begin_ts(ts1), end_ts(ts2)
	{

	}
};
typedef shared_ptr<record_file_entity> record_file_entity_ptr;
typedef vector<record_file_entity_ptr> record_file_entity_list;
//typedef struct media_info_entity
//{
//	int media_index;
//	int media_ssrc;
//	string codec_name;
//	int clock_rate;
//	bool is_video()
//	{
//		if (codec_name == "h264" || codec_name == "H264")
//		{
//			return true;
//		}
//		return false;
//	}
//};
//typedef shared_ptr<media_info_entity> media_info_entity_ptr;

struct record_map
{
	bool support_reverse_play;
	string manufacture;
	vector<simple_media_info_ptr> media_info_list;
	map<int, record_file_entity_list> media_file_map;
	map<int, record_index_list_ptr> media_index_map;
	record_map()
		: support_reverse_play(false), manufacture("vistek")
	{

	}
	bool check_media_index(int mi)
	{
		if (mi >= 0 && mi < media_index_map.size())
		{
			return true;
		}
		return false; 
	}
	int get_media_count()
	{
		return media_index_map.size();
	}
	int64_t get_min_ticks()
	{
		if (!media_index_map.empty())
		{
			if (!media_index_map[0]->empty())
			{
				return media_index_map[0]->front().frame_ts;
			}
		}
		return 0;
	}
	int64_t get_max_ticks()
	{
		if (!media_index_map.empty())
		{
			if (!media_index_map[0]->empty())
			{
				return media_index_map[0]->back().frame_ts;
			}
		}
		return 0;
	}
	int get_filename(int mi, int64_t ts, string& filename)
	{
		auto iter = media_file_map.find(mi);
		if (iter != media_file_map.end())
		{
			// nvr录像通常不分割文件 当只有一个文件时 认为是nvr
			if (iter->second.size() == 1)
			{
				filename = iter->second.front()->file_name;
				return v_OK;
			}
			else
			{
				for (auto f : iter->second)
				{
					if (ts >= f->begin_ts && ts <= f->end_ts)
					{
						filename = f->file_name;
						return v_OK;
					}
				}
			}
			
		}
		return v_ERR_FILE_NOT_FOUND;
	}
};
typedef shared_ptr<record_map> record_map_ptr;

struct vod_result
{
	string file_name;
	boost::posix_time::ptime start_time;
	boost::posix_time::ptime end_time;
	uint64_t filesize;
};

struct nvr_source_info
{
	string source_name;
	int storage_mode;
	int64_t storage_param;
	string storage_schedule;
	int is_active;
	double actual_priority;
	nvr_source_info()
		: source_name(""), storage_mode(0), storage_param(0), storage_schedule(""), is_active(0), actual_priority(0)
	{

	}
};
struct nvr_record_info
{
	int64_t record_id;
	string source_name;
	string path_name;
	string file_name;
	string start_time;
	string end_time;
	int64_t file_size;
	int64_t priority;
	double duration;
	mutable string m_full_path;
	nvr_record_info()
		: record_id(0), file_size(0), priority(100), duration(0)
	{

	}

	const string& full_path() const
	{
		if (m_full_path == "")
		{
			if (path_name == "")
			{
				m_full_path = file_name;
			}
			else
			{
				boost::filesystem::path p(path_name);
				p /= file_name;
				m_full_path = p.string();
			}
		}
		return m_full_path;
	}
	static bool compare_little(const nvr_record_info& nri1, const nvr_record_info& nri2)
	{
		return nri1.start_time < nri2.start_time;
	}
	static bool compare_little_p(const shared_ptr<nvr_record_info>& rip1, const shared_ptr<nvr_record_info>& rip2)
	{
		if (rip1 == 0 || rip2 == 0)
		{
			if (rip1 == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		return rip1->start_time < rip2->start_time;
	}
	
};
struct nvr_record_fragment_info
{
	string source_name;
	int64_t start_time;
	int64_t end_time;
};

typedef shared_ptr<nvr_source_info> source_info_ptr;
typedef shared_ptr<nvr_record_info> record_info_ptr;
typedef vector<record_info_ptr> Record_List;
typedef shared_ptr<Record_List> record_list_ptr;
typedef pair<string, string> record_gap;
typedef shared_ptr<record_gap> record_gap_ptr;
typedef shared_ptr<list<record_gap_ptr>> record_gap_plist;

typedef enum vistek_record_mode
{
	vistek_fast_record,
	vistek_file_record
};
typedef struct vistek_storage_service_config
{
	string db_home;
	string storage_dir;
	string record_extension;
	vector<pair<string, double>> volume_list;
}vistek_storage_service_config_t;

#endif // record_defs_h__