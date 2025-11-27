
#ifndef __string_printf_h__
#define __string_printf_h__

#include <string>
#include <stdarg.h>
#include <vector>

// Return a C++ string given printf-like input.(ÏÞÖÆ:<=32M)
std::string StringPrintf(const char* format, ...);
std::wstring StringPrintf(const wchar_t* format, ...);

void StringAppendV(std::string* dst, const char* format, va_list ap);
void StringAppendV(std::wstring* dst, const wchar_t* format, va_list ap);

#endif //__string_printf_h__