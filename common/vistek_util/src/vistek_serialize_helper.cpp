#include "stdafx.h"
#include "../include/vistek_serialize_helper.h"
#include "pre_regex.h"
using namespace std;
const char* g_xml_serialize_header = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<!DOCTYPE boost_serialization>\r\n<boost_serialization signature=\"serialization::archive\" version=\"11\">";

void vistek_xml_serializable_ex::_add_header(std::string& content)
{
	stringstream ss;
	ss << g_xml_serialize_header << endl;
	ss << content;
	content = ss.str();
}

void vistek_xml_serializable_ex::_remove_header(string& content)
{
	REGEX e("(<\?[\\S\\s]+\?>)[\r\n]{1,2}(<!DOCTYPE boost_serialization>)[\r\n]{1,2}(<boost_serialization .+?>)[\r\n]{1,2}(<.+$)");
	content = REGEX_REPLACE(content, e, "$4");
}

void vistek_xml_serializable_ex::_add_attributes(std::string& content, const std::string& nodeName, int classID)
{
	string reg_str("<(" + nodeName + ")>");
	REGEX e(reg_str);
	stringstream ss;
	ss << "<$1 " << "class_id=\"" << classID << "\" tracking_level=\"" << 0 << "\" version=\"" << 0 << "\">";
	content = REGEX_REPLACE(content, e, ss.str(), REGEX_CONSTANTS::format_first_only);
}

void vistek_xml_serializable_ex::_remove_attributes(std::string& content)
{
	string reg_str = "<(\\w+) (class_id=\"\\d+\" tracking_level=\"\\d+\" version=\"\\d+\")>";
	REGEX e(reg_str);
	content = REGEX_REPLACE(content, e, "<$1>");
}

void vistek_xml_serializable_ex::_replace_attributes(std::string& content, const string& nodeName, const string& attr)
{
	string reg_str = "<(" + nodeName + ")([ \\w\"]*)>";
	REGEX e(reg_str);
	if (attr != "")
	{
		content = REGEX_REPLACE(content, e, "<$1 " + attr + ">");
	}
	else
	{
		content = REGEX_REPLACE(content, e, "<$1>");
	}
}

void vistek_xml_serializable_ex::transform_array_to_msg(std::string& content, const std::string& nodeName)
{
	_remove_attributes(content);
	string reg_str0 = "(<" + nodeName + ">)([\\r\\n\\t]+<count>(\\d+)</count>[\\r\\n\\t]+<item_version>0</item_version>)";
	REGEX e0(reg_str0);
	string num_str = REGEX_REPLACE(content, e0, "$3", REGEX_CONSTANTS::format_no_copy);
	int num = atoi(num_str.c_str());
	content = REGEX_REPLACE(content, e0, "$1");

	REGEX e1("<([/]{0,1})(item)>");
	content = REGEX_REPLACE(content, e1, "<$1Item>");

	stringstream ss;
	ss << "Num=\"" << num << "\"";
	_replace_attributes(content, nodeName, ss.str());
}

void vistek_xml_serializable_ex::transform_array_to_std(std::string& content)
{
	string reg_str = "<(\\w+) (Num=\"(\\d+)\")>";

	string result = content;
	REGEX e(reg_str);
	REGEX_ITERATOR p = REGEX_ITERATOR(result.begin(), result.end(), e);
	for (REGEX_ITERATOR q; p != q; ++p)
	{
		string nodeName = p->format("$1");
		string num_str = p->format("$3");
		int num = atoi(num_str.c_str());
		stringstream ss;

		REGEX e0("<(" + nodeName + ") (Num=\"\\d+\")>");
		ss << "<$1>" << endl << "<count>" << num << "</count>" << endl << "<item_version>0</item_version>" << endl;
		string val = ss.str();
		content = REGEX_REPLACE(content, e0, val, REGEX_CONSTANTS::format_first_only);
	}

	REGEX e1("<([/]{0,1})Item");
	content = REGEX_REPLACE(content, e1, "<$1item");
}


std::string serialization_sample::to_string(const std::string& nodeName)
{
	
	stringstream ss;
	boost::archive::xml_oarchive xs(ss);
	xs << boost::serialization::make_nvp(nodeName.c_str(), *this);
	string content = ss.str();
	REGEX e("(<\?[\\S\\s]+\?>)[\r\n]{0,2}(<!DOCTYPE boost_serialization>)[\r\n]{0,2}(<boost_serialization .+?>)[\r\n]{0,2}(<.+$)");
	content = REGEX_REPLACE(content, e, "$4");
	return content;
}

bool serialization_sample::apply(const std::string& content, const std::string& nodeName)
{
	REGEX e("<(" + nodeName + ")>");
	string format_content = REGEX_REPLACE(content, e, "<$1 class_id=\"0\" tracking_level=\"0\" version=\"0\">", REGEX_CONSTANTS::format_all);
	return deserialize(this, format_content);
}

serialization_sample::serialization_sample(int classID)
: vistek_xml_serializable_ex(classID)
{

}
