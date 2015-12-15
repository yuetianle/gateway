#include "stdafx.h"
#include "authentication_util.h"
#include "md5.h"
#include "base64.h"
#include <typeinfo>
using namespace std;

std::string AuthenticationTypeDesc[3] = { "None", "Basic", "Digest" };
const char * AuthenticationTypeNames[] = { "None", "Basic", "Digest" };

extern string convert_authentication_type_to_str(AuthenticationType auth_type)
{
	int index = (int)auth_type;
	int count = sizeof (AuthenticationTypeNames) / sizeof (AuthenticationTypeNames[0]);
	if (index < count)
	{
		return AuthenticationTypeNames[index];
	}
	return "";
}

extern AuthenticationType convert_authentication_type_from_str(const string& str)
{
	for (int i = 0; i < sizeof (AuthenticationTypeNames) / sizeof (AuthenticationTypeNames[0]); i++)
	{
		if (strcmp(str.c_str(), AuthenticationTypeNames[i]) == 0)
		{
			return static_cast <AuthenticationType>(i);
		}
	}
    cout<<"Failed to convert a string to OSType"<<endl;
	throw bad_cast();
}

std::string www_authentication_info::get_authorization(const string& method, const string& url)
{
	string result = "Authorization: ";
	if (m_auth_type == AuthenticationType::Digest) {
		result.append(AuthenticationTypeDesc[(int)AuthenticationType::Digest]);
		result.append(" username=\"").append(m_username).append("\", ");
		result.append(" realm=\"").append(m_realm).append("\", ");
		if (m_nonce.size() > 0) {
			result.append("nonce=\"").append(m_nonce).append("\", ");
		}
		if (m_nc.size() > 0) {
			result.append("nc=\"").append(m_nc).append("\", ");
		}
		if (m_cnonce.size() > 0) {
			result.append("cnonce=\"").append(m_cnonce).append("\", ");
		}
		if (m_qop.size() > 0) {
			result.append("qop=\"").append(m_qop).append("\", ");
		}
		result.append("uri=\"").append(url).append("\", ");
		m_response = _get_response(method, url);
		result.append("response=\"").append(m_response).append("\"");
	}
	else if (m_auth_type == AuthenticationType::Basic) {
		result.append(AuthenticationTypeDesc[(int)AuthenticationType::Basic]);
		string content = "";
		content.append(m_username).append(":").append(m_password);
		content = base64_enc(content);
		result.append(" ").append(content);
	}
	return result;
}

std::string www_authentication_info::_get_response(const string& method, const string& url)
{
	string h1 = "";
	string h2 = "";
	string h3 = "";
	h1.append(m_username).append(":").append(m_realm).append(":").append(m_password);
	h1 = md5(h1);
	h2 = h2.append(method).append(":").append(url);
	h2 = md5(h2);
	h3.append(h1);
	if (m_nonce.size() > 0) {
		h3.append(":");
		h3.append(m_nonce);
	}
	if (m_nc.size() > 0) {
		h3.append(":");
		h3.append(m_nc);
	}
	if (m_cnonce.size() > 0) {
		h3.append(":");
		h3.append(m_cnonce);
	}
	if (m_qop.size() > 0) {
		h3.append(":");
		h3.append(m_qop);
	}
	h3.append(":");
	h3.append(h2);
	h3 = md5(h3);
	return h3;
}

www_authentication_info::www_authentication_info(const string& un, const string& pw, const string& auth_info)
{
	m_username = un;
	m_password = pw;
	regex_text_finder finder;
	if (!finder.find(auth_info, "(WWW-Authenticate:)"))
	{
		return;
	}
	if (finder.find(auth_info, "(Digest )\\S+"))
	{
		m_auth_type = AuthenticationType::Digest;
	}
	else if (finder.find(auth_info, "(Basic )\\S+"))
	{
		m_auth_type = AuthenticationType::Basic;
	}
	if (finder.find(auth_info, "realm=\"(\\w+)\""))
	{
		m_realm = finder[1];
	}
	if (finder.find(auth_info, "nonce=\"(\\w+)\""))
	{
		m_nonce = finder[1];
	}
	if (finder.find(auth_info, "nc=\"(\\w+)\""))
	{
		m_nc = finder[1];
	}
	if (finder.find(auth_info, "cnonce=\"(\\w+)\""))
	{
		m_cnonce = finder[1];
	}
	if (finder.find(auth_info, "qop=\"(\\w+)\""))
	{
		m_qop = finder[1];
	}
}

www_authentication_info::www_authentication_info()
{
	m_auth_type = AuthenticationType::None;
	m_realm = "";
	m_nonce = "";
	m_nc = "";
	m_cnonce = "";
	m_qop = "";
}

std::string www_authentication_info::calc_response(const std::string& method, const std::string& uri, const std::string& un, const std::string& pw, const std::string& realm, const std::string& nonce, const std::string& nc /*= ""*/, const std::string& cnonce /*= ""*/, const std::string& qop /*= ""*/)
{
	m_username = un;
	m_password = pw;
	m_realm = realm;
	m_nonce = nonce;
	m_nc = nc;
	m_cnonce = cnonce;
	m_qop = qop;
	m_response = _get_response(method, uri);
	return m_response;
}
