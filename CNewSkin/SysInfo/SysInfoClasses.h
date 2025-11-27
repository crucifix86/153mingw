//------------------------------------------------------------------------------
// SysInfoClasses.h
//    
//   This file uses #pragma's and #define's that are shared between all 
//   SysInfo classes. In order to link statically, #define 
//   SYSINFO_LINKSTATIC in your project's settings. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef SYSINFOCLASSES_H_
#define SYSINFOCLASSES_H_

//------------------------------------------------------
// this allows us to export/import a class
// to link with the dll, do nothing; 
// to link statically, #define SYSINFO_LINKSTATIC in 
// your project settings
//
#if !defined(SYSINFO_LINKSTATIC)
#   if defined(SYSINFO_EXPORTS)
#      undef DLL_CLASS
#      define DLL_CLASS __declspec(dllexport)
#   else 
#      undef DLL_CLASS
#      define DLL_CLASS __declspec(dllimport)
#      if defined(_DEBUG)
#         pragma comment(lib, "SysInfo_d.lib")
#      else
#         pragma comment(lib, "SysInfo.lib")
#      endif
#   endif
#else
#   define DLL_CLASS
#   if defined(_DEBUG)
#      pragma comment(lib, "SysInfos_d.lib")
#   else
#      pragma comment(lib, "SysInfos.lib")
#   endif
#endif

//-------------------------------------------------------
// this gets rid of annoying 4251 warning about 
// std::string needing dll interface
// it also gets rid of 4800 -- BOOL to bool cast warnings
//
#pragma warning(disable : 4251 4800)

#endif