#ifndef vistek_args_util_h__
#define vistek_args_util_h__

extern const std::string g_root_dir;
extern const std::string g_vistek_product_path;
std::string v_get_server_id();

int v_parse_args(int argc, char* argv[], const std::string& reg_fmt, std::string& output);
int v_parse_text_value(int argc, char* argv[], const std::string& tag, std::string& output);
int v_parse_int_value(int argc, char* argv[], const std::string& tag, int& output);

int v_parse_args(std::vector<std::string>& params, const std::string& reg_fmt, std::string& output);
int v_parse_text_value(std::vector<std::string>& params, const std::string& tag, std::string& output);
int v_parse_int_value(std::vector<std::string>& params, const std::string& tag, int& output);
#endif // vistek_args_util_h__