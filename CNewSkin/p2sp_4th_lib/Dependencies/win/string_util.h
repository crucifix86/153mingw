

#ifndef __string_util_h__
#define __string_util_h__

// 转换任意字符类型到对应的无符号类型.
// 例如: char, signed char, unsigned char -> unsigned char.
template<typename T>
struct ToUnsigned
{
	typedef T Unsigned;
};

template<>
struct ToUnsigned<char>
{
	typedef unsigned char Unsigned;
};
template<>
struct ToUnsigned<signed char>
{
	typedef unsigned char Unsigned;
};
template<>
struct ToUnsigned<wchar_t>
{
	typedef unsigned short Unsigned;
};
template<>
struct ToUnsigned<short>
{
	typedef unsigned short Unsigned;
};

#endif //__string_util_h__
