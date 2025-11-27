/****************************************\
*										*
*				异常相关				*
*										*
\****************************************/
#ifndef __fagexException_h__
#define __fagexException_h__

#pragma once
#include <windows.h>

// ref from http://www.codeproject.com/tools/minidump.asp 
//generate stack frame pointers for all functions - same as /Oy- in the project
#pragma optimize("y", off)

namespace fagex
{
	typedef VOID (*_fpOnCreatedDmpFile)(CONST WCHAR* wszDmpFile);
	typedef struct tagExceptionParam {
		HINSTANCE hInstance;
		HWND	  hWndMainWindow;
		_fpOnCreatedDmpFile fpOnCreatedDmpFile;
	}ExceptionParam;
	VOID SetExceptionParam(ExceptionParam *param);
	ExceptionParam& GetExceptionParam(VOID);

	//分析SEH异常
	VOID ProcessException(EXCEPTION_POINTERS* pException, BOOL bRunCrashReport);

	//抛出字符串形式异常
	VOID ExceptionThrow(CONST CHAR* wszFormat, ...);

	//抛出断言形式异常
	VOID ExceptionThrowAssert(CONST CHAR* szFileName, INT nFileLine, CONST CHAR* pExpr);

#	define FAGEX_EXCEPTION	fagex::ExceptionThrow
#	define FAGEX_EXCEPTION_ASSERT(exp)	\
	do { \
	if(!(exp)) { \
	fagex::ExceptionThrowAssert(__FILE__, __LINE__, #exp); \
	} \
	} while(FALSE)

	//显示一个内部异常
	INT ProcessInnerException(EXCEPTION_POINTERS* pException, VOID* hParentWnd, CONST WCHAR* wszTitle);

} //namespace fagex

#endif //__fagexException_h__