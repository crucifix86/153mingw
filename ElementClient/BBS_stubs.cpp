// Stub implementations for CDlgBBS and CDlgLoginPage
// BBS system is disabled in this build - uses MSVC-specific features

#include "DlgBBS.h"
#include "DlgLoginPage.h"

// CDlgBBS implementation stubs
AUI_BEGIN_COMMAND_MAP(CDlgBBS, CDlgBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgBBS, CDlgBase)
AUI_END_EVENT_MAP()

CDlgBBS::CDlgBBS()
    : m_pGoBackButton(nullptr)
    , m_pGoAheadButton(nullptr)
    , m_pRefreshButton(nullptr)
    , m_pStopButton(nullptr)
    , m_pCloseButton(nullptr)
    , m_pHomePageButton(nullptr)
    , m_pFontTipLabel(nullptr)
    , m_pFontSizeCombo(nullptr)
    , m_bUseNewFontSize(false)
    , m_bInit(false)
    , m_bNeedClose(false)
    , m_bNeedStop(false)
    , m_bFirstOpen(true)
    , m_bShowBBS(false)
{
}

CDlgBBS::~CDlgBBS() {
}

bool CDlgBBS::OnInitDialog() {
    return CDlgBase::OnInitDialog();
}

bool CDlgBBS::Release() {
    return CDlgBase::Release();
}

bool CDlgBBS::Tick() {
    return CDlgBase::Tick();
}

void CDlgBBS::OnShowDialog() {
}

void CDlgBBS::ChangeSize(int nWidth, int nHeight) {
}

void CDlgBBS::OnCommand_selectchange(const char* szCommand) {}
void CDlgBBS::OnCommand_goback(const char* szCommand) {}
void CDlgBBS::OnCommand_goahead(const char* szCommand) {}
void CDlgBBS::OnCommand_refresh(const char* szCommand) {}
void CDlgBBS::OnCommand_stop(const char* szCommand) {}
void CDlgBBS::OnCommand_homepage(const char* szCommand) {}
void CDlgBBS::OnCommand_close(const char* szCommand) {}
void CDlgBBS::OnCommand_fontsize(const char* szCommand) {}
void CDlgBBS::OnEvent_mousewheel(WPARAM wParam, LPARAM lParam, AUIObject* pObj) {}
void CDlgBBS::OnEvent_mousemove(WPARAM wParam, LPARAM lParam, AUIObject* pObj) {}
void CDlgBBS::OnEvent_lbuttonup(WPARAM wParam, LPARAM lParam, AUIObject* pObj) {}


// CDlgLoginPage implementation stubs
AUI_BEGIN_COMMAND_MAP(CDlgLoginPage, CDlgBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgLoginPage, CDlgBase)
AUI_END_EVENT_MAP()

CDlgLoginPage::CDlgLoginPage()
    : m_bUseNewFontSize(false)
    , m_bFirstOpen(true)
    , m_bLoaded(false)
{
}

CDlgLoginPage::~CDlgLoginPage() {
}

void CDlgLoginPage::ChangeSize(int nWidth, int nHeight) {
}

void CDlgLoginPage::OnCommand_Cancel(const char* szCommand) {
}

void CDlgLoginPage::OnEvent_mousewheel(WPARAM wParam, LPARAM lParam, AUIObject* pObj) {}
void CDlgLoginPage::OnEvent_mousemove(WPARAM wParam, LPARAM lParam, AUIObject* pObj) {}
void CDlgLoginPage::OnEvent_lbuttonup(WPARAM wParam, LPARAM lParam, AUIObject* pObj) {}

void CDlgLoginPage::OnShowDialog() {}
bool CDlgLoginPage::OnInitDialog() { return CDlgBase::OnInitDialog(); }
bool CDlgLoginPage::Release() { return CDlgBase::Release(); }
void CDlgLoginPage::OnTick() {}
void CDlgLoginPage::Resize(A3DRECT rcOld, A3DRECT rcNew) {}
void CDlgLoginPage::OnChangeLayoutEnd(bool bAllDone) {}
void CDlgLoginPage::SetBBSPagePos() {}
