// Stub implementation for CDlgExplorer and CDlgLoginOther
// These classes require ATL which is not available with MinGW
// This provides minimal stubs to satisfy the linker

#include "DlgExplorer.h"
#include "DlgLoginOther.h"

// Note: InitBrowser/FreeBrowser are in PWBrowser_stub.cpp

// CExplorerMan stubs
CExplorerMan::CExplorerMan() : m_pBrowser(NULL), m_pDlg(NULL), m_hDownloadThread(NULL),
    m_bDownLoading(false), m_bUpdateDownLoadState(false), m_bUpdateDownData(false),
    m_bInTimer(false), m_bCancelCurDownLoad(false), m_dwDownloadThreadId(0),
    m_mutex(NULL), m_event(NULL) {
    memset(m_szDownStatusText, 0, sizeof(m_szDownStatusText));
    memset(m_wsPath, 0, sizeof(m_wsPath));
}
CExplorerMan::~CExplorerMan() {}
bool CExplorerMan::OnInitDialog() { return false; }
BOOL CExplorerMan::CreateBrowser() { return FALSE; }
void CExplorerMan::HideBrowser() {}
void CExplorerMan::ShowBrowser() {}
BOOL CExplorerMan::AddDownloadUrl(WCHAR *FileUrl) { return FALSE; }
void CExplorerMan::ThreadRun() {}
BOOL CExplorerMan::InitTaskList() { return FALSE; }
void CExplorerMan::NavigateUrl(WCHAR *URL) {}
void CExplorerMan::GoForward() {}
void CExplorerMan::GoBackward() {}
void CExplorerMan::Refresh() {}
bool CExplorerMan::GetDownloadState(abase::vector<AWString>& vecTaskDone, abase::vector<AWString>& vecTaskToDo, AWString& curDownFile, int &curDownProgress) { return false; }
double CExplorerMan::GetDownloadRate() { return 0.0; }
bool CExplorerMan::IsDownloading() { return false; }
void CExplorerMan::CancelCurDownLoad() {}
void CExplorerMan::OnWinTimer() {}
void CExplorerMan::OnTick() {}
void CExplorerMan::SetDownloadStatus(ULONG ulProgress,ULONG ulProgressMax,LONG ulStatusCode,LPCWSTR szStatusText) {}
BOOL CExplorerMan::GetDownloadingUrlFileSize(UINT* plength) { return FALSE; }
BOOL CExplorerMan::BindUI(CDlgExplorer *pDlg) { return FALSE; }
BOOL CExplorerMan::ReleaseUI() { return FALSE; }

// CBSCallbackImpl stubs
CBSCallbackImpl::CBSCallbackImpl(CExplorerMan* pDlgExplorer, HANDLE hEventStop)
    : m_pDlgExplorer(pDlgExplorer), m_hEventStop(hEventStop), m_ulObjRefCount(1), m_filesize(0) {}
STDMETHODIMP CBSCallbackImpl::QueryInterface(REFIID riid, void **ppvObject) { return E_NOINTERFACE; }
STDMETHODIMP_(ULONG) CBSCallbackImpl::AddRef() { return ++m_ulObjRefCount; }
STDMETHODIMP_(ULONG) CBSCallbackImpl::Release() { if (--m_ulObjRefCount == 0) { delete this; return 0; } return m_ulObjRefCount; }
STDMETHODIMP CBSCallbackImpl::OnStartBinding(DWORD, IBinding *) { return S_OK; }
STDMETHODIMP CBSCallbackImpl::GetPriority(LONG *) { return S_OK; }
STDMETHODIMP CBSCallbackImpl::OnLowResource(DWORD) { return S_OK; }
STDMETHODIMP CBSCallbackImpl::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText) { return S_OK; }
STDMETHODIMP CBSCallbackImpl::OnStopBinding(HRESULT, LPCWSTR) { return S_OK; }
STDMETHODIMP CBSCallbackImpl::GetBindInfo(DWORD *, BINDINFO *) { return S_OK; }
STDMETHODIMP CBSCallbackImpl::OnDataAvailable(DWORD, DWORD, FORMATETC *, STGMEDIUM *) { return S_OK; }
STDMETHODIMP CBSCallbackImpl::OnObjectAvailable(REFIID, IUnknown *) { return S_OK; }

// CDlgExplorer AUI macro implementations
AUI_BEGIN_COMMAND_MAP(CDlgExplorer, CDlgBase)
AUI_ON_COMMAND("prev", OnCommand_Prev)
AUI_ON_COMMAND("next", OnCommand_Next)
AUI_ON_COMMAND("refresh", OnCommand_Refresh)
AUI_ON_COMMAND("osnavigate", OnCommand_OSNavigate)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgExplorer, CDlgBase)
AUI_END_EVENT_MAP()

// CDlgExplorer stubs
CDlgExplorer::CDlgExplorer() : m_pWPic(NULL), m_pExploreMan(NULL), m_pOSNavigator(NULL), m_bInTimer(false), m_pBtn_NavigateInOS(NULL) {}
CDlgExplorer::~CDlgExplorer() {}
bool CDlgExplorer::OnInitDialog() { return CDlgBase::OnInitDialog(); }
void CDlgExplorer::OnShowDialog() {}
bool CDlgExplorer::Release() { return CDlgBase::Release(); }
void CDlgExplorer::OnHideDialog() {}
void CDlgExplorer::OnTick() {}
bool CDlgExplorer::MessageTranslate(AString Control, UINT msg, WPARAM wParam, LPARAM lParam) { return false; }
BOOL CDlgExplorer::PeekMessage_Filter(LPMSG lpMsg,HWND hWnd) { return FALSE; }
BOOL CDlgExplorer::AddDownloadUrl(WCHAR *FileUrl) { return FALSE; }
BOOL CDlgExplorer::InitTaskList() { return FALSE; }
void CDlgExplorer::NavigateUrl(const WCHAR *URL, CECURLOSNavigator* pOSNavigator) {}
void CDlgExplorer::NavigateUrl(const CHAR *Url, CECURLOSNavigator* pOSNavigator) {}
void CDlgExplorer::OnWinTimer() {}
BOOL CDlgExplorer::FreeExplorerMan() { return FALSE; }
void CDlgExplorer::GetCurrentUrl() {}
void CDlgExplorer::GoForward() {}
void CDlgExplorer::GoBackward() {}
void CDlgExplorer::Refresh() {}
bool CDlgExplorer::GetDownloadState(abase::vector<AWString>& vecTaskDone, abase::vector<AWString>& vecTaskToDo, AWString& curDownFile, int &curDownProgress) { return false; }
double CDlgExplorer::GetDownloadRate() { return 0.0; }
bool CDlgExplorer::IsDownloading() { return false; }
void CDlgExplorer::CancelCurDownLoad() {}
void CDlgExplorer::KeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam) {}
void CDlgExplorer::KeyEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam) {}
void CDlgExplorer::MouseEvent(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam) {}
void CDlgExplorer::MouseEventX(HWND hwnd, UINT uMsg,WPARAM wParam,LPARAM lParam) {}
void CDlgExplorer::MouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam) {}
HWND CDlgExplorer::GetCaretWindowPos(LPRECT lprect) { return NULL; }
void CDlgExplorer::OnCommand_Prev(const char *szCommand) {}
void CDlgExplorer::OnCommand_Next(const char *szCommand) {}
void CDlgExplorer::OnCommand_Refresh(const char *szCommand) {}
void CDlgExplorer::OnCommand_OSNavigate(const char *szCommand) {}
void CDlgExplorer::ReleaseOSNavigator() {}
void CDlgExplorer::SetOSNavigator(CECURLOSNavigator *pOSNavigator) {}
void CDlgExplorer::UpdateOSNavigatorUI() {}
bool CDlgExplorer::CanDoOSNavigate() const { return false; }

// CDlgLoginOther AUI macro implementations
AUI_BEGIN_COMMAND_MAP(CDlgLoginOther, CDlgBase)
AUI_END_COMMAND_MAP()

// CDlgLoginOther stubs
CDlgLoginOther::CDlgLoginOther() : m_bLoginSwitch(false) {}
CDlgLoginOther::~CDlgLoginOther() {}
void CDlgLoginOther::DocumentComplete(IDispatch* pDisp,VARIANT* URL) {}
void CDlgLoginOther::OnShowDialog() {}
void CDlgLoginOther::OnHideDialog() {}
CECLoginUIMan* CDlgLoginOther::GetLoginUIMan() { return NULL; }
