
#include "stdafx.h"
#include "ControlWindow.h"

extern HMODULE hFuncInst;
// CControlWindow dialog

CControlWindow::CControlWindow(CWnd* pParentWnd/* = NULL*/)
	: CDialog(IDD, pParentWnd)
	, m_clrTransparent(RGB(0,255,0))
	, m_pOleWnd(NULL)
{
}

CControlWindow::~CControlWindow()
{
}

void CControlWindow::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(CControlWindow, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CControlWindow message handlers

void CControlWindow::OnOK()
{

	CDialog::OnOK();
}

void CControlWindow::OnCancel()
{
	if (m_pOleWnd)
	{
		m_pOleWnd->DestroyWindow();
	}

	DestroyWindow();
}

void CControlWindow::EndDialog(int nResult)
{
	CDialog::EndDialog(nResult);
}

HBRUSH CControlWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
//	if (nCtlColor == CTLCOLOR_DLG)
//		return m_brush;

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CControlWindow::OnInitDialog()
{
	CDialog::OnInitDialog();
/*	
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	VERIFY((dwStyle & WS_POPUP) != 0);
	VERIFY((dwStyle & WS_BORDER) == 0);
	VERIFY((dwStyle & WS_SIZEBOX) == 0);
	VERIFY((dwStyle & WS_DLGFRAME) == 0);

	m_brush.CreateSolidBrush(m_clrTransparent);
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	VERIFY((dwExStyle & WS_EX_APPWINDOW) == 0);
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	typedef BOOL (WINAPI *MYFUNC)(HWND, COLORREF, BYTE, DWORD); 
	MYFUNC SetLayeredWindowAttributes = NULL; 
	SetLayeredWindowAttributes = (MYFUNC)::GetProcAddress(hFuncInst, "SetLayeredWindowAttributes"); 
	SetLayeredWindowAttributes(GetSafeHwnd(), m_clrTransparent, 0, 1); 
*/
	return TRUE;
}

void CControlWindow::ShowDialog()
{
	Create(m_lpszTemplateName, m_pParentWnd);
	ShowWindow(SW_HIDE);
}
