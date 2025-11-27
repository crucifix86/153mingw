
#include "p2spException.h"
#include "p2spCrashReport.h"

#include <stdio.h>
#include <string>
#include <tchar.h>

#include <ShellAPI.h>
#include <ShlObj.h>
#include <atlbase.h>

namespace fagex
{
	//--------------------------------------------------------------------------------------------------------------
	std::wstring FormatMessage(DWORD dwError)
	{
		LPVOID lpMsgBuf; 
		::FormatMessageW( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			dwError, //GetLastError(), 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPWSTR) &lpMsgBuf, 
			0, 
			NULL 
			); 
		std::wstring ret = (wchar_t*)lpMsgBuf;
		::LocalFree( lpMsgBuf );
		return ret;
	}

	//------------------------------------------------------------------------------------------------------
	//分析SEH异常
	VOID ProcessException(EXCEPTION_POINTERS* pException, BOOL bRunCrashReport) throw() //保证不抛出任何异常
	{
		//保存最后的错误代码
		DWORD dwLastError = ::GetLastError();
		if(!pException) return;

		//生成基本描述文件
		WCHAR wszSmallInfoFile[MAX_PATH] = {0};
		if(!CreateSmallDumpInfo(pException, wszSmallInfoFile, dwLastError))
		{
			//return;
		}

		//生成完整表述文件
		WCHAR wszBigInfoFile[MAX_PATH] = {0};
		if(!CreateBigInfoFile(pException, wszBigInfoFile, dwLastError))
		{
			//return;
		}

		//生成dump报告
		WCHAR wszDumpFile[MAX_PATH] = {0};
		BOOL ok = CreateDumpHelpFile(pException, wszDumpFile);
		if(ok) {
			::OutputDebugStringW(wszDumpFile);
			MessageBoxW(NULL, wszDumpFile, L"抱歉, 发生一个异常", MB_OK);
		}
	}

	//------------------------------------------------------------------------------------------------
	VOID ExceptionThrow(CONST CHAR* format, ...)
	{
		va_list ap;
		va_start(ap, format);
		size_t len = _vscprintf(format, ap);
		std::string message;
		message.resize(len + 1, ' ');
		vsprintf((char*)&(message[0]), format, ap);
		va_end(ap);
		throw std::exception(message.c_str());
	}

	//------------------------------------------------------------------------------------------------
	VOID ExceptionThrowAssert(CONST CHAR* szFileName, INT nFileLine, CONST CHAR* pExpr)
	{
		const char *szAssertFmt = 
			"Assert	\r\n"
			"\r\n"
			"File:     %s\r\n"
			"Line:     %d\r\n"
			"Expr:     %s\r\n";

		size_t len = _scprintf(szAssertFmt, szFileName, nFileLine, pExpr);
		std::string message;
		message.resize(len + 1, ' ');
		sprintf((char*)&(message[0]), szAssertFmt, szFileName, nFileLine, pExpr);
		throw std::exception(message.c_str());
	}

	INT ProcessInnerException(EXCEPTION_POINTERS* pException, VOID* hParentWnd, CONST WCHAR* wszTitle)
	{
		ProcessException(pException, FALSE);
		return 0;
	}
}


