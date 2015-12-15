#ifndef _BASE64_H_
#define _BASE64_H_
#include <string>

class ZBase64
{
public:
    /*����
    DataByte
        [in]��������ݳ���,���ֽ�Ϊ��λ
    */
    std::string Encode(const unsigned char* Data,int DataByte);
    /*����
    DataByte
        [in]��������ݳ���,���ֽ�Ϊ��λ
    OutByte
        [out]��������ݳ���,���ֽ�Ϊ��λ,�벻Ҫͨ������ֵ����
        ������ݵĳ���
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