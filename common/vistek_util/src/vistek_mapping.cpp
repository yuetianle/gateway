#include "stdafx.h"
#include "vistek_mapping.h"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#endif
#include "vistek_file_util.h"

void vistek_close_handle(void* hdl)
{
#ifdef WIN32
	if (hdl != 0 && hdl != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hdl);
	}
#endif
}

bool vistek_mapping_memory::create(int size)
{
#ifdef WIN32
	fsys::path p(m_name);
	wstring name_w = p.native();
	SECURITY_ATTRIBUTES sa = { 0 };
	sa.bInheritHandle = 0;
	sa.lpSecurityDescriptor = 0;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);

	shared_ptr<void> temp(CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, size, name_w.c_str()), vistek_close_handle);
	if (temp != nullptr)
	{
		uint32_t xx = 0xF001F;
		uint32_t yy = SECTION_ALL_ACCESS;
		shared_ptr<void> hdl(MapViewOfFile(temp.get(), SECTION_ALL_ACCESS, 0, 0, size), UnmapViewOfFile);
		if (hdl != 0)
		{
			memset(hdl.get(), 0, size);
			m_map_handle = temp;
			m_mem_handle = hdl;
			return true;
		}
	}
#endif
	return false;
}
shared_ptr<void> vistek_mapping_memory::get_accsessor(int64_t offset, int size)
{
#ifdef WIN32
	if (m_map_handle != 0)
	{
		int low = offset & 0x00000000ffffffff;
		int high = offset >> 32;
		shared_ptr<void> hdl(MapViewOfFile(m_map_handle.get(), SECTION_ALL_ACCESS, high, low, size), UnmapViewOfFile);
		if (hdl != 0)
		{
			return hdl;
		}
	}
#endif
	return 0;
}

bool vistek_mapping_memory::open(int size)
{
	fsys::path p(m_name);
#ifdef WIN32	
    wstring name_w = p.native();
	shared_ptr<void> hdl(OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, name_w.c_str()), vistek_close_handle);
	if (hdl != nullptr)
	{
		m_map_handle = hdl;
		return true;
	}
#endif
	return false;
}


vistek_mapping_memory::~vistek_mapping_memory()
{
	close();
}

vistek_mapping_memory::vistek_mapping_memory(const string& name)
: vistek_mapping_base("Global\\" + name), m_mem_handle(0)
{
}

void vistek_mapping_memory::close()
{
	m_mem_handle.reset();
	m_map_handle.reset();
}



bool vistek_mapping_file::open(int size)
{
#ifdef WIN32
	boost::system::error_code ec;
	fsys::path p(m_name);
	/*shared_ptr<void> hdl(CreateFile(p.native().c_str(), FILE_ALL_ACCESS, 0, 0, 0, 0, 0), vistek_close_handle);
	if (hdl != 0)*/
	{
		SECURITY_ATTRIBUTES sa = { 0 };
		sa.bInheritHandle = 0;
		sa.lpSecurityDescriptor = 0;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		shared_ptr<void> mapping(CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, size, 0), vistek_close_handle);
		if (mapping != 0)
		{
			m_map_handle = mapping;
			//m_file_handle = hdl;
			return true;
		}
	}
#endif
	return false;
}

shared_ptr<void> vistek_mapping_file::get_accsessor(int64_t offset, int size)
{
#ifdef WIN32
	if (m_map_handle != 0)
	{
		int low = (int32_t)(offset & 0x00000000ffffffff);
		int high = offset >> 32;
		shared_ptr<void> hdl(MapViewOfFile(m_map_handle.get(), SECTION_ALL_ACCESS, high, low, size), UnmapViewOfFile);
		
		if (hdl != 0)
		{
			return hdl;
		}
		int err = GetLastError();
		cout << "failed to map" << err << endl;
	}
#endif
	return 0;
}

void vistek_mapping_file::close()
{
	m_map_handle.reset();
	//m_file_handle.reset();
	/*boost::system::error_code ec;
	if (fsys::exists(p, ec))
	{
		fsys::remove(p, ec);
	}*/
}

vistek_mapping_file::~vistek_mapping_file()
{
	close();
}

vistek_mapping_file::vistek_mapping_file(const string& filename) : vistek_mapping_base(filename)
{

}

vistek_mapping_base::vistek_mapping_base(const string& name)
{
	fsys::path p = fsys::current_path() / name;
	m_name = p.string();
}
