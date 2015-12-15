#include "stdafx.h"
#include "vistek_product_util.h"
#include "pre_regex.h"
#include "vistek_error_code.h"
#include "vistek_file_util.h"

using namespace std;
using namespace boost::filesystem;
#ifdef WIN32
const string g_root_dir = string(getenv("windir"));
const string g_vistek_product_path((path(g_root_dir).parent_path() / "ProgramData" / "Vistek").string());
#else
const string g_root_dir("/usr/share");
const string g_vistek_product_path((path(g_root_dir) / "vistek").string());
#endif
std::string v_get_server_id()
{
	path serverf = path(g_vistek_product_path) / "CoreServer.info";
	if (exists(serverf))
	{
		boost::property_tree::ptree p;
		boost::property_tree::read_xml(serverf.string(), p);
		return p.get<string>("core_server.id");
	}
	return "";
}

int v_parse_args(vector<string>& params, const string& reg_fmt, string& output)
{
	regex_text_finder rtf;
	for (auto& arg : params)
	{
		if (rtf.find(arg, reg_fmt))
		{
			output = rtf[1];
			return v_OK;
		}
	}
	return v_ERR_INVAILID_ARGS;
}

int v_parse_text_value(vector<string>& params, const string& tag, string& output)
{
	string reg_fmt = "--" + tag + "=([^ ]+)";
	return v_parse_args(params, reg_fmt, output);
}
int v_parse_int_value(vector<string>& params, const string& tag, int& output)
{
	string reg_fmt = "--" + tag + "=(\\d+)";
	string output_str = "";
	int ec = v_parse_args(params, reg_fmt, output_str);
	if (ec == v_OK)
	{
		output = atoi(output_str.c_str());
	}
	return ec;
}
int v_parse_args(int argc, char* argv[], const string& reg_fmt, string& output)
{
	regex_text_finder rtf;
	for (int i = 1; i < argc; i++)
	{
		string str(argv[i]);
		if (rtf.find(str, reg_fmt))
		{
			output = rtf[1];
			return v_OK;
		}
	}
	return v_ERR_INVAILID_ARGS;
}

int v_parse_text_value(int argc, char* argv[], const string& tag, string& output)
{
	string reg_fmt = "--" + tag + "=([\\S\\s]+)";
	return v_parse_args(argc, argv, reg_fmt, output);
}
int v_parse_int_value(int argc, char* argv[], const string& tag, int& output)
{
	string reg_fmt = "--" + tag + "=(\\d+)";
	string output_str = "";
	int ec = v_parse_args(argc, argv, reg_fmt, output_str);
	if (ec == v_OK)
	{
		output = atoi(output_str.c_str());
	}
	return ec;
}
