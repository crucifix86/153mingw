#ifndef __fagexWinUtility_h__
#define __fagexWinUtility_h__

#include "fagexPrerequisites.h"
#include "fagexStdHeaders.h"
#include "fagexStringUtil.h"

#include <windows.h>
#include <iostream>
#include <stdarg.h>

namespace fagex
{
	class WinUtility {
	public:
		static std::wstring GetWinAppExecutePath(HINSTANCE hInstance)
		{
			wchar_t wszPath[MAX_PATH] = { 0 };
			::GetModuleFileNameW(hInstance, wszPath, MAX_PATH);
			*(wcsrchr(wszPath, '\\') + 1) = '\0';
			return std::wstring(wszPath, wcslen(wszPath));
		}

		//------------------------------------------------------------------------------------------
		static std::wstring FormatWin32Error(DWORD dw)
		{
			LPVOID lpMsgBuf; 
			FormatMessageW( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS, 
				NULL, 
				dw, 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPWSTR) &lpMsgBuf, 
				0, 
				NULL 
				); 
			std::wstring ret = (wchar_t*)lpMsgBuf;
			// Free the buffer.
			LocalFree( lpMsgBuf );
			return ret;
		}

		static BOOL ReadFile(const WCHAR* fileName, std::string& strBuffer)
		{
			strBuffer.clear();

			FILE *fp = NULL;
			_wfopen_s(&fp, fileName, L"rb");
			if(!fp) return FALSE;

			const int BUF_LENGTH_4K = 4 * 1024;
			char szBuffer[BUF_LENGTH_4K] = { 0 };
			while(!feof(fp))
			{
				size_t iReadCount = fread(szBuffer, 1, BUF_LENGTH_4K, fp);
				strBuffer.append(szBuffer, iReadCount);
			}
			fclose(fp);
			return TRUE;
		}

		static BOOL ReadFileW(const std::wstring& wsFilename, std::wstring& wsBuffer) 
		{
			wsBuffer.clear();

			FILE *fp = NULL;
			_wfopen_s(&fp, wsFilename.c_str(), L"rb");
			if(!fp) return FALSE;

			const int BUF_LENGTH_4K = 4 * 1024;
			wchar_t wszBuffer[BUF_LENGTH_4K] = { 0 };
			while(!feof(fp))
			{
				size_t iReadCount = fread(wszBuffer, 2, BUF_LENGTH_4K, fp);
				wsBuffer.append(wszBuffer, iReadCount);
			}
			fclose(fp);
			return TRUE;
		}

		static void stdstring_format(std::string& s, const char *format, ...) {
			va_list ap;
			va_start(ap, format);
			StringUtil::stdstring_svformat(s, format, ap);
			va_end(ap);
		}

		static void stdstring_svformat(std::string& s, const char *format, va_list ap) {
			size_t len = ::_vscprintf(format, ap) + 1;
			char* buffer = new char[len];
			memset(buffer, 0, len);
			::vsprintf_s(buffer, len, format, ap);
			s.assign(buffer, len);
			delete buffer;
		}
	};
}

#endif //__fagexWinUtility_h__