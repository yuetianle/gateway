#ifndef _BASE64_H_
#define _BASE64_H_
#include <string>

class ZBase64
{
public:
    /*编码
    DataByte
        [in]输入的数据长度,以字节为单位
    */
    std::string Encode(const unsigned char* Data,int DataByte);
    /*解码
    DataByte
        [in]输入的数据长度,以字节为单位
    OutByte
        [out]输出的数据长度,以字节为单位,请不要通过返回值计算
        输出数据的长度
    */
    std::string Decode(const char* Data,int DataByte,int& OutByte);
};

std::string base64_enc(const std::string& text);
std::string base64_dec(const std::string& enc);

std::string base64_enc_bin(uint8_t* data, int size);
std::vector<uint8_t> base64_dec_bin(const std::string& str);

bool base64_dec_bin(const std::string& str, std::vector<uint8_t>& buffer);


std::string base64_enc_ex(const std::string& text, uint8_t x);
std::string base64_dec_ex(const std::string& enc, uint8_t x);
#endif