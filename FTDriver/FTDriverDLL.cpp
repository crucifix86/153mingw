// FTDriverDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "FTDriverDLL.h"
#include "FTInterface.h"

// Pure virtual destructor definitions (required by C++ standard)
IFTFont::~IFTFont() {}
IFTManager::~IFTManager() {}

// DllMain is only needed when building as a DLL, not as a static library
#if defined(_WINDLL) || defined(BUILD_DLL)
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif
