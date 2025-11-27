// AnimWindow.cpp : implementation file
//

#include "stdafx.h"
#include "NewSkin.h"
#include "AnimWindow.h"
#include "SkinWindow.h"

#include "SkinItem.h"
// CAnimWindow dialog
typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);  
extern MYFUNC MyUpdateLayeredWindow;

IMPLEMENT_DYNAMIC(CAnimWindow, CDialog)

CAnimWindow::CAnimWindow(SkinAnimItem* pItem,CWnd* pParent /*=NULL*/)
	: CDialog(CAnimWindow::IDD, pParent),
	m_clrTransparent(RGB(0,255,0))
{
	m_pAnimItem = pItem;
}

CAnimWindow::~CAnimWindow()
{
}

void CAnimWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAnimWindow, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CAnimWindow message handlers

void CAnimWindow::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

// 	if (m_pAnimItem)
// 	{
// 		Graphics graphics(dc);
// 		m_pAnimItem->Draw(&graphics);
// 	}
}

void CAnimWindow::ShowDialog()
{
	Create(m_lpszTemplateName, m_pParentWnd);
	ShowWindow(SW_HIDE);
	//UpdateView();
}


BOOL CAnimWindow::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
//	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);
	
//	::SetLayeredWindowAttributes(GetSafeHwnd(), 0,0,LWA_ALPHA);

	//设置窗口的EXSTYLE为0x80000,LayeredWindow
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CAnimWindow::UpdateView()
{
	//避免创建该窗口的线程和窗口线程同时调用GDI+的函数
	CWnd* parent = GetParent();
	CSkinWindow* pSW = (CSkinWindow*)parent;

	EnterCriticalSection(pSW->GetCriticalSection());
	CRect r = m_pAnimItem->m_cPosition;
	SIZE sizeWindow = { r.right-r.left, r.bottom-r.top};

	HDC hDC = ::GetDC(m_hWnd);
	//先画到memoryDC上，使用这个memoryDC 更新hDC
	HDC hdcMemory = CreateCompatibleDC(hDC);
	CDC*			m_dcMemory;
	m_dcMemory = CDC::FromHandle(hdcMemory);

	BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
	int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
	stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
	stBmpInfoHeader.biWidth = sizeWindow.cx;   
	stBmpInfoHeader.biHeight = sizeWindow.cy;   
	stBmpInfoHeader.biPlanes = 1;   
	stBmpInfoHeader.biBitCount = 32;   
	stBmpInfoHeader.biCompression = BI_RGB;   
	stBmpInfoHeader.biClrUsed = 0;   
	stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

	PVOID pvBits = NULL;   
	HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);

	if(hbmpMem)   
	{
		memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);

		HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);

		Graphics* m_pGraphics = Gdiplus::Graphics::FromHDC(hdcMemory);

		m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);

		m_pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

		m_pAnimItem->Draw(m_pGraphics);

		POINT ptSrc = { 0, 0};

		BLENDFUNCTION b;
		b.AlphaFormat = 1;
		b.BlendFlags = 0;
		b.BlendOp = 0;
		b.SourceConstantAlpha = 255;//AC_SRC_ALPHA
		MyUpdateLayeredWindow(m_hWnd, hDC, NULL, &sizeWindow, hdcMemory, &ptSrc, 0, &b, 2);

		::SelectObject( hdcMemory, hbmpOld);   
		::DeleteObject(hbmpMem); 
		Gdiplus::DllExports::GdipFree(m_pGraphics);
		m_pGraphics = NULL;
	}

	::DeleteDC(hdcMemory);
	::ReleaseDC(m_hWnd,hDC);
	m_dcMemory = NULL;
	LeaveCriticalSection(pSW->GetCriticalSection());
}
void CAnimWindow::Tick()
{
	if(m_pAnimItem) m_pAnimItem->Tick();
}