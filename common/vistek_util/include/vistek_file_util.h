#ifndef vistek_file_util_h__
#define vistek_file_util_h__

#pragma once
#ifdef __linux
#ifndef BOOST_NO_CXX11_SCOPED_ENUMS
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#endif
#include "boost/filesystem.hpp"
#include "pre_boost_basic.h"
#include <string>
using namespace std;
namespace fsys = boost::filesystem;
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
inline int v_file_chmod(const string& target, int mod)
{
    boost::system::error_code ec;
    fsys::path p(target);
    
    //if(fsys::exists(p, ec))
    {
        int error_code = chmod(p.string().c_str(), mod);
        // S_ISUID | S_ISGID | S_ISVTX | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IXOTH);
        if (error_code != 0) 
        {
            cout << "chomd() failed; error code = " << error_code << endl;
        }
        return error_code;
    }
    return 0;
}
#endif
inline int v_file_get_sub_dir(const fsys::path& dir, vector<fsys::path>& subs)
{
	boost_error_code ec;
	fsys::path p(dir);
	if (fsys::exists(p, ec) && fsys::is_directory(p, ec))
	{
		fsys::directory_iterator enditer;
		int count = 0;
		try
		{
			for (fsys::directory_iterator iter(p); iter != enditer; ++iter)
			{
				auto pp = iter->path();
				if (fsys::is_directory(pp, ec))
				{
					subs.push_back(pp);
					count++;
				}
			}
		}
		catch (std::exception& e)
		{

		}
		catch (const char* e)
		{
		}
		return count;
	}
	return 0;
}
inline int v_file_get_sub_path(const fsys::path& dir, vector<fsys::path>& subs)
{
	fsys::path p(dir);
	if (fsys::exists(p) && fsys::is_directory(p))
	{
		fsys::directory_iterator enditer;
		int count = 0;
		try
		{
			for (fsys::directory_iterator iter(p); iter != enditer; ++iter)
			{
				auto pp = iter->path();
				subs.push_back(pp);
				count++;
			}
		}
		catch (std::exception& e)
		{

		}
		catch (const char* e)
		{
		}
		return count;
	}
	return 0;
}

class vistek_file_stream
{
	FILE* m_file = 0;
public:
	vistek_file_stream(const string& fn)
	{

	}
	vistek_file_stream(const fsys::path p)
	{

	}
};
#endif // vistek_file_util_h__