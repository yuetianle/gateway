#include "stdafx.h"
#include "http_clients.h"


Vistek::v_http_client::v_http_client()
	:m_bdebug(false), m_bauth(false)
{

}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if (itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if (itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if (itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if (itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if (itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return nmemb;
}

int Vistek::v_http_client::_post(CURL *curl, const std::string & strPost, std::string & strResponse)
{
	CURLcode res;
	if (!curl)
	{
		return CURLE_FAILED_INIT;
	}
	res = curl_easy_setopt(curl, CURLOPT_POST, 1);
	res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	res = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strPost.size());
	return res;
}

int Vistek::v_http_client::_get(CURL *curl)
{
	CURLcode res;
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	res = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	return res;
}

int Vistek::v_http_client::_posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bdebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int Vistek::v_http_client::_gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bdebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Vistek::v_http_client::SetDebug(bool bDebug)
{
	m_bdebug = bDebug;
}

int Vistek::v_http_client::request(v_method mtd, const std::string &str_content, std::string &str_response)
{
	if (!is_auth())
	{
		return CURLE_LOGIN_DENIED;
	}
	CURL* curl = curl_easy_init();

	if (m_bdebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	int ec = CURLE_OK;
	string loginid = m_user_name + ":" + m_password;
	curl_easy_setopt(curl, CURLOPT_URL, str_content.c_str());
	ec = curl_easy_setopt(curl, CURLOPT_USERPWD, loginid.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&str_response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	strcmp(Vistek::v_methods::GET.c_str(), mtd.c_str());
	if (Vistek::v_methods::GET == mtd)
	{
		ec = _get(curl);
	} else if (Vistek::v_methods::PUT == mtd)
	{
		ec = _put(curl, str_content);
	} else if (Vistek::v_methods::POST == mtd)
	{
		string params;
		ec = _post(curl, params, str_response);
	} else if (Vistek::v_methods::DEL == mtd)
	{
		ec = _del(curl);
	}
	ec = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return ec;
}

int Vistek::v_http_client::set_auth(const string &username, const string &password)
{
	m_user_name = username;
	m_password = password;
	if (m_user_name.empty() || m_password.empty())
	{
		m_bauth = false;
	}
	else
	{
		m_bauth = true;
	}
	return CURLE_OK;
	/*
	CURLcode res;
	CURL* curl = curl_easy_init();
	string digist_id = username + ":" + password;
	if (!curl)
	{
		return CURLE_FAILED_INIT;
	}
	m_curl = curl;
	res = curl_easy_setopt(curl, CURLOPT_USERPWD, digist_id);
	if (CURLE_OK != res)
	{
		printf("error");
	}
	else
	{
		m_bauth = true;
	}
	return res;
	*/
}

int Vistek::v_http_client::_put(CURL *curl,const std::string &struri)
{
	CURLcode ec = CURLE_OK;
	if (!curl)
	{
		return CURLE_FAILED_INIT;
	}
	ec = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
	string content;
	ec = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.length());
	ec = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
	return ec;
}

int Vistek::v_http_client::_del(CURL *curl)
{
	if (!curl)
	{
		return CURLE_FAILED_INIT;
	}
	CURLcode ec = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	return ec;
}

Vistek::v_http_client::~v_http_client()
{

}

#define _V_METHODS
#define DAT(a,b) const Vistek::v_method Vistek::v_methods::a = b;
#include "http_client_defs.dat"
#undef _V_METHODS
#undef DAT
/*
#define _V_PHRASES
#define DAT(a,b,c) {Vistek::v_status_codes::a, c},
#include "http_client_defs.dat"
#undef _V_PHRASES
#undef DAT
*/