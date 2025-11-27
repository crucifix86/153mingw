
#include "window_util.h"

#include <atlbase.h>

#define SIZEOF_STRUCT_WITH_SPECIFIED_LAST_MEMBER(struct_name, member) \
    offsetof(struct_name, member)+(sizeof static_cast<struct_name*>(NULL)->member)
#define NONCLIENTMETRICS_SIZE_PRE_VISTA \
    SIZEOF_STRUCT_WITH_SPECIFIED_LAST_MEMBER(NONCLIENTMETRICS, lfMessageFont)

WinVersion GetWinVersion()
{
    static bool checked_version = false;
    static WinVersion win_version = WINVERSION_PRE_2000;
    if(!checked_version)
    {
        OSVERSIONINFOEX version_info;
        version_info.dwOSVersionInfoSize = sizeof(version_info);
        ::GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&version_info));
        if(version_info.dwMajorVersion == 5)
        {
            switch(version_info.dwMinorVersion)
            {
            case 0:
                win_version = WINVERSION_2000;
                break;
            case 1:
                win_version = WINVERSION_XP;
                break;
            case 2:
            default:
                win_version = WINVERSION_SERVER_2003;
                break;
            }
        }
        else if(version_info.dwMajorVersion == 6)
        {
            if(version_info.wProductType != VER_NT_WORKSTATION)
            {
                // 2008 is 6.0, and 2008 R2 is 6.1.
                win_version = WINVERSION_2008;
            }
            else
            {
                if(version_info.dwMinorVersion == 0)
                {
                    win_version = WINVERSION_VISTA;
                }
                else
                {
                    win_version = WINVERSION_WIN7;
                }
            }
        }
        else if(version_info.dwMajorVersion > 6)
        {
            win_version = WINVERSION_WIN7;
        }
        checked_version = true;
    }

    return win_version;
}

void GetServicePackLevel(int& major, int& minor)
{
    static bool checked_version = false;
    static int service_pack_major = -1;
    static int service_pack_minor = -1;
    if(!checked_version)
    {
        OSVERSIONINFOEX version_info = { 0 };
        version_info.dwOSVersionInfoSize = sizeof(version_info);
        ::GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&version_info));
        service_pack_major = version_info.wServicePackMajor;
        service_pack_minor = version_info.wServicePackMinor;
        checked_version = true;
    }

    major = service_pack_major;
    minor = service_pack_minor;
}

void GetNonClientMetrics(NONCLIENTMETRICS& metrics)
{
    static const UINT SIZEOF_NONCLIENTMETRICS = (GetWinVersion()>=WINVERSION_VISTA) ?
        sizeof(NONCLIENTMETRICS) : NONCLIENTMETRICS_SIZE_PRE_VISTA;
    metrics.cbSize = SIZEOF_NONCLIENTMETRICS;
    const bool success = !!::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
        SIZEOF_NONCLIENTMETRICS, &metrics, 0);
    ATLASSERT(success);
}

std::wstring GetClassName(HWND window)
{
    wchar_t class_name[MAX_PATH] = { 0 };
    ::GetClassNameW(window, class_name, MAX_PATH);
    return std::wstring(class_name);
}

#pragma warning(push)
#pragma warning(disable:4312 4244)

WNDPROC SetWindowProc(HWND hwnd, WNDPROC proc)
{
    // The reason we don't return the SetwindowLongPtr() value is that it returns
    // the orignal window procedure and not the current one. I don't know if it is
    // a bug or an intended feature.
    WNDPROC oldwindow_proc =
        reinterpret_cast<WNDPROC>(GetWindowLongPtr(hwnd, GWLP_WNDPROC));
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(proc));
    return oldwindow_proc;
}

void* SetWindowUserData(HWND hwnd, void* user_data)
{
    return reinterpret_cast<void*>(SetWindowLongPtr(hwnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>(user_data)));
}

void* GetWindowUserData(HWND hwnd)
{
    return reinterpret_cast<void*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

// Maps to the WNDPROC for a window that was active before the subclass was
// installed.
static const wchar_t* const kHandlerKey = L"__ORIGINAL_MESSAGE_HANDLER__";

bool IsSubclassed(HWND window, WNDPROC subclass_proc)
{
    WNDPROC original_handler =
        reinterpret_cast<WNDPROC>(GetWindowLongPtr(window, GWLP_WNDPROC));
    return original_handler == subclass_proc;
}

bool Subclass(HWND window, WNDPROC subclass_proc)
{
    WNDPROC original_handler =
        reinterpret_cast<WNDPROC>(GetWindowLongPtr(window, GWLP_WNDPROC));
    if(original_handler != subclass_proc)
    {
        SetWindowProc(window, subclass_proc);
        ::SetPropW(window, kHandlerKey, original_handler);
        return true;
    }
    return false;
}

bool Unsubclass(HWND window, WNDPROC subclass_proc)
{
    WNDPROC current_handler =
        reinterpret_cast<WNDPROC>(GetWindowLongPtr(window, GWLP_WNDPROC));
    if(current_handler == subclass_proc)
    {
        HANDLE original_handler = ::GetPropW(window, kHandlerKey);
        if(original_handler)
        {
            ::RemovePropW(window, kHandlerKey);
            SetWindowProc(window, reinterpret_cast<WNDPROC>(original_handler));
            return true;
        }
    }
    return false;
}

WNDPROC GetSuperclassWNDPROC(HWND window)
{
    return reinterpret_cast<WNDPROC>(::GetPropW(window, kHandlerKey));
}