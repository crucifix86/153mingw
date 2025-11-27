// Stub implementations for defence (anti-cheat) and other excluded features
// These are needed because they use MSVC SEH, inline asm, and Windows-specific APIs
// that are not available with MinGW

#include <windows.h>

// Include the existing headers for namespaces
#include "defence/cheaterkiller.h"
#include "defence/imagechecker.h"
#include "defence/stackchecker.h"

// CheaterKiller namespace stubs
namespace CheaterKiller {
    void WINAPI AppendCheatInfo(LPVOID buf) {
        // Stub - anti-cheat disabled
    }

    void WINAPI TryKillCheaters() {
        // Stub - anti-cheat disabled
    }
}

// ImageChecker namespace stubs
namespace ImageChecker {
    void ACImageCheckerInit() {
        // Stub - image checking disabled
    }

    void CheckImage() {
        // Stub - image checking disabled
    }

    DWORD WINAPI UpdateImageChecker(void*) {
        // Stub - image checking disabled
        return 0;
    }
}

// StackChecker namespace stubs
namespace StackChecker {
    void ACStackCheckInit() {
        // Stub - stack checking disabled
    }

    void ACTrace(int id) {
        // Stub - stack tracing disabled
    }

    VOID WINAPI GetStackCheckData(void*) {
        // Stub - stack checking disabled
    }

    VOID WINAPI UpdateStackCheckCode(void*) {
        // Stub - stack checking disabled
    }
}

// Collector class definition and stubs
// This class is used for collecting system info for anti-cheat
class Collector {
public:
    static int __stdcall GetNextInfoType();
    static int __stdcall GetInfo(int type, unsigned char** data, const char* a, const char* b);
    static int __stdcall GetInfoNetDLL(unsigned char** data);
    static int __stdcall GetInfoNetDLLEx(unsigned char** data);
};

int __stdcall Collector::GetNextInfoType() {
    return 0;
}

int __stdcall Collector::GetInfo(int type, unsigned char** data, const char* a, const char* b) {
    return 0;
}

int __stdcall Collector::GetInfoNetDLL(unsigned char** data) {
    return 0;
}

int __stdcall Collector::GetInfoNetDLLEx(unsigned char** data) {
    return 0;
}
