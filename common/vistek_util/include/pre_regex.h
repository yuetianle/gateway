#ifndef pre_regex_h__
#define pre_regex_h__


#ifndef __std_regex__
#include <boost/regex.hpp>
#define REGEX boost::regex
#define SMATCH boost::smatch
#define REGEX_ITERATOR boost::sregex_iterator
#define REGEX_SEARCH boost::regex_search
#define REGEX_MATCH boost::regex_match
#define SREGEX_ITERATOR boost::sregex_iterator
#define REGEX_ERROR boost::regex_error
#define REGEX_CONSTANTS boost::regex_constants
#define REGEX_REPLACE boost::regex_replace
#define REGEX_FORMATTER boost::formatter
#define REGEX_CONSTANTS boost::regex_constants
#define REGEX_FORMAT_NO_COPY boost::regex_constants::format_no_copy
#else
#include <regex>
#define REGEX std::regex
#define SMATCH std::smatch
#define REGEX_ITERATOR std::sregex_iterator
#define REGEX_SEARCH std::regex_search
#define REGEX_MATCH std::regex_match
#define SREGEX_ITERATOR std::sregex_iterator
#define REGEX_ERROR std::regex_error
#define REGEX_CONSTANTS std::regex_constants
#define REGEX_REPLACE std::regex_replace
#define REGEX_FORMATTER std::formatter
#define REGEX_CONSTANTS std::regex_constants
#define REGEX_FORMAT_NO_COPY std::regex_constants::format_no_copy
#endif

#ifdef WIN32
#define SPRINTF_S   sprintf_s
#define SSCANF_S    sscanf_s
#else
#define SPRINTF_S   snprintf
#define SSCANF_S    sscanf
#endif // __linux

#include <iostream>
#include <sstream>
using namespace std;
class regex_text_finder
{
	string m_temp;
	REGEX_ITERATOR m_reg_iter;
	REGEX_ITERATOR m_reg_iter_null;
	stringstream m_format;
private:
	template<typename T>
	void _append(T&& val)
	{
		m_format << val;
	}
	template<typename T, typename... Args>
	void _append(T&& first, Args... args)
	{
		_append(first);
		_append(forward<Args>(args)...);
	}
public:
	template<typename T, typename... Args>
	bool find(const string& msg, T&& first, Args&&... args)
	{
		m_temp = msg;
		m_format.str("");
		_append(first, forward<Args>(args)...);
		string pattern = m_format.str();
		REGEX e(pattern);
		m_reg_iter = REGEX_ITERATOR(m_temp.begin(), m_temp.end(), e);
		return m_reg_iter != m_reg_iter_null;
	}
	template<typename T, typename... Args>
	string replace(const string& msg, const string& pattern, T&& first, Args&&... args)
	{
		m_format.str("");
		_append(first, forward<Args>(args)...);
		string replacement = m_format.str();
		REGEX e(pattern);
		string result = REGEX_REPLACE(msg, e, replacement);
		return result;
	}
	/*bool find(const string& pattern, const string& msg)
	{
		REGEX e(pattern);
		m_reg_iter = REGEX_ITERATOR(msg.begin(), msg.end(), e);
		return m_reg_iter != m_reg_iter_null;
	}*/
	string operator[](int index)
	{
		return get_value(index);
	}
	string get_value(int index)
	{
		m_format.str("");
		m_format << "$" << index;
		return m_reg_iter->format(m_format.str());
	}
};
#endif // pre_regex_h__