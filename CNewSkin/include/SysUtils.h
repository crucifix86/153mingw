//------------------------------------------------------------------------------
// SysUtils.h
//    
//   This file contains generically useful system utilities.
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef SYSUTILS_H_
#define SYSUTILS_H_

#include "SysInfoClasses.h"
#include "windows.h"
#include <string>

namespace SysUtils
{
   std::string ByteToStr(const __int64& n64ByteCount = 0, const int& nPrecision = 2 );
};

#endif