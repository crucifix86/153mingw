
#include "p2spCrashReport.h"
#include "p2spException.h"

#include <tlhelp32.h>
#include <time.h>
#include <dbghelp.h>

#include <ShellAPI.h>
#include <ShlObj.h>
#include <atlbase.h>

#include <string>
#include <tchar.h>

#include "./Dependencies/win/path_service.h"
#include "./Dependencies/win/file_util.h"

namespace fagex
{
	//------------------------------------------------------------------------------------------------------
	ExceptionParam _ExceptionParam = { NULL, NULL, NULL};
	VOID SetExceptionParam(ExceptionParam *param)
	{
		_ExceptionParam.hInstance = param->hInstance;
		_ExceptionParam.hWndMainWindow = param->hWndMainWindow;
		_ExceptionParam.fpOnCreatedDmpFile = param->fpOnCreatedDmpFile;
	}
	ExceptionParam& GetExceptionParam(VOID) {
		return _ExceptionParam;
	}
	//------------------------------------------------------------------------------------------------------

	namespace {
#		define	DUMP_SIZE_MAX	8000	//max size of our dump
#		define	CALL_TRACE_MAX	((DUMP_SIZE_MAX - 2000) / (MAX_PATH + 40))	//max number of traced calls
#		define	NL				L"\n"	//new line

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		struct AutoLoadDgbHelp {	//自动加载dbghelp.dll
			typedef	BOOL (WINAPI* pfMiniDumpWriteDump)(
				__in  HANDLE			hProcess,
				__in  DWORD				ProcessId,
				__in  HANDLE			hFile,
				__in  MINIDUMP_TYPE		DumpType,
				__in  PMINIDUMP_EXCEPTION_INFORMATION	ExceptionParam,
				__in  PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
				__in  PMINIDUMP_CALLBACK_INFORMATION	CallbackParam
				);

			AutoLoadDgbHelp() {
				mhDbgHelp = ::LoadLibraryW(L"Dbghelp.dll");
				if(NULL != mhDbgHelp) {
					mpfMiniDumpWriteDump = (pfMiniDumpWriteDump)::GetProcAddress(mhDbgHelp, (LPCSTR)("MiniDumpWriteDump"));
					if(NULL == mpfMiniDumpWriteDump) {
						::FreeLibrary(mhDbgHelp);
						mhDbgHelp = NULL;
					}
				}
			}

			~AutoLoadDgbHelp() {
				if(mhDbgHelp) {
					::FreeLibrary(mhDbgHelp);
					mhDbgHelp = NULL;
				}
				mpfMiniDumpWriteDump = NULL;
			}

			HMODULE				mhDbgHelp;
			pfMiniDumpWriteDump	mpfMiniDumpWriteDump;
		} theDbgHelper;

		//---------------------------------------------------------------------------------------------------
		// Find module by Ret_Addr (address in the module).
		// Return Module_Name (full path) and Module_Addr (start address).
		// Return TRUE if found.
		BOOL WINAPI GetModuleByRetAddr(PBYTE pbRetAddr, WCHAR* wszModuleName, PBYTE& pbModuleAddr)
		{
			MODULEENTRY32W M = {sizeof(MODULEENTRY32W)}; //M.dwSize = sizeof(MODULEENTRY32W);
			HANDLE hSnapshot;

			wszModuleName[0] = 0;
			hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
			if((hSnapshot != INVALID_HANDLE_VALUE) && ::Module32FirstW(hSnapshot, &M)) {
				do {
					if(M.modBaseAddr + M.modBaseSize > pbRetAddr && pbRetAddr > M.modBaseAddr) {
						lstrcpynW(wszModuleName, M.szExePath, MAX_PATH);
						pbModuleAddr = M.modBaseAddr;
						break;
					}
				}while(::Module32NextW(hSnapshot, &M));
			}
			::CloseHandle(hSnapshot);
			return !!wszModuleName[0];
		}

		//---------------------------------------------------------------------------------------------------
		// Fill Str with call stack info.
		// pException can be either GetExceptionInformation() or NULL.
		// If pException = NULL - get current call stack.
		VOID GetCallStack(PEXCEPTION_POINTERS pException, FILE* fp)
		{
			WCHAR wszModuleName[MAX_PATH];
			PBYTE pbModuleAddr;
			PBYTE pbModuleAddr1;

#			pragma warning(disable: 4200)	//nonstandard extension used : zero-sized array in struct/union
			typedef struct tagSTACK {
				tagSTACK*	ebp;
				PBYTE		addr;
				UINT		param[0];
			}STACK, *PSTACK;
#			pragma warning(default: 4200)

			STACK stack = {0, 0};
			PSTACK ebp;

			pbModuleAddr = 0;
			if(pException) {		//fake frame for exception address
				stack.ebp	= (PSTACK)(DWORD_PTR)pException->ContextRecord->Ebp;
				stack.addr	= (PBYTE)pException->ExceptionRecord->ExceptionAddress;
				ebp = &stack;
			} 
			else {
				//压栈前一指针 &pException - (sizeof(tagSTACK) == 8)
				ebp = (PSTACK)&pException - 1;	//frame addr of GetCallStack()

				// Skip frame of GetCallStack(). 如果此堆栈没有被破坏，返回到调用点
				if(!IsBadReadPtr(ebp, sizeof(PSTACK)))
					ebp = ebp->ebp;		//caller ebp
			}

			// Trace CALL_TRACE_MAX calls maximum - not to exceed DUMP_SIZE_MAX.
			// Break trace on wrong stack frame.
			INT iRetAddr;
			for(iRetAddr = 0; 
				(iRetAddr < CALL_TRACE_MAX) && !IsBadReadPtr(ebp, sizeof(PSTACK)) && !IsBadCodePtr(FARPROC(ebp->addr));
				iRetAddr++, ebp = ebp->ebp)
			{
				// If module with Ebp->Ret_Addr found.
				if (GetModuleByRetAddr(ebp->addr, wszModuleName, pbModuleAddr1)) {
					if (pbModuleAddr1 != pbModuleAddr) {	//new module 
						pbModuleAddr = pbModuleAddr1; //Save module's address and full path.
						fwprintf(fp, L"%08X %s"NL, (LONG_PTR)pbModuleAddr, wszModuleName);
					}

					// Save call offset.
					fwprintf(fp, L" +%08X", ebp->addr - pbModuleAddr);

					// Save 5 params of the call. We don't know the real number of params.
					if (pException && !iRetAddr)	//fake frame for exception address
						fwprintf(fp, L"  Exception Offset"NL);
					else if (!IsBadReadPtr(ebp, sizeof(PSTACK) + 5 * sizeof(UINT))) {
						fwprintf(fp, L" (%X, %X, %X, %X, %X)"NL,
							ebp->param[0], ebp->param[1], ebp->param[2], ebp->param[3], ebp->param[4]);
					}
				}
				else fwprintf(fp, L"%08X"NL, (LONG_PTR)(ebp->addr));
			}
		}

		//----------------------------------------------------------------------------------------------------------
		// Fill Str with Windows version.
		VOID WINAPI GetVersionStr(FILE* fp)
		{
			OSVERSIONINFOEXW V = {sizeof(OSVERSIONINFOEXW)};	//EX for NT 5.0 and later
			if(!::GetVersionExW((LPOSVERSIONINFOW)&V))
			{
				ZeroMemory(&V, sizeof(OSVERSIONINFOEXW));
				V.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
				::GetVersionExW((LPOSVERSIONINFOW)&V);
			}

			if (V.dwPlatformId != VER_PLATFORM_WIN32_NT)
				V.dwBuildNumber = LOWORD(V.dwBuildNumber);	//for 9x HIWORD(dwBuildNumber) = 0x04xx

			WCHAR dateBuf[32];
			_wstrdate(dateBuf);

			WCHAR timeBuf[32];
			_wstrtime(timeBuf);

			fwprintf(fp,
				L"=============================================================================="NL
				L"Windows: %d.%d.%d, SP %d.%d, Product Type %d"NL	//SP - service pack, Product Type - VER_NT_WORKSTATION,...
				NL
				L"Time: %s %s"NL,
				V.dwMajorVersion, V.dwMinorVersion, V.dwBuildNumber, V.wServicePackMajor, V.wServicePackMinor, V.wProductType, 
				dateBuf, timeBuf);
		}

		//----------------------------------------------------------------------------------------------------------
		VOID CreateExceptionDesc(PEXCEPTION_POINTERS pException, FILE* fp, DWORD dwLastError)
		{
			if (!pException || !fp) return;

			EXCEPTION_RECORD& E = *pException->ExceptionRecord;
			CONTEXT&		  C = *pException->ContextRecord;

			//取得异常发生地
			WCHAR wszModeleInfo[MAX_PATH];
			WCHAR wszModuleName[MAX_PATH];
			PBYTE pbModuleAddr;

			if(GetModuleByRetAddr((PBYTE)E.ExceptionAddress, wszModuleName, pbModuleAddr))
			{
				_snwprintf(wszModeleInfo, MAX_PATH, L"%s", wszModuleName);
			}
			else
			{
				_snwprintf(wszModeleInfo, MAX_PATH, L"%08X", (DWORD_PTR)(E.ExceptionAddress));
			}

			switch(E.ExceptionCode)
			{
			case 0XE000C0DE:	//转化后的c++异常
				{
					const std::string* pCPPException = (const std::string*)E.ExceptionInformation[0];
					if(!pCPPException) return;
					fwprintf(fp, 
						L"C++ Exception\n"
						L"\n"
						L"Expr:      %s\n", 
						pCPPException->c_str());
				}
				break;

			case EXCEPTION_ACCESS_VIOLATION:	//试图对一个虚地址进行读写
				{
					// Access violation type - Write/Read.
					fwprintf(fp,
						L"\t\tAccess violation\n"
						L"\n"
						L"@:         %s\n"
						L"Operate:   %s\n"
						L"Address:   0x%08X\n"
						L"LastError: 0x%08X\n",
						wszModeleInfo,
						(E.ExceptionInformation[0]) ? L"Write" : L"Read", 
						E.ExceptionInformation[1], 
						dwLastError);
				}
				break;

			default:
				{
					fwprintf(fp,
						L"\t\tOTHER\n"
						L"\n"
						L"@:         %s\n"
						L"Code:      0x%08X\n"
						L"LastError: 0x%08X\n",
						wszModeleInfo,
						E.ExceptionCode, 
						dwLastError);
				}
				break;
			}
		}

		//----------------------------------------------------------------------------------------------------------
		// Allocate Str[DUMP_SIZE_MAX] and return Str with dump, if !pException - just return call stack in Str.
		VOID GetExceptionInfo(PEXCEPTION_POINTERS pException, FILE* fp, DWORD dwLastError)
		{
			WCHAR		wszModuleName[MAX_PATH];
			PBYTE		pbModuleAddr;
			HANDLE		hFile;
			FILETIME	timeLastWrite;
			FILETIME	timeLocalFile;
			SYSTEMTIME	T;

			GetVersionStr(fp);

			fwprintf(fp, L"------------------------------------------------------------------------------"NL);
			fwprintf(fp, L"Process:  ");

			::GetModuleFileNameW(NULL, wszModuleName, MAX_PATH);
			fwprintf(fp, L"%s"NL, wszModuleName);

			// If exception occurred.
			if (pException)
			{
				EXCEPTION_RECORD&	E = *pException->ExceptionRecord;
				CONTEXT&			C = *pException->ContextRecord;

				// If module with E.ExceptionAddress found - save its path and date.
				if (GetModuleByRetAddr((PBYTE)E.ExceptionAddress, wszModuleName, pbModuleAddr))
				{
					fwprintf(fp, L"Module:   %s"NL, wszModuleName);
					hFile = ::CreateFileW(wszModuleName, 
						GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if(hFile != INVALID_HANDLE_VALUE)
					{
						if (::GetFileTime(hFile, NULL, NULL, &timeLastWrite))
						{
							::FileTimeToLocalFileTime(&timeLastWrite, &timeLocalFile);
							::FileTimeToSystemTime(&timeLocalFile, &T);

							fwprintf(fp, L"Date Modified:  %02d/%02d/%d"NL, T.wMonth, T.wDay, T.wYear);
						}
						::CloseHandle(hFile);
					}
				}
				else
				{
					fwprintf(fp, L"Exception Addr:  %08X"NL, (LONG_PTR)(E.ExceptionAddress));
				}
				fwprintf(fp, L"------------------------------------------------------------------------------"NL);

				//加入具体异常解释信息
				CreateExceptionDesc(pException, fp, dwLastError);
			}
			fwprintf(fp, L"------------------------------------------------------------------------------"NL);

			// Save call stack info.
			fwprintf(fp, L"Call Stack:"NL);
			GetCallStack(pException, fp);
		}

		//----------------------------------------------------------------------------------------------------------
		BOOL SaveCurrentlyLog(const WCHAR* wszContent, size_t iContent)
		{
			CFilePath dir;
			if(!PathProvider(DIR_EXE, dir)) {
				ATLVERIFY(PathProvider(DIR_MODULE, dir));
			}
			dir.Append(L"dmp");
			dir.Append(L"pdownload.log");

			SYSTEMTIME TNow = { 0 };
			::GetLocalTime(&TNow);

			FILE* fp = _wfopen(dir.value(), L"at");
			if(!fp) return FALSE;
			fwprintf(fp, NL);
			fwprintf(fp, L"操作描述为:"NL);
			fwprintf(fp, L"%s"NL, wszContent);
			fwprintf(fp, L"Time:%d年%d月%d日%d时%d分%d秒"NL, TNow.wYear,TNow.wMonth,TNow.wDay,TNow.wHour,TNow.wMinute,TNow.wSecond);
			fwprintf(fp, L"------------------------------------------------------------------------------"NL);
			fclose(fp); fp = NULL;

			return TRUE;
		}

		//----------------------------------------------------------------------------------------------------------
		BOOL SaveCurrentlyLog(const CHAR* wszContent, size_t iContent)
		{
			CFilePath dir;
			if(!PathProvider(DIR_COMMON_APP_DATA, dir)) {
				ATLVERIFY(PathProvider(DIR_MODULE, dir));
			}
			dir.Append(L"pwd\\dump\\");
			dir.Append(L"pdownload.log");

			SYSTEMTIME TNow = { 0 };
			::GetLocalTime(&TNow);

			FILE* fp = _wfopen(dir.value(), L"at");
			if(!fp) return FALSE;
			fprintf(fp, "\n");
			fprintf(fp, "操作描述为:\n");
			fprintf(fp, "%s\n", wszContent);
			fprintf(fp, "Time:%d年%d月%d日%d时%d分%d秒\n", TNow.wYear,TNow.wMonth,TNow.wDay,TNow.wHour,TNow.wMinute,TNow.wSecond);
			fprintf(fp, "------------------------------------------------------------------------------\n");
			fclose(fp); fp = NULL;

			return TRUE;
		}
	} //namespace

	//----------------------------------------------------------------------------------------------------------
	BOOL CreateSmallDumpInfo(EXCEPTION_POINTERS* pException, WCHAR* wszSmallFile, unsigned long dwLastError)
	{
		CFilePath dir;
		if(!PathProvider(DIR_COMMON_APP_DATA, dir)) {
			ATLVERIFY(PathProvider(DIR_MODULE, dir));
		}
		dir.Append(L"pwd\\dump\\");
		dir.Append(L"pdownload.log");
		EnsureDirectoryForFile(dir.value(), NULL);

		FILE* fp = _wfopen(dir.value(), L"a+");
		if(!fp) return FALSE;

		CreateExceptionDesc(pException, fp, dwLastError);

		fclose(fp); fp = NULL;

// 		::DialogBoxW(GetExceptionParam().hInstance, MAKEINTRESOURCEW(IDD_DIALOG_ERROR_INFO), GetExceptionParam().hWndMainWindow, ErrorInfoDialogProc);
// 		DWORD dwError = ::GetLastError();

		::GetShortPathNameW(dir.value(), wszSmallFile, MAX_PATH);
		if(wszSmallFile[0] == 0) return FALSE;
		return TRUE;
	}

	//----------------------------------------------------------------------------------------------------------
	BOOL CreateBigInfoFile(EXCEPTION_POINTERS* pException, WCHAR* wszBigFile, unsigned long dwLastError)
	{
		if (!pException) return FALSE;

		WCHAR wszTempDir[MAX_PATH] = {0};
		::GetTempPathW(MAX_PATH, wszTempDir);

		WCHAR wszTempFile[MAX_PATH] = {0};
		::GetTempFileNameW(wszTempDir, L"dtl", MAX_PATH, wszTempFile);

		FILE* fp = _wfopen(wszTempFile, L"w");
		if(!fp) return FALSE;

		GetExceptionInfo(pException, fp, dwLastError);

		fclose(fp); fp = NULL;

		::GetShortPathNameW(wszTempFile, wszBigFile, MAX_PATH);
		if(wszBigFile[0] == 0) return FALSE;

		return TRUE;
	}

	//创建DBGHLEP所需要的dmp信息
	//----------------------------------------------------------------------------------------------------------
	BOOL CreateDumpHelpFile(EXCEPTION_POINTERS* pException, WCHAR* wszDumpFile)
	{
		wszDumpFile[0] = ' ';
		wszDumpFile[1] = 0;

		// If MiniDumpWriteDump() of DbgHelp.dll available.
		if (!theDbgHelper.mpfMiniDumpWriteDump) return FALSE;

		WCHAR wszTempDir[MAX_PATH] = {0};
		::GetTempPathW(MAX_PATH, wszTempDir);

		WCHAR wszTempFile[MAX_PATH] = {0};
		::GetTempFileNameW(wszTempDir, L"dmp", MAX_PATH, wszTempFile);

		MINIDUMP_EXCEPTION_INFORMATION	M;
		M.ThreadId = ::GetCurrentThreadId();
		M.ExceptionPointers = pException;
		M.ClientPointers = 0;

		HANDLE hDumpFile = ::CreateFileW(wszTempFile,
			GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		theDbgHelper.mpfMiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hDumpFile,
			MiniDumpNormal, (pException) ? &M : NULL, NULL, NULL);

		::CloseHandle(hDumpFile);

		::GetShortPathNameW(wszTempFile, wszDumpFile, MAX_PATH);
		if(wszDumpFile[0] == 0) return FALSE;
		return TRUE;
	}
}