/********************************************************************
	created:	2005/10/12
	created:	12:10:2005   11:10
	file name:	TPBase.h
	author:		yaojun

	purpose:

	Modified for GCC/MinGW compatibility:
	- Rewrote IsSameType to avoid explicit specialization in class scope
*********************************************************************/

#pragma once

struct NullType {};

// GCC-compatible implementation of IsSameType
template<typename T1, typename T2>
struct IsSameType
{
	enum { value = 0 };
};

template<typename T>
struct IsSameType<T, T>
{
	enum { value = 1 };
};

template<bool Value>
struct Bool2Type
{
	enum { value = Value };
};

template<DWORD Value>
struct DWORD2Type
{
	enum { value = Value };
};

template<int Value>
struct Int2Type
{
	enum { value = Value };
};

template<typename TType>
struct Type2Type
{
	typedef TType OriginalType;
};




template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> {};
#define STATIC_CHECK(expr, msg) \
{ CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }

#define STATIC_DWORD_INDEX(index) (DWORD2Type<index>())
