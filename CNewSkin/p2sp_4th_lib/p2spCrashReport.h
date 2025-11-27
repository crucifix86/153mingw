
/************************************************************\
*          程序崩溃时保存信息并启动CrachReport               *
*                                                            *
*                                                            *
*   ref from http://www.codeproject.com/tools/minidump.asp   *
*       By Vladimir Sedach                                   *
*                                                            *
\************************************************************/
#ifndef __fagexCrashReport_h__
#define __fagexCrashReport_h__

#pragma once
#include <windows.h>

//generate stack frame poINTers for all functions - same as /Oy- in the project
#pragma optimize("y", off)		

namespace fagex
{
	BOOL CreateSmallDumpInfo(EXCEPTION_POINTERS* pException, WCHAR* wszSmallFile, DWORD dwLastError);

	BOOL CreateBigInfoFile(EXCEPTION_POINTERS* pException, WCHAR* wszBigFile, DWORD dwLastError);

	BOOL CreateDumpHelpFile(EXCEPTION_POINTERS* pException, WCHAR* wszDumpFile);

} //namespace fagex

#endif //__fagexCrashReport_h__
