// Linker stubs for missing symbols
// These provide empty implementations for functionality that is disabled
// or not available in the MinGW cross-compilation build

#include <windows.h>
#include <basetyps.h>
#include <initguid.h>  // DEFINE_GUID implementation
#include <stdlib.h>

// Forward declare the Unicode entry point
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);

// WinMain shim that calls wWinMain
// MinGW with UNICODE needs this bridge because its CRT calls WinMain
// but the application defines wWinMain
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}

// NetDLL namespace - network anti-cheat DLL parsing
namespace NetDLL {
    int __stdcall Parse(void* data, unsigned long size) {
        // Anti-cheat disabled
        return 0;
    }
}

// stack_trace function - from infocollection.h/defence system
// Used for collecting stack trace information for anti-cheat
// Declared as C++ function (not extern "C") to match the declaration in infocollection.h
void stack_trace(LPCVOID sv0, LPCVOID sv1) {
    // Stack tracing disabled
}

// Additional stubs may be added here as needed
