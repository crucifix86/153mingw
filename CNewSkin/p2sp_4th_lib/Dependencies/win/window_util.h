
#ifndef __window_util_h__
#define __window_util_h__

#include <string>
#include <Windows.h>

// NOTE: Keep these in order so callers can do things like
// "if (GetWinVersion() > WINVERSION_2000) ...".  It's OK to change the values,
// though.
enum WinVersion
{
    WINVERSION_PRE_2000 = 0, // Not supported
    WINVERSION_2000 = 1,     // Not supported
    WINVERSION_XP = 2,
    WINVERSION_SERVER_2003 = 3,
    WINVERSION_VISTA = 4,
    WINVERSION_2008 = 5,
    WINVERSION_WIN7 = 6,
};
// Returns the running version of Windows.
WinVersion GetWinVersion();

// Returns the major and minor version of the service pack installed.
void GetServicePackLevel(int& major, int& minor);

void GetNonClientMetrics(NONCLIENTMETRICS& metrics);

// A version of the GetClassNameW API that returns the class name in an
// std::wstring.  An empty result indicates a failure to get the class name.
std::wstring GetClassName(HWND window);

// Useful for subclassing a HWND.  Returns the previous window procedure.
WNDPROC SetWindowProc(HWND hwnd, WNDPROC wndproc);

// Returns true if the existing window procedure is the same as |subclass_proc|.
bool IsSubclassed(HWND window, WNDPROC subclass_proc);

// Subclasses a window, replacing its existing window procedure with the
// specified one. Returns true if the current window procedure was replaced,
// false if the window has already been subclassed with the specified
// subclass procedure.
bool Subclass(HWND window, WNDPROC subclass_proc);

// Unsubclasses a window subclassed using Subclass. Returns true if
// the window was subclassed with the specified |subclass_proc| and the window
// was successfully unsubclassed, false if the window's window procedure is not
// |subclass_proc|.
bool Unsubclass(HWND window, WNDPROC subclass_proc);

// Retrieves the original WNDPROC of a window subclassed using
// SubclassWindow.
WNDPROC GetSuperclassWNDPROC(HWND window);

// Pointer-friendly wrappers around Get/SetWindowLong(..., GWLP_USERDATA, ...)
// Returns the previously set value.
void* SetWindowUserData(HWND hwnd, void* user_data);
void* GetWindowUserData(HWND hwnd);

#endif //__window_util_h__