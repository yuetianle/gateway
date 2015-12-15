#ifndef uri_parser_h__
#define uri_parser_h__

#pragma once
class uri_parser
{
public:
	std::string std_to_uri_time(const string& stdTime);

	std::string uri_to_std_time(const string &uriTime);

private:
	std::string m_uri;
	std::string m_protocol_name;
	std::string m_resource_path;
	std::string m_param_str;
	std::string m_hostname;
	std::string m_username;
	std::string m_password;
	int m_port;
	bool m_is_ready = false;
public:
	typedef shared_ptr<uri_parser> ptr;
	uri_parser(const std::string& uri);
	~uri_parser();
	
private:
	bool _parse_user_pass(const std::string& auth_str);
	bool _parse_endpoint(const std::string& endpoint_str);
	bool _parse_proxy_str(const std::string& endpoint_str, const std::string& proxy_name);
	bool _parse_parameters(const std::string& actName, const std::string& params);
public:
	bool is_ready() const { return m_is_ready; }
	std::string uri() const { return m_uri; }
	std::string protocol_name() const { return m_protocol_name; }
	std::string hostname() const { return m_hostname; }
	int port() const { return m_port; }
	std::string resource_path() const { return m_resource_path; }
	std::string param_str() const { return m_param_str; }
	std::string username() const { return m_username; }
	std::string password() const { return m_password; }
};

#endif // uri_parser_h__
