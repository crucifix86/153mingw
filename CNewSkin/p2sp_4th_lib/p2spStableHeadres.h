

#ifndef __p2spStableHeadres_h__
#define __p2spStableHeadres_h__

#include <atlbase.h>
#include <atlstr.h>

#include "fagex/fagexPrerequisites.h"
#include "fagex/fagexSingleton.h"
#include "fagex/fagexStdHeaders.h"
#include "fagex/fagexStringUtil.h"
#include "fagex/fagexWinUtility.h"

#include "win/file_util.h"
#include "win/file_version_info.h"
#include "win/http_request.h"
#include "win/http_upload.h"
#include "win/netcard_info.h"
#include "win/path_service.h"
#include "win/registry.h"
#include "win/string_printf.h"
#include "win/window_util.h"
#include "win/string_conversion.h"

#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"

#include "SimpleIni.h"
#include "md5.h"
#include "lzari.h"

#pragma comment(lib, "ws2_32.lib")

//BaseType
namespace p2sp4 {
	static const std::string   sNullString = "";
	static const std::wstring wsNullString = L"";
}

namespace p2sp4 {

#ifdef QA_CLIENT
	static const char* szConfigXML_URL		= "http://115.182.50.74/qa_p2sp.xml";
#else
	static const char* szConfigXML_URL		= "http://task.p2sp.wanmei.com/p2sp_n/p2sp.xml";
#endif

	static const wchar_t* wszEngineDllName	= L"p2sp_engine.dll";
	static const wchar_t* wszRelativelyPath = L"p2sp_update";
	static const wchar_t* wszUpdateToolName = L"p2sp_4th_lib_o.dll";

	struct tUpdateFileInfo
	{
		std::wstring	wsFilename;			//p2sp_engine.dll
		std::wstring	wsMD5;				//94d91edd1ef2e80b6b3eff75bb28f1bc
		std::wstring	wsZipFilename;		//p2sp_engine.dll.zip
		std::wstring	wsZipMD5;
		std::wstring	wsUpdateMode;		//cover
		std::wstring	wsAction;
		std::wstring	wsRestartExe;		//1
		std::wstring	wsUpdateURL;
	};
}
#endif //__p2spStableHeadres_h__