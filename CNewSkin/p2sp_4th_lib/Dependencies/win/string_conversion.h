
#ifndef __string_conversion_h__
#define __string_conversion_h__

#include <string>

// Converts a UTF8 string to UTF16.
std::wstring UTF8ToWide(const std::string& utf8);

// Converts a UTF16 string to UTF8.
std::string WideToUTF8(const std::wstring& wide);

// 通过截断方式转化到7-bit ASCII码, 需要预先知道转化前内容都是ASCII码.
std::string WideToASCII(const std::wstring& wide);

// 转化ASCII字符串(通常是字符串常量)到宽字节字符串.
std::wstring ASCIIToWide(const std::string& ascii);

#endif //__string_conversion_h__