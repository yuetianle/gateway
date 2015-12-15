#include "stdafx.h"
#include "hikvision_device_ipc.h"

hikvision_device_ipc::hikvision_device_ipc(const string &device_id, const int &device_type, const string &device_name, const string &device_manufacture, const string &device_ip, const long &device_port, const string &user_name, const string &user_pwd)
	:hikvision_common_device(device_id, device_type, device_name, device_manufacture, device_ip, device_port, user_name, user_pwd)
{
}

hikvision_device_ipc::~hikvision_device_ipc()
{
}

int hikvision_device_ipc::get_device_status(char *out_data, long *out_len)
{
	switch (m_handle_info.handle_type)
	{
	case HANDLE_L:
	{
		NET_DVR_TIME ret[1] = { 0 };
		DWORD sz = 0;
		if (NET_DVR_GetDVRConfig(m_handle_info.handle.lhandle, NET_DVR_GET_TIMECFG, 0, ret, sizeof(NET_DVR_TIME), &sz))
		{
			_createStatusXml(true, out_data, out_len);
		}
		else
		{
			_createStatusXml(false, out_data, out_len);
		}
	}
		break;
	default:
		break;
	}
	return v_OK;
}

int hikvision_device_ipc::get_device_info(char *out_data, long *out_len)
{
	return v_OK;
}

int hikvision_device_ipc::_createStatusXml(const bool &status, char *out_data, long *out_size)
{
	pugi::xml_document doc;
	pugi::xml_node status_node = doc.append_child(HIK_NODE_STATUS_NAME);
	pugi::xml_attribute status_attr = status_node.append_attribute(HIK_STATUS_NODE_ATTR_STATUS_NAME);
	status_attr.set_value(status);
	stringstream ss_out;
	doc.save(ss_out);
	if (*out_size < ss_out.str().size() && 0 < *out_size)
	{
		memcpy(out_data, ss_out.str().c_str(), *out_size);
	}
	else
	{
		memcpy(out_data, ss_out.str().c_str(), ss_out.str().size());
		*out_size = ss_out.str().size();
	}
	return v_OK;

}

base_device_media* hikvision_device_ipc::create_media_service()
{
	if (m_device_media)
	{
		return m_device_media;
	}
	else
	{
		auto ptr = new hikvision_device_media(this);
		if (ptr)
		{
			m_device_media = ptr;
			return ptr;
		}
		else
		{
			return nullptr;
		}
	}
}

int hikvision_device_ipc::add_user(const char *user, const char *password, const int &level /*= 0*/)
{
	if (!user || !password)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	switch (m_handle_info.handle_type)
	{
	case HANDLE_L:
	{
		long handle = m_handle_info.handle.lhandle;
		NET_DVR_USER_V40 hik_user_info = {0};
		int hik_user_index = -1;
		if (v_OK != _check_user_exist(user, &hik_user_info, &hik_user_index))
		{
			int user_count = hik_user_info.dwSize / sizeof(NET_DVR_USER_INFO_V40);
			int user_index = 0;
			while (user_index < user_count)
			{
				string tmp_name = (char*)hik_user_info.struUser[user_index].sUserName;
				if (tmp_name.empty())
				{
					strncpy((char*)hik_user_info.struUser[user_index].sUserName, user, MAX_NAME_LEN);
					strncpy((char*)hik_user_info.struUser[user_index].sPassword, password, PASSWD_LEN);
					break;
				}
				user_index++;
			}
			if (NET_DVR_SetDVRConfig(handle, NET_DVR_SET_USERCFG_V40, 0, &hik_user_info, sizeof(hik_user_info)))
			{
				return v_OK;
			}
		}
	}
	break;
	case HANDLE_I:
	{
	}
	break;
	case HANDLE_VOID:
	{
	}
	break;
	default:
		break;
	}
	return v_ERR_BAD_ARGUMENT;
}

int hikvision_device_ipc::delete_user(const char *user)
{
	if (!user)
	{
		return v_ERR_BAD_ARGUMENT;
	}
	switch (m_handle_info.handle_type)
	{
	case HANDLE_L:
	{
		long handle = m_handle_info.handle.lhandle;
		NET_DVR_USER_V40 hik_user_info = {0};
		int hik_user_index = -1;
		if (v_OK == _check_user_exist(user, &hik_user_info, &hik_user_index))
		{
			if (0 <= hik_user_index && hik_user_index < MAX_USERNUM_V30)
			{
				memset(&hik_user_info.struUser[hik_user_index], 0, sizeof(NET_DVR_USER_INFO_V40));
			}
			if (NET_DVR_SetDVRConfig(handle, NET_DVR_SET_USERCFG_V40, 0, &hik_user_info, sizeof(NET_DVR_USER_V40)))
			{
				return v_OK;
			}
		}
	}
	break;
	case HANDLE_I:
	{
	}
	break;
	case HANDLE_VOID:
	{
	}
	break;
	default:
		break;
	}
	return v_ERR_BAD_ARGUMENT;
}

int hikvision_device_ipc::alter_user(const char *old_name, const char *new_name, char *password, const int &level /* = 0 */)
{
	return v_OK;
}

int hikvision_device_ipc::_check_user_exist(const char *user, NET_DVR_USER_V40 *hik_user_info, int *hik_user_index)
{
	if (!user || !hik_user_info || !hik_user_index)
	{
		return -1;
	}
	else
	{
		DWORD real_user_len = 0;
		if (NET_DVR_GetDVRConfig(m_handle_info.handle.lhandle, NET_DVR_GET_USERCFG_V40, 0, hik_user_info, sizeof(NET_DVR_USER_V40), &real_user_len))
		{
			for (auto user_index = 0; user_index < MAX_USERNUM_V30; user_index++)
			{
				if (v_str_compare_no_case((char*)hik_user_info->struUser[user_index].sUserName, user))
				{
					*hik_user_index = user_index;
					return 0;
				}
			}
		}
	}
	return -1;
}





hikvision_device_media::hikvision_device_media(hikvision_device_ipc *device)
	:base_device_media(device)
{

}

int hikvision_device_media::get_stream_urls(const int &stream_id, char *out_data, long *out_len)
{
	if (m_device)
	{
		if (!out_len || !out_data)
		{
			return v_ERR_BAD_ARGUMENT;
		}
		auto ptr = dynamic_cast<hikvision_device_ipc*>(m_device);
		switch (ptr->get_login_handle().handle_type)
		{
		case HANDLE_I:
			break;
		case HANDLE_L:
		{
			pugi::xml_document in_doc;
			pugi::xml_node decl = in_doc.prepend_child(pugi::node_declaration);
			decl.append_attribute("version") = "1.0";
			decl.append_attribute("encoding") = "UTF-8";
			pugi::xml_node encode_info = in_doc.append_child("AudioVideoCompressInfo");
			pugi::xml_node channel_node = encode_info.append_child("VideoChannelNumber");
			channel_node.set_value("1");
			stringstream hik_in_xml;
			in_doc.save(hik_in_xml);
#ifdef _DEBUG
			in_doc.save_file("hik_in.xml");
#endif
			const long stream_out_xml_size = 2 * 1024 * 1024;
			char *stream_out_xml = new char[stream_out_xml_size];
#ifdef _DEBUG
			//pugi::xml_document tmp_doc;
			//tmp_doc.load_string(stream_out_xml);
			//tmp_doc.save_file("hik_out.xml");
#endif
			long handle = ptr->get_login_handle().handle.lhandle;
			try
			{
				if (NET_DVR_GetDeviceAbility(handle, DEVICE_ENCODE_ALL_ABILITY_V20, const_cast<char*>(hik_in_xml.str().c_str()), hik_in_xml.str().size(), stream_out_xml, stream_out_xml_size))
				{
					HIKStreamInfoVec stream_info_vec;
					if (v_OK == _parser_hik_stream_xml(stream_out_xml, &stream_info_vec))
					{
						_create_stream_url(out_data, out_len, stream_info_vec, stream_id);
						if (0 < strlen(out_data))
						{
							return v_OK;
						}
					}
				}
				else
				{
					vvlog_e("GetDeviceAbility fail, " << hikvision_device_ipc::get_hik_error_info());
					return v_ERR_Bad_XML;
				}
			}
			catch (exception *e)
			{
				cout << "exception:" << e->what() << endl;
			}
			
		}
		break;
		case HANDLE_VOID:
			break;
		case HANDLE_UNKOWN:
		break;
		default:
			break;
		}
	}
	return v_ERR_Bad_XML;
}

int hikvision_device_media::_parser_hik_stream_xml(const char *in_xml, HIKStreamInfoVec *stream_info)
{
	pugi::xml_document doc;
	pugi::xml_parse_result xml_ret = doc.load_string(in_xml);
	if (pugi::xml_parse_status::status_ok == xml_ret.status)
	{
		int rtsp_port = 0;
		NET_DVR_RTSPCFG rtsp_info;
		memset(&rtsp_info, 0, sizeof(NET_DVR_RTSPCFG));
		auto ptr = dynamic_cast<hikvision_device_ipc*>(m_device);
		if (NET_DVR_GetRtspConfig(ptr->get_login_handle().handle.lhandle, 0, &rtsp_info, sizeof(NET_DVR_RTSPCFG)))
		{
			rtsp_port = rtsp_info.wPort;
		}
		pugi::xml_node url_node = doc.child("AudioVideoCompressInfo").child("VideoCompressInfo").child("ChannelList").child("ChannelEntry");
		pugi::xml_node main_node = url_node.child("MainChannel");
		if (!main_node.empty())
		{
			HIKStreamInfo main_info;
			memset(&main_info, 0, sizeof(HIKStreamInfo));
			main_info.ip = m_device->device_ip();
			main_info.channel = 1;
			main_info.rtsp_port = rtsp_port;
			main_info.stream_type = 0;
			pugi::xml_node codec_node = main_node.child("VideoEncodeType").child("Range");
			string codec_str = codec_node.value();
			if (codec_str.find("1"))
			{
				main_info.codec_type = 0;
			}
			stream_info->push_back(main_info);
		}
		pugi::xml_node sub_node = url_node.child("SubChannelList");
		if (!sub_node.empty())
		{
			HIKStreamInfo sub_info;
			memset(&sub_info, 0, sizeof(HIKStreamInfo));
			sub_info.ip = m_device->device_ip();
			sub_info.rtsp_port = rtsp_port;
			sub_info.channel = 1;
			sub_info.stream_type = 1;
			pugi::xml_node codec_node = sub_node.child("SubChannelEntry").child("VideoEncodeType");
			string codec_str = codec_node.value();
			if (codec_str.find("1"))
			{
				sub_info.codec_type = 0;
			}
			stream_info->push_back(sub_info);
		}
		if (!stream_info->empty())
		{
			return v_OK;
		}
	}
	return v_ERR_Bad_XML;
}

std::string hikvision_device_media::_create_stream_url(char *out_xml, long *out_size, const HIKStreamInfoVec &stream_info, const int &stream_id)
{
	if (!out_size || !out_xml)
	{
		return "";
	}
	pugi::xml_document doc;
	stringstream ss;
	pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";
	pugi::xml_node root_node		= doc.append_child(HIK_NODE_STREAM_URL_LISTS_NAME);
	for (auto &item :stream_info)
	{
		if (0 == item.stream_type && StreamMain&stream_id)
		{
			ss << "rtsp://" << item.ip << ":" << item.rtsp_port << "/h264/ch" << item.channel << "/main/av_stream";
			pugi::xml_node url_node			= root_node.append_child(HIK_NODE_STREAM_URL_NAME);
			string content = ss.str();
			if (!url_node.empty())
			{
				if (!url_node.text().set(content.c_str()))
				{
					cout << "error";
				}
			}
		}
		else if (1 == item.stream_type && StreamSub&stream_id)
		{
			ss << "rtsp://" << item.ip << ":" << item.rtsp_port << "/h264/ch" << item.channel << "/sub/av_stream";
			pugi::xml_node url_node			= root_node.append_child(HIK_NODE_STREAM_URL_NAME);
			string content = ss.str();
			if (!url_node.empty())
			{
				if (!url_node.text().set(content.c_str()))
				{
					cout << "error";
				}
			}
		}
		ss.str("");
	}
	stringstream ss_out;
	doc.save(ss_out);
#ifdef _DEBUG
	doc.save_file("vistek_stream_out.xml");
#endif
	if (*out_size < ss_out.str().size() && 0 < *out_size)
	{
		memcpy(out_xml, ss_out.str().c_str(), *out_size);
	}
	else
	{
		memcpy(out_xml, ss_out.str().c_str(), ss_out.str().size());
		*out_size = ss_out.str().size();
	}
	return ss_out.str();
}

/*
void __stdcall hikvision_device_ipc::s_exception_message(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
	if (pUser)
	{
		hikvision_device_ipc *device_ptr = (hikvision_device_ipc *)pUser;
		pugi::xml_document doc;
		stringstream ss;
		doc.save(ss);
		long data_size = ss.str().size();
		device_ptr->m_status_channge_signal((char*)ss.str().c_str(), &data_size, device_ptr);
	}
	else
	{
		
	}
}
*/
