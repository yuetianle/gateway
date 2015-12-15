#include "stdafx.h"
#include "base64.h"

using namespace std;
string ZBase64::Encode(const unsigned char* Data,int DataByte)
{
	//编码表
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	string strEncode;
	unsigned char Tmp[4]={0};
	int LineLength=0;
	for(int i=0;i<(int)(DataByte / 3);i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode+= EncodeTable[Tmp[1] >> 2];
		strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode+= EncodeTable[Tmp[3] & 0x3F];
		if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}
	}
	//对剩余数据进行编码
	int Mod=DataByte % 3;
	if(Mod==1)
	{
		Tmp[1] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode+= "==";
	}
	else if(Mod==2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode+= "=";
	}

	return strEncode;
}

string ZBase64::Decode(const char* Data,int DataByte,int& OutByte)
{
	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	//返回值
	string strDecode;
	int nValue;
	int i= 0;
	while (i < DataByte)
	{
		if (*Data != '\r' && *Data!='\n')
		{
			nValue = DecodeTable[*Data++] << 18;
			nValue += DecodeTable[*Data++] << 12;
			strDecode+=(nValue & 0x00FF0000) >> 16;
			OutByte++;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++] << 6;
				strDecode+=(nValue & 0x0000FF00) >> 8;
				OutByte++;
				if (*Data != '=')
				{
					nValue += DecodeTable[*Data++];
					strDecode+=nValue & 0x000000FF;
					OutByte++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			Data++;
			i++;
		}
	}
	return strDecode;
}

std::string base64_enc(const string& text)
{
	ZBase64 zb64;
	return zb64.Encode((uint8_t*)text.c_str(), text.size());
}


std::string base64_dec(const string& enc)
{
	ZBase64 zb64;
	int outbytes = 0;
	return zb64.Decode(enc.c_str(), enc.size(), outbytes);
}


std::string base64_enc_ex(const string& text, uint8_t x)
{
	ZBase64 zb64;
	string ss = zb64.Encode((uint8_t*)text.c_str(), text.size());
	for (auto& c : ss)
	{
		c ^= x;
	}
	string enc = zb64.Encode((uint8_t*)ss.c_str(), ss.size());
	return enc;
}

std::string base64_dec_ex(const string& enc, uint8_t x)
{
	ZBase64 zb64;
	int outbytes = 0;
	string ss = zb64.Decode(enc.c_str(), enc.size(), outbytes);
	for (auto& c : ss)
	{
		c ^= x;
	}
	string dec = zb64.Decode(ss.c_str(), ss.size(), outbytes);
	return dec;
}

std::string base64_enc_bin(uint8_t* data, int size)
{
	ZBase64 zb64;
	return zb64.Encode(data, size);
}

std::vector<uint8_t> base64_dec_bin(const string& str)
{
	ZBase64 zb64;
	int outbytes = 0;

	string ret = zb64.Decode(str.c_str(), str.size(), outbytes);
	std::vector<uint8_t> output(outbytes);
	memcpy(output.data(), ret.data(), outbytes);
	return output;
}

bool base64_dec_bin(const string& str, std::vector<uint8_t>& buffer)
{
	if (str.length() > 0)
	{
		ZBase64 zb64;
		int outbytes = 0;
		string ret = zb64.Decode(str.c_str(), str.size(), outbytes);
		buffer.resize(outbytes);
		memcpy(buffer.data(), ret.data(), outbytes);
		return true;
	}
	return false;
}
