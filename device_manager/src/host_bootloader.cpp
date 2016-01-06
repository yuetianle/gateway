#include "stdafx.h"
#include "host_bootloader.h"

host_bootloader::host_bootloader()
	:m_is_start(false) 
{
	_load_host_cfg(m_cfg_file.c_str());
}

host_bootloader::~host_bootloader()
{
}

int host_bootloader::_load_host_cfg(const char *cfg_file)
{
	fsys::path p(cfg_file);

	pugi::xml_document doc;
	pugi::xml_parse_result pugi_res = doc.load_file(p.filename().c_str());

	if (pugi_res.status == pugi::xml_parse_status::status_ok)
	{
		for (auto& x : doc.child("host_list"))
		{
			string exe_name = x.attribute("exe").as_string();
			string dir = x.attribute("dir").as_string();
			string manufacture = x.attribute("manufacture").as_string();
			int	   proc_count = x.attribute("counts").as_int();
			string filename = "";
			string workpath = "";
			boost_error_code ec;
			if (exe_name != "")
			{
				fsys::path p = fsys::current_path();
				fsys::directory_iterator search_dir(p, ec);
				if (!ec)
				{
					for (auto f : search_dir)
					{
						auto fn = f.path();
						if (!fsys::is_directory(fn, ec) && fn.filename().string().find(exe_name) != string::npos && fn.extension().string() == ".exe")
						{
							filename = fn.filename().string();
							workpath = fn.parent_path().string();
							break;
						}
					}
				}
				if (filename == "")
				{
					fsys::path win_dir = string(getenv("windir"));
					fsys::path root_dir(win_dir.root_directory());
					fsys::path media_api_dir = (root_dir / "ProgramData" / "Vistek" / "MediaClientPack");
					if (fsys::exists(media_api_dir, ec) && fsys::is_directory(media_api_dir, ec))
					{
						fsys::directory_iterator search_dir(p, ec);
						if (!ec)
						{
							for (auto f : search_dir)
							{
								auto fn = f.path();
								if (!fsys::is_directory(fn, ec) && fn.filename().string().find(exe_name) != string::npos && fn.extension().string() == ".exe")
								{
									filename = fn.filename().string();
									workpath = fn.parent_path().string();
									break;
								}
							}
						}
					}
				}
			}
			m_hosts_info.push_back(make_tuple("", workpath, filename, manufacture, proc_count));
		}
		return v_OK;
	}
	return v_ERR_Bad_XML;

}

int host_bootloader::start_hosts()
{
	for (auto item: m_hosts_info)
	{
		int proc_count = std::get<4>(item);
		int proc_index = 0;
		if (0 < proc_count && proc_index < proc_count)
		{
			vistek_proc_ptr host_proc(new vistek_process(std::get<0>(item), std::get<1>(item), std::get<2>(item), std::get<3>(item)));
			host_proc->start();
			
			m_host_proc_lists.push_back(host_proc);
			proc_index++;
		}
	}
	_remove_invaild_cfg("hostboot");
	return v_OK;
}

int host_bootloader::stop_hosts()
{
	for (auto item: m_host_proc_lists)
	{
		item->stop();
	}
	m_host_proc_lists.clear();
	return v_OK;
}

int host_bootloader::_remove_invaild_cfg(const char *cfg_dir)
{
	if (cfg_dir)
	{
		fsys::path tmp_dir(cfg_dir);
		if (fsys::exists(tmp_dir))
		{
			boost_error_code ec;
			fsys::directory_iterator search_dir(tmp_dir, ec);
			for (auto item : search_dir)
			{
				auto file = item.path();
				if (!fsys::is_directory(file) && file.extension().string() == ".xml")
				{
					string file_name = file.filename().stem().string();
					if (cfg_dir)
					{
						string remove_file;
						bool b_find = false;
						for (auto item :m_host_proc_lists)
						{
							if (item->process_info().dwProcessId == atoi(file_name.c_str()))
							{
								b_find = true;
								break;
							}
						}
						if (!b_find)
						{
							fsys::remove(file);
						}
					}
				}
			}
		}
		return v_OK;
	}
	else
	{
		return v_ERR_BAD_ARGUMENT;
	}
}

/*int host_bootloader::_write_host_listen_ports(vistek_proc_ptr proc, const string &manuc)
{
	if (m_is_start && proc)
	{
		fsys::path host_listen_dir(m_host_listen_dir);
#ifdef _WIN32
		char file_name[100] = { 0 };
		sprintf_s(file_name, "%d.xml", proc->process_info().dwProcessId);
#endif
		if (!fsys::exists(host_listen_dir))
		{
			fsys::create_directory(host_listen_dir);
		}
		host_listen_dir /= file_name;
		fsys::path full_file = host_listen_dir;
		auto host_nodes_tmp = m_host_lists_doc.child("hosts");
		pugi::xml_node decl_node = m_host_lists_doc.prepend_child(pugi::node_declaration);
		decl_node.append_attribute("version") = "1.0";
		decl_node.append_attribute("encoding") = "utf-8";
		auto host_nodes = m_host_lists_doc.child("hosts");
		if (host_nodes.empty())
		{
			host_nodes = m_host_lists_doc.append_child("hosts");
		}
		pugi::xml_node host_node = host_nodes.append_child("host");
		auto manu_attr = host_node.attribute("manufacture");
		if (manu_attr.empty())
		{
			manu_attr = host_node.append_attribute("manufacture");
			manu_attr.set_value(manuc.c_str());
		}
		auto port_attr = host_node.attribute("port");
		if (port_attr.empty())
		{
			port_attr = host_node.append_attribute("port");
			//port_attr.set_value();
		}
		if (m_host_lists_doc.save_file(host_listen_dir.string().c_str()))
		{
			vvlog_i("save host_listen file sucess:file:" << host_listen_dir.string());
			return v_OK;
		}
		else
		{
			vvlog_e("save host_listen file fail:file:" << host_listen_dir.string());
		}
	}
	return v_ERR_Bad_XML;
}*/
