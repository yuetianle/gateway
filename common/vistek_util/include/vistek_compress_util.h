#ifndef vistek_compress_util_h__
#define vistek_compress_util_h__

#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <string.h>
struct vistek_compress_header
{
	uint32_t magic;
	uint32_t filelength;
	char filename[504];
	vistek_compress_header()
	{
		magic = 0;
		filelength = 0;
		memset(filename, 0, sizeof(filename));
	}
	bool is_valid()
	{
		return magic == 0xffffffff;
	}
};
struct vistek_file_info
{
	std::string file_name;
	std::vector<uint8_t> file_data;
};
typedef std::shared_ptr<vistek_file_info> vistek_file_info_ptr;
int v_compress_file_to_buffer(const std::string& filename, std::vector<uint8_t>& buffer);
int v_decompress_buffer_to_file(std::vector<uint8_t>& buffer, const std::string& output);
int v_decompress_buffer(uint8_t* data, size_t size, std::vector<uint8_t>& output);

void v_traversale_dir(const std::string& rt, std::vector<std::string>& result, const std::string& filename);

int v_compress_dir(const std::string& dir, const std::string& output);
//int v_compress_path(const std::string& filename, const std::string& output);
int v_decompress_file_to_path(const std::string& compressFile, const std::string& outputPath);

int v_decompress_buffer_to_mem(std::vector<uint8_t> src, std::vector<vistek_file_info_ptr>& dest);
int v_decompress_file_to_mem(const std::string& compressFile, std::vector<vistek_file_info_ptr>& output);
class vistek_compressor
{
public:
	int compress();
public:
	vistek_compressor(const std::string& inputPath, const std::string& outputPath);
private:
	const std::string m_input_path;
	const std::string m_output_path;
	std::string m_root;
	std::vector<std::string> m_filelist;

};
#endif // vistek_compress_util_h__
