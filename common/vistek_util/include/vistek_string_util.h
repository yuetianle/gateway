
#ifndef vistek_string_util_h__
#define vistek_string_util_h__

#define USE_BOOST_LOCALE
//#define GB_ALPHA_CONVERT(start, end, code, letter) if(code >= start && code <= end) return letter
#define GB_ALPHA_CONVERT(start, end, code, letter) if((code&0xffff) >= start && (code&0xffff) <= end) return letter

inline char v_str_get_alpha(wchar_t n)
{
	/// ¸ù¾Ýºº×ÖÇøÓòÂë»ñÈ¡Æ´ÒôÉùÄ¸
	GB_ALPHA_CONVERT(0xB0A1, 0xB0C4, n, 'a');
	GB_ALPHA_CONVERT(0XB0C5, 0XB2C0, n, 'b');
	GB_ALPHA_CONVERT(0xB2C1, 0xB4ED, n, 'c');
	GB_ALPHA_CONVERT(0xB4EE, 0xB6E9, n, 'd');
	GB_ALPHA_CONVERT(0xB6EA, 0xB7A1, n, 'e');
	GB_ALPHA_CONVERT(0xB7A2, 0xB8c0, n, 'f');
	GB_ALPHA_CONVERT(0xB8C1, 0xB9FD, n, 'g');
	GB_ALPHA_CONVERT(0xB9FE, 0xBBF6, n, 'h');
	GB_ALPHA_CONVERT(0xBBF7, 0xBFA5, n, 'j');
	GB_ALPHA_CONVERT(0xBFA6, 0xC0AB, n, 'k');
	GB_ALPHA_CONVERT(0xC0AC, 0xC2E7, n, 'l');
	GB_ALPHA_CONVERT(0xC2E8, 0xC4C2, n, 'm');
	GB_ALPHA_CONVERT(0xC4C3, 0xC5B5, n, 'n');
	GB_ALPHA_CONVERT(0xC5B6, 0xC5BD, n, 'o');
	GB_ALPHA_CONVERT(0xC5BE, 0xC6D9, n, 'p');
	GB_ALPHA_CONVERT(0xC6DA, 0xC8BA, n, 'q');
	GB_ALPHA_CONVERT(0xC8BB, 0xC8F5, n, 'r');
	GB_ALPHA_CONVERT(0xC8F6, 0xCBF0, n, 's');
	GB_ALPHA_CONVERT(0xCBFA, 0xCDD9, n, 't');
	GB_ALPHA_CONVERT(0xCDDA, 0xCEF3, n, 'w');
	GB_ALPHA_CONVERT(0xCEF4, 0xD188, n, 'x');
	GB_ALPHA_CONVERT(0xD1B9, 0xD4D0, n, 'y');
	GB_ALPHA_CONVERT(0xD4D1, 0xD7F9, n, 'z');
	if (n & 0x007f > 0)
	{
		return (char)(n & 0x007f);
	}
	return '\0';
}
#include <string>

inline std::string v_str_trim(const std::string& str)
{
	std::string::size_type pos = str.find_first_not_of(' ');
	if (pos == std::string::npos)
	{
		return "";
	}
	std::string::size_type pos2 = str.find_last_not_of(' ');
	if (pos2 != std::string::npos)
	{
		return move(str.substr(pos, pos2 - pos + 1));
	}
	return move(str.substr(pos));
}

inline std::string v_str_trim_end(const std::string& str, char trimchar)
{
	
	std::string::size_type pos2 = str.find_last_not_of(trimchar);
	if (pos2 != std::string::npos)
	{
		return move(str.substr(0, pos2 + 1));
	}
	else
	{
		return str;
	}
}
inline std::string v_str_trim_ex(const std::string& str, char trimchar)
{
	std::string::size_type pos = str.find_first_not_of(trimchar);
	if (pos == std::string::npos)
	{
		return "";
	}
	std::string::size_type pos2 = str.find_last_not_of(trimchar);
	if (pos2 != std::string::npos)
	{
		return move(str.substr(pos, pos2 - pos + 1));
	}
	return move(str.substr(pos));
}
inline std::vector<std::string> v_str_split_ex(const std::string& s, const std::string& delim)
{
	std::vector<std::string> ret;
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(s.substr(last, index - last));
	}
	return std::move(ret);
}
inline int v_str_split(const std::string& str, std::vector<std::string>& ret_, std::string sep = ",")
{
	if (str.empty())
	{
		return 0;
	}

	std::string tmp;
	std::string::size_type pos_begin = str.find_first_not_of(sep);
	std::string::size_type comma_pos = 0;

	while (pos_begin != std::string::npos)
	{
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != std::string::npos)
		{
			tmp = str.substr(pos_begin, comma_pos - pos_begin);
			pos_begin = comma_pos + sep.length();
		}
		else
		{
			tmp = str.substr(pos_begin);
			pos_begin = comma_pos;
		}

		if (!tmp.empty())
		{
			ret_.push_back(tmp);
			tmp.clear();
		}
	}
	return 0;
}

inline std::string v_str_replace(const std::string& src, const std::string& replacement, const std::string& dest)
{
	std::string ret;
	std::string::size_type pos_begin = 0;
	std::string::size_type pos = src.find(replacement);
	int old_len = replacement.size();
	while (pos != std::string::npos)
	{
#ifdef _DEBUG
		std::cout << "replacexxx:" << pos_begin << " " << pos << std::endl;
#endif
		ret.append(src.data() + pos_begin, pos - pos_begin);
		ret += dest;
		pos_begin = pos + old_len;
		pos = src.find(replacement, pos_begin);
	}
	if (pos_begin < src.length())
	{
		ret.append(src.begin() + pos_begin, src.end());
	}
	return ret;
}

#ifdef WIN32
#include <algorithm>
#include <codecvt>
#endif

#ifndef USE_BOOST_LOCALE
#define USE_BOOST_LOCALE
#endif
inline std::string v_str_to_lower(const std::string &str)
{
	std::string strTmp = str;
	std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::towlower);
	return move(strTmp);
}
// string.compareNoCase
inline bool v_str_compare_no_case(const std::string &strA, const std::string &strB)
{
	std::string str1 = v_str_to_lower(strA);
	std::string str2 = v_str_to_lower(strB);
	return (str1 == str2);
}
#ifdef USE_BOOST_LOCALE
#include "boost/locale.hpp"

inline std::wstring v_str_to_wstr(const std::string& s, const char* loc = "GBK")
{
	std::wstring result = boost::locale::conv::to_utf<wchar_t>(s, loc);
	return result;
}

inline std::string v_wstr_to_str(const std::wstring& s, const char* loc = "GBK")
{
	std::string result = boost::locale::conv::from_utf(s, loc);
	return result;
}
inline std::string v_str_to_utf8(const std::string& str, const char* loc = "GBK")
{
	std::string const &to_encoding("UTF-8");
	std::string const &from_encoding(loc);
	std::string result = boost::locale::conv::between(str.c_str(), str.c_str() + str.size(), to_encoding, from_encoding, boost::locale::conv::default_method);
	return result;
}

inline std::string v_utf8_to_string(const std::string & str, const char* loc = "GBK")
{
	std::string const &to_encoding(loc);
	std::string const &from_encoding("UTF-8"); 
	std::string result = boost::locale::conv::between(str.c_str(), str.c_str() + str.size(), to_encoding, from_encoding, boost::locale::conv::default_method);
	return result;
}
inline std::string v_wstr_to_utf8(const std::wstring& wstr, const char* loc = "GBK")
{
	std::string str = v_wstr_to_str(wstr, loc);
	std::string result = v_str_to_utf8(str.c_str(), loc);
	return result;
}
#else
inline std::string v_str_to_utf8_(const char* str)
{
	std::wstring wstr = v_str_to_wstr(str);
	//std::locale old_locale;
	//std::locale utf8_locale(old_locale, new codecvt_utf8<wchar_t>);
	//std::locale::global(utf8_locale);
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

inline std::string v_utf8_to_string_(const std::string & s)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring wstr = conv.from_bytes(s);
	std::string result = v_wstr_to_str(wstr);
	return result;
}
inline std::wstring v_str_to_wstr(const std::string& s)
{
	setlocale(LC_ALL, "zh-CN");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

inline std::string v_wstr_to_str(const std::wstring& s)
{
	setlocale(LC_ALL, "zh-CN");
	int ln = s.length();
	const wchar_t* _Source = s.data();

	memcpy((void*)_Source, (void*)s.data(), ln * sizeof(wchar_t));
	size_t _Dsize = (ln + 1) * 2;
	int sz = s.size();
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result(_Dest);
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}
inline std::string v_str_to_utf8(const char* str)
{
	std::wstring wstr = v_str_to_wstr(str);
	//std::locale old_locale;
	//std::locale utf8_locale(old_locale, new codecvt_utf8<wchar_t>);
	//std::locale::global(utf8_locale);
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

inline std::string v_utf8_to_string(const std::string & s)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring wstr = conv.from_bytes(s);
	std::string result = v_wstr_to_str(wstr);
	return result;
}
inline std::string v_wstr_to_utf8(const wchar_t* wideStr)
{
	//std::locale old_locale;
	//std::locale utf8_locale(old_locale, new codecvt_utf8<wchar_t>);
	//std::locale::global(utf8_locale);
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wideStr);
}
#endif

inline std::string v_str_gb_to_alpha(const std::string& str)
{
	//std::wstring ws = v_str_to_wstr(str, "");
	std::string result;
	for (int i = 0; i < (int)str.size();)
	{
		uint8_t* ptr = (uint8_t*)str.data() + i;
		if (*ptr < 0x80)
		{
			result.push_back(*ptr);
			++i;
		}
		else
		{
			uint16_t w = (*ptr) << 8;
			w |= *(ptr + 1);
			char x = v_str_get_alpha((wchar_t)w);
			result.push_back(x);
			i += 2;
		}
	}
	return result;
}

#endif // vistek_string_util_h__