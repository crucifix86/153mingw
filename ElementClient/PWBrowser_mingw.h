// PWBrowser_mingw.h - MinGW stub version of PWBrowser.h
// This provides minimal declarations when ATL is not available
// Most class definitions are in DlgExplorer.h

#ifndef PWBRWOSER_H
#define PWBRWOSER_H

#include <windows.h>
#include "DlgBase.h"
#include <vector.h>
#include <AString.h>

// Forward declarations only - actual class definitions are in DlgExplorer.h
class CDlgExplorer;
class CExplorerMan;

// Stub CPWMessageTranslate - only define if not already defined
#ifndef CPWMESSAGETRANSLATE_DEFINED
#define CPWMESSAGETRANSLATE_DEFINED
class CPWMessageTranslate
{
public:
    virtual ~CPWMessageTranslate() {}
    virtual bool MessageTranslate(AString Control, UINT msg, WPARAM wParam, LPARAM lParam) { return false; }
    virtual BOOL PeekMessage_Filter(LPMSG lpMsg, HWND hWnd) { return FALSE; }
};
#endif

// Stub functions
inline BOOL InitBrowser() { return TRUE; }
inline BOOL FreeBrowser() { return TRUE; }

#define STATUS_TXT_LEN 256
#define USE_GLOBAL_BROWSER 1

// Stub CPWBrowser class
class CPWBrowser
{
public:
    CPWBrowser() : m_hWnd(NULL) {}
    CPWBrowser(CExplorerMan* pParent) : m_hWnd(NULL) {}
    virtual ~CPWBrowser() {}

    BOOL InitContainer(HINSTANCE hInstance) { return FALSE; }
    BOOL InitContainer() { return FALSE; }
    HWND GetHandle() { return m_hWnd; }
    HWND Create(HWND hWndParent, RECT& rcPos) { return NULL; }
    VOID CreateX(HWND hWndParent, RECT& rcPos) {}
    HWND CreateInOtherThread(HWND hWndParent, RECT& rcPos) { return NULL; }
    BOOL SetWindowTextA(LPCSTR lpString) { return FALSE; }
    BOOL SetWindowTextW(LPCWSTR lpString) { return FALSE; }
    BOOL IsReady() { return FALSE; }
    BOOL MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE) { return FALSE; }
    BOOL MoveWindowOld() { return FALSE; }
    BOOL AddDownloadUrl(WCHAR *FileUrl) { return FALSE; }
    BOOL CanDelete(DWORD curThreadID) { return TRUE; }
    void DelayDestroy() {}
    void NavigateUrl(const WCHAR *Url) {}
    void NavigateUrl(const CHAR *Url) {}
    void NavigateUrl(VARIANT *Url) {}
    void GoBackward() {}
    void GoForward() {}
    void Refresh() {}
    void FixIE6() {}
    void GetCurrentUrl() {}
    BOOL ShowWindow(int nCmdShow) { return FALSE; }
    void OnBrowserQuit() {}
    void SetDocReady(BOOL bReady) {}
    HWND GetCtrl() { return NULL; }
    void SetActive(bool bActive) {}
    static VOID TermContainer() {}
    BOOL IsDocReady() { return FALSE; }
    VOID WaitReady() {}
    BOOL IsFirstReady() { return FALSE; }
    void SetFirstReady() {}
    void UpdateCurrentUrl() {}
    void OnTick() {}
    void SetScrollHeight(int nHeight) {}
    int GetScrollHeight() { return 0; }
    void SetScrollPos(int nPos) {}
    int GetScrollPos() { return 0; }
    void SetExplorer(CExplorerMan* pPtr) {}
    void MoveScrollPos(int nDelta) {}

    // Event callbacks (stubs)
    void OnBeforeNavigate2(IDispatch** pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel) {}
    void OnNavigateComplete2(IDispatch** pDisp, VARIANT* URL) {}
    void OnNewWindow2(IDispatch** ppDisp, VARIANT_BOOL* Cancel) {}
    void OnNewWindow3(IDispatch** ppDisp, VARIANT_BOOL* Cancel, unsigned long dwFlags, BSTR bstrUrlContext, BSTR bstrUrl) {}
    void OnWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL* Cancel) {}
    void OnQuit() {}
    void DocumentComplete(IDispatch* pDisp, VARIANT* URL) {}
    void OnNavigateError(IDispatch* pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, VARIANT_BOOL* Cancel) {}
    void OnFileDownLoad(VARIANT_BOOL ActiveDocument, VARIANT_BOOL* Cancel) {}

protected:
    HWND m_hWnd;
};

#endif // PWBRWOSER_H
