#ifndef authentication_defs_h__
#define authentication_defs_h__


#include <string>
enum class AuthenticationType {
	None,
	Basic,
	Digest
};
extern std::string AuthenticationTypeDesc[3];

extern std::string convert_authentication_type_to_str(AuthenticationType auth_type);
extern AuthenticationType convert_authentication_type_from_str(const std::string& str);



class www_authentication_info 
{
	AuthenticationType m_auth_type;
	std::string m_username;
	std::string m_password;
	std::string m_realm;
	std::string m_nonce;
	std::string m_nc;
	std::string m_cnonce;
	std::string m_qop;
	std::string m_response;
private:
	std::string _get_response(const std::string& method, const std::string& url);
public:
	www_authentication_info();
	www_authentication_info(const std::string& un, const std::string& pw, const std::string& auth_info);
	std::string calc_response(const std::string& method, const std::string& uri, const std::string& un, const std::string& pw, const std::string& realm, const std::string& nonce, const std::string& nc = "", const std::string& cnonce = "", const std::string& qop = "");
	std::string get_authorization(const std::string& method, const std::string& url);
	std::string response() const { return m_response; }
};
#endif // authentication_defs_h__