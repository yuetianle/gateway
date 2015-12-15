#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_
#pragma once
#include <string>
using std::string;
#include "curl.h"

#ifdef _WIN32
#define _XPLATSTR(x) L ## x
#else
#define _XPLATSTR(x) 
#endif
namespace Vistek
{
	//typedef std::wstring v_method;
	typedef std::string v_method;
	class v_methods
	{
	public:
#define _V_METHODS
#define DAT(a, b) const static v_method a;
#include "http_client_defs.dat"
#undef _V_METHODS
#undef DAT
	};
	typedef unsigned short v_status_code;
	/*class v_status_codes
	{
#define _V_PHRASES
#define DAT(a, b, c) const static v_status_code a = b;
#include "http_client_defs.dat"
#undef _V_PHRASES
#undef DAT
	};
	*/
class v_http_client
{
public:
	v_http_client();
	~v_http_client();

public:
	int set_auth(const string &username, const string &password);
	int request(v_method mtd, const std::string &str_content, std::string &str_response);
	void SetDebug(bool bDebug);
	bool is_auth()const { return m_bauth; }
private:
	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int _post(CURL* curl, const std::string & strPost, std::string & strResponse);

	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int _get(CURL *curl);

	/**
	* @brief HTTPS POST请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int _posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* @brief HTTPS GET请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int _gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);

	/**
	 * @brief HTTPS PUT请求.
	 * @param
	 */
	int _put(CURL *curl,const std::string &struri);
	/** 
	 * @brief HTTPS DEL请求.
	 *
	 */
	int _del(CURL *curl);

private:
	bool m_bdebug;
	bool m_bauth;
	string m_user_name;
	string m_password;
	CURL *m_curl;
};
}
#endif

