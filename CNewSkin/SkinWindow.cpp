// SkinWindow.cpp : implementation file
//

#include "stdafx.h"
#include "SkinWindow.h"
#include "SkinItem.h"
#include "XMLFile.h"
#include "Update.h"
#include <Mmsystem.h>
#include "ControlWindow.h"
#include "AnimWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CNewSkinApp theApp;
typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
extern MYFUNC MyUpdateLayeredWindow;
extern volatile float fFileProgress;
extern int LastProgress;
extern bool IsArcNorthAmerica;
/////////////////////////////////////////////////////////////////////////////
// CSkinWindow dialog


CSkinWindow::CSkinWindow(SkinWindowList *sList,XMLLabel *xLabel,CWnd* pParent /*=NULL*/)
	: CDialog(CSkinWindow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSkinWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sWindowList=sList;
	m_xLabel=xLabel;
	m_bIsMainWindow=FALSE;
	m_sItemHead=new SkinItemNode;
	m_sItemHead->item=NULL;
	m_sItemHead->next=NULL;
	m_sHoverItem=NULL;
	m_sFocusItem=NULL;
	m_sTabItem=NULL;
	m_sClickedItem=NULL;
	m_dcMemory = NULL;
	m_pGraphics = NULL;
	m_sArt = NULL;
	m_cName=m_xLabel->Attrib->GetAttribute(TEXT("Name"));
	m_cText=m_xLabel->Attrib->GetAttribute(TEXT("Text"));
	m_pCtrlWnd = NULL;
	m_pWindowNode = NULL;
	m_Browser = NULL;
	m_pcsGDIPlus = new CRITICAL_SECTION;
	InitializeCriticalSection(m_pcsGDIPlus);
	if((CString)m_xLabel->Attrib->GetAttribute(TEXT("MainWindow"))==TEXT("true"))
		m_bIsMainWindow=TRUE;
}

void CSkinWindow::Release()
{
	if (m_sArt)
	{
		delete m_sArt;
		m_sArt = NULL;
	}

	if (m_sItemHead)
	{
		SkinItemNode* pNode = m_sItemHead;
		while(pNode != NULL)
		{
			SkinItemNode* pDel = pNode;
			pNode = pNode->next;

			//if (pDel->item && pDel->item->m_iType != SK_BROWSER)
			if (pDel->item)
				delete pDel->item;

			delete pDel;
		}
		
		m_sItemHead = NULL;
	}

	if (m_pCtrlWnd)
	{
//		m_pCtrlWnd->DestroyWindow();
//		delete m_pCtrlWnd;
//		m_pCtrlWnd = NULL;
	}
	for (int i=0;i<m_AnimVector.size();i++)
	{
		CAnimWindow* p = m_AnimVector[i];
		p->DestroyWindow();
		delete p;
	}

	DeleteCriticalSection(m_pcsGDIPlus);
	delete m_pcsGDIPlus;
	
}

void CSkinWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkinWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSkinWindow, CDialog)
	//{{AFX_MSG_MAP(CSkinWindow)
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_EN_UPDATE(IDC_EDIT, OnUpdateEdit)
	ON_WM_SETCURSOR()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinWindow message handlers

BOOL CSkinWindow::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	

	m_sArt=new SkinArt(m_xLabel);
	m_sArt->LoadArt();
	XMLLabel *chLabel=m_xLabel->FirstChildLabel;
	while(chLabel!=NULL)
	{
		if(chLabel->LabelType==TEXT("SkinButton"))
		{
			SkinButton *s=new SkinButton(this,chLabel);
			AddItem(s);
		}
		else if(chLabel->LabelType==TEXT("SkinBrowser"))
		{
			m_pCtrlWnd = new CControlWindow(this);
			if (m_pCtrlWnd != NULL)
			{
				m_pCtrlWnd->ShowDialog();
			}
			SkinBrowser *s=new SkinBrowser(this,chLabel);
			AddItem(s);
			m_Browser = s;

		}
		else if(chLabel->LabelType==TEXT("SkinTextField"))
		{
			SkinTextField *s=new SkinTextField(this,chLabel);
			AddItem(s);
		}
		else if(chLabel->LabelType==TEXT("SkinPasswordField"))
		{
			SkinPasswordField *s=new SkinPasswordField(this,chLabel);
			AddItem(s);
		}
		else if(chLabel->LabelType==TEXT("SkinProgressBar"))
		{
			SkinProgressBar *s=new SkinProgressBar(this,chLabel);
			AddItem(s);
		}
		else if(chLabel->LabelType==TEXT("SkinComboBox"))
		{
			SkinComboBox *s=new SkinComboBox(this,chLabel);
			AddItem(s);
		}
		else if(chLabel->LabelType==TEXT("SkinListBox"))
		{
			SkinListBox *s=new SkinListBox(this,chLabel);
			AddItem(s);
		}
		else if(chLabel->LabelType==TEXT("SkinScrollBar"))
		{
			SkinScrollBar *s=new SkinScrollBar(this,chLabel);
			AddItem(s);
		}
		else if(chLabel->LabelType==TEXT("SkinScrollArrow"))
		{
			SkinScrollArrow *s=new SkinScrollArrow(this,chLabel);
			AddItem(s);
		}
		else if(chLabel->LabelType==TEXT("SkinSlider"))
		{
			SkinSlider*s=new SkinSlider(this,chLabel);
			AddItem(s);
		}
		else if (chLabel->LabelType==TEXT("SkinAnimItem"))
		{
			SkinAnimItem* s = new SkinAnimItem(this,chLabel);
			AddItem(s);

			CAnimWindow* pCtrlWnd = new CAnimWindow(s,this);
			if (pCtrlWnd != NULL)
			{
				m_AnimVector.push_back(pCtrlWnd);
				//::SetWindowPos(pCtrlWnd->GetSafeHwnd(), NULL,  x + s->m_cPosition.left, y + s->m_cPosition.top, s->m_cPosition.right - s->m_cPosition.left,  s->m_cPosition.bottom - s->m_cPosition.top,0);
				pCtrlWnd->ShowDialog();
			}			
		}
		chLabel=chLabel->NextLabel;
	}
	
	CString cStr;
	
	//设置图标和鼠标指针
	cStr=m_xLabel->Attrib->GetAttribute(TEXT("Icon"));
	if(cStr!="")
		m_hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance,cStr,IMAGE_ICON,0,0,LR_LOADFROMFILE);
	else
		m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);

	cStr=m_xLabel->Attrib->GetAttribute(TEXT("Cursor"));
	if(cStr!="")
		m_hCursor = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance,cStr,IMAGE_CURSOR,0,0,LR_LOADFROMFILE);
	else
		m_hCursor = NULL;

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	
	ModifyStyle(WS_CAPTION, 0, SWP_FRAMECHANGED);
	
	XMLLabel *xLabel;
	xLabel=m_xLabel->FirstChildLabel;
	while(xLabel!=NULL)
	{
		if(xLabel->LabelType=="OnWindowLoad")
			m_sWindowList->LoadDllFunction(xLabel->Attrib->GetAttribute(TEXT("Name")),TEXT(""));
		xLabel=xLabel->NextLabel;
	}

	//设置窗口的EXSTYLE为0x80000,LayeredWindow
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	//设置对齐方式
	CWnd *c=GetDesktopWindow();
	CRect cScreenRect;
	c->GetWindowRect(&cScreenRect);
	int x,y;
	cStr=m_xLabel->Attrib->GetAttribute(TEXT("Align"));
	if(cStr==TEXT("left"))	
		x=1;
	else if(cStr==TEXT("right"))	
		x=cScreenRect.Width()-m_sArt->pBG->GetWidth();
	else 
		x=(cScreenRect.Width()-m_sArt->pBG->GetWidth())/2;
	cStr=m_xLabel->Attrib->GetAttribute(TEXT("VAlign"));
	if(cStr==TEXT("top"))	
		y=1;
	else if(cStr==TEXT("bottom"))	
		y=cScreenRect.Height()-m_sArt->pBG->GetHeight();
	else 
		y=(cScreenRect.Height()-m_sArt->pBG->GetHeight())/2;
	SetWindowPos(this,x,y,m_sArt->pBG->GetWidth(),m_sArt->pBG->GetHeight(),0);

	if (m_pCtrlWnd)
	{
		::SetWindowPos(m_pCtrlWnd->GetSafeHwnd(), NULL,  x + m_Browser->m_cPosition.left, y + m_Browser->m_cPosition.top, m_Browser->m_cPosition.right - m_Browser->m_cPosition.left,  m_Browser->m_cPosition.bottom - m_Browser->m_cPosition.top,0);
	}
	for (int i=0;i<m_AnimVector.size();i++)
	{
		CAnimWindow* p = m_AnimVector[i];
		CRect rect = p->GetSkinItem()->m_cPosition;
		::SetWindowPos(p->GetSafeHwnd(),NULL,x+rect.left,y+rect.top,rect.right-rect.left,rect.bottom-rect.top,0);
	}

	//设置窗口文字
	SetWindowText(m_cText);

	SkinItemNode *sNode=m_sItemHead;
	while( (sNode=sNode->next)!=NULL && sNode->item->m_iTabID==0);
	if(sNode!=NULL)
	{
		m_sTabItem=sNode->item;
		SetFocusItem(sNode->item);
	}

	SkinWindowNode *sWndNode = m_sWindowList->sWindowHead;
	while((sWndNode = sWndNode->next) != NULL)
		if(sWndNode->window == this)
		{
			sWndNode->m_iState = SkinWindowNode::STATE_LIVE;
			m_pWindowNode = sWndNode;
		}

	int itimer = SetTimer(1,30,NULL);
	if (m_bIsMainWindow)
	{
		SetTimer(2,200,NULL);
		SetTimer(3,120,NULL);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSkinWindow::PostNcDestroy()
{
//	delete this;
}

void CSkinWindow::UpdateView()
{
	//避免创建该窗口的线程和窗口线程同时调用GDI+的函数
	EnterCriticalSection(m_pcsGDIPlus);
	SIZE sizeWindow = { m_sArt->pBG->GetWidth(), m_sArt->pBG->GetHeight()};
	
	HDC hDC = ::GetDC(m_hWnd);
	//先画到memoryDC上，使用这个memoryDC 更新hDC
	HDC hdcMemory = CreateCompatibleDC(hDC);
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
		
		m_pGraphics = Gdiplus::Graphics::FromHDC(hdcMemory);
		
		m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);

		// 背景
		m_pGraphics->DrawImage(m_sArt->pBG, 0, 0, sizeWindow.cx, sizeWindow.cy);

		m_pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
		
		SkinItemNode *sNode;

		// 按钮
		sNode=m_sItemHead;
		while((sNode=sNode->next)!=NULL)
		{
			
			if (sNode->item->m_iType != SK_LISTBOX && sNode->item->m_iType != SK_SCROLLBAR && sNode->item->m_iType != SK_SCROLLARROW && sNode->item->m_iType != SK_TEXTFIELD) 
			{
				sNode->item->Draw();
			}
		}
		// listbox
		sNode=m_sItemHead;
		while((sNode=sNode->next)!=NULL)
		{
			if (sNode->item->m_iType == SK_LISTBOX || sNode->item->m_iType == SK_SCROLLARROW || sNode->item->m_iType == SK_SCROLLBAR || sNode->item->m_iType == SK_TEXTFIELD)
			{
				sNode->item->Draw();
			}

		}

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
	LeaveCriticalSection(m_pcsGDIPlus);
}


LRESULT CSkinWindow::OnNcHitTest(CPoint point) 
{
	ScreenToClient(&point);
	SkinItem *sItem=GetItem(point);
	if(m_sClickedItem!=NULL&&sItem!=m_sClickedItem)
	{
		m_sClickedItem->m_bIsClilcked=FALSE;
		UpdateView();
		m_sClickedItem=FALSE;
	}
	SetHoverItem(sItem);
	if(sItem==NULL)
	{
		if((GetKeyState(VK_LBUTTON)&0x8000))
			return HTCLIENT;
		else
		{
			if((GetAsyncKeyState(VK_LBUTTON)&0x8000))
			{
				SkinItemNode *sNode1=m_sItemHead;
				while( (sNode1=sNode1->next) != NULL)
					if(sNode1->item->m_iType==SK_COMBOBOX&&((SkinComboBox*)sNode1->item)->m_bExpand)
						((SkinComboBox*)sNode1->item)->Close();
				SetFocus();
			}
			return HTCAPTION;
		}
	}
	else
	{
		if(sItem->m_iType==SK_PROGRESSBAR||sItem->m_iType==SK_BUTTON&&sItem->m_bPlainText)
		{
			if((GetAsyncKeyState(VK_LBUTTON)&0x8000))
			{
				SkinItemNode *sNode1=m_sItemHead;
				while( (sNode1=sNode1->next) != NULL)
					if(sNode1->item->m_iType==SK_COMBOBOX&&((SkinComboBox*)sNode1->item)->m_bExpand)
						((SkinComboBox*)sNode1->item)->Close();
			}
			return HTCAPTION;
		}
		if(sItem->m_iType==SK_LISTBOX)
			((SkinListBox*)sItem)->Hover(point.y-sItem->m_cPosition.top);
		return HTCLIENT;
	}
}

BOOL CSkinWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CSkinWindow::OnDestroy() 
{
	CDialog::OnDestroy();
}

BOOL CSkinWindow::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	CPoint p(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
	SkinItem* sItem;
	if(pMsg->message==WM_KEYDOWN)
	{
		if(	pMsg->wParam==VK_TAB||pMsg->wParam==VK_RETURN||
			pMsg->wParam==VK_LEFT||pMsg->wParam==VK_UP||
			pMsg->wParam==VK_RIGHT||pMsg->wParam==VK_DOWN)
		{
			OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
 			return TRUE;
		}
		if(m_sFocusItem==NULL||m_sFocusItem->m_iType!=SK_TEXTFIELD)
			return TRUE;
	}
	if(pMsg->hwnd!=this->GetSafeHwnd()&&
		(pMsg->message==WM_LBUTTONDOWN||pMsg->message==WM_LBUTTONDBLCLK||
		pMsg->message==WM_LBUTTONUP||pMsg->message==WM_MOUSEWHEEL))
	{
		CPoint p1=p;
		::ClientToScreen(pMsg->hwnd,&p1);
		ScreenToClient(&p1);
		PostMessage(pMsg->message,pMsg->wParam,p1.x+(p1.y<<16));
	}
	SkinListBox *sListBox=NULL;
	if(pMsg->hwnd==this->GetSafeHwnd())
		switch(pMsg->message)
		{
		case WM_MOUSEWHEEL:
			if(m_sFocusItem!=NULL)
			{
				if(m_sFocusItem->m_iType==SK_SCROLLBAR)
					sListBox=((SkinScrollBar*)m_sFocusItem)->m_sParentListBox;
				if(m_sFocusItem->m_iType==SK_SCROLLARROW)
					sListBox=((SkinScrollArrow*)m_sFocusItem)->m_sParentListBox;
				if(m_sFocusItem->m_iType==SK_LISTBOX)
					sListBox=(SkinListBox*)m_sFocusItem;
				if(m_sFocusItem->m_iType==SK_COMBOBOX&&((SkinComboBox*)m_sFocusItem)->m_bExpand)
					sListBox=((SkinComboBox*)m_sFocusItem)->m_sListBox;
			}
			if(sListBox!=NULL)
			{
				int zDelta=HIWORD(pMsg->wParam);
				if((zDelta&0x8000)!=0)
					sListBox->SetFirstLine(sListBox->m_iFirstLine+1);
				else
					sListBox->SetFirstLine(sListBox->m_iFirstLine-1);
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			sItem=GetItem(p);
			SetFocusItem(sItem);
			if(m_sFocusItem!=NULL)
			{
				SkinComboBox *comboBox = NULL;
				if(sItem->m_iType==SK_COMBOBOX)
					comboBox=(SkinComboBox*)sItem;
				if(sItem->m_iType==SK_LISTBOX)
					comboBox=((SkinListBox*)sItem)->m_sParentComboBox;
				if(sItem->m_iType==SK_SCROLLBAR)
					comboBox=((SkinScrollBar*)sItem)->m_sParentListBox->m_sParentComboBox;
				if(sItem->m_iType==SK_SCROLLARROW)
					comboBox=((SkinScrollArrow*)sItem)->m_sParentListBox->m_sParentComboBox;
				SkinItemNode *sNode1=m_sItemHead;
				while( (sNode1=sNode1->next) != NULL)
					if(sNode1->item->m_iType==SK_COMBOBOX)
						if(comboBox!=sNode1->item&&((SkinComboBox*)sNode1->item)->m_bExpand)
							((SkinComboBox*)sNode1->item)->Close();
				p.x-=m_sFocusItem->m_cPosition.left;
				p.y-=m_sFocusItem->m_cPosition.top;
				switch(m_sFocusItem->m_iType)
				{
				case SK_TEXTFIELD:
					if(pMsg->message==WM_LBUTTONDOWN)
					{
						((SkinTextField*)m_sFocusItem)->LButtonDown(p);
						return TRUE;
					}
					break;
				case SK_LISTBOX:
					((SkinListBox*)m_sFocusItem)->Press(p.y);
					return TRUE;
					break;
				case SK_SCROLLBAR:
					((SkinScrollBar*)m_sFocusItem)->Press(p.y);
					return TRUE;
					break;
				case SK_SCROLLARROW:
					((SkinScrollArrow*)m_sFocusItem)->Press();
					return TRUE;
					break;
				case SK_Slider:
					((SkinSlider*)m_sFocusItem)->Press(p.x);
					return TRUE;
					break;
				case SK_BUTTON:
				case SK_COMBOBOX:
					if(m_sFocusItem->m_bIsEnable&&!m_sFocusItem->m_bPlainText)
					{
						m_sFocusItem->m_bIsClilcked=TRUE;
						m_sClickedItem=m_sFocusItem;
						UpdateView();
					}
					return TRUE;
					break;
				}
			}
			break;
		case WM_MOUSEMOVE:
			if(m_sFocusItem!=NULL)
			{
				p.x-=m_sFocusItem->m_cPosition.left;
				p.y-=m_sFocusItem->m_cPosition.top;
				switch(m_sFocusItem->m_iType)
				{
				case SK_TEXTFIELD:
					if(GetAsyncKeyState(VK_LBUTTON)&0x8000)
					{
						((SkinTextField*)m_sFocusItem)->Drag(p);
						return TRUE;
					}
					break;
				case SK_SCROLLBAR:
					if(GetAsyncKeyState(VK_LBUTTON)&0x8000)
					{
						((SkinScrollBar*)m_sFocusItem)->Drag(p.y);
						return TRUE;
					}
					break;
				case SK_Slider:
					if(GetAsyncKeyState(VK_LBUTTON)&0x8000)
					{
						((SkinSlider*)m_sFocusItem)->Drag(p.x);
						return TRUE;
					}
					else
						SetFocusItem(NULL);
					break;
				}
			}
			break;
		case WM_LBUTTONUP:
			sItem=GetItem(p);
			if(sItem!=NULL&&sItem==m_sFocusItem)
				ClickItem();
			if(	m_sClickedItem!=NULL)
			{
				m_sClickedItem->m_bIsClilcked=FALSE;
				if(m_sClickedItem->m_iTabID>0)
				{
					if(m_sTabItem!=NULL)
					{
						SkinItem *item=m_sTabItem;
						m_sTabItem=NULL;
					}
					m_sTabItem=m_sClickedItem;
				}
				UpdateView();
				m_sClickedItem=NULL;
			}
			if(m_sFocusItem!=NULL&&m_sFocusItem->m_iType==SK_Slider)
				SetFocusItem(NULL);
			break;
		case WM_KEYDOWN:
			if(pMsg->wParam==VK_TAB||pMsg->wParam==VK_RETURN)
			{
				OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
 				return TRUE;
			}
			if(m_sFocusItem==NULL||m_sFocusItem->m_iType!=SK_TEXTFIELD)
				return TRUE;
			break;
		}
	BOOL b=CDialog::PreTranslateMessage(pMsg);
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if(m_sFocusItem!=NULL)
			if(m_sFocusItem->m_iType==SK_TEXTFIELD)
				((SkinTextField*)m_sFocusItem)->TextChange();
		break;
	}
	return b;
}

BOOL CSkinWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_hCursor==NULL)	
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
	SetCursor(m_hCursor);
	return TRUE;
}


void CSkinWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_RETURN:
		if(m_sFocusItem!=NULL)
			ClickItem();
		break;
	case VK_TAB:
	case VK_RIGHT:
	case VK_DOWN:
		TabItem();
		break;
	case VK_LEFT:
	case VK_UP:
		TabItem(FALSE);
		break;
	default:
		break;
	}
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSkinWindow::OnUpdateEdit()
{
	if(m_sFocusItem!=NULL)
		if(m_sFocusItem->m_iType==SK_TEXTFIELD)
		{
			((SkinTextField*)m_sFocusItem)->TextChange();
		}
}

void CSkinWindow::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

	if (m_sWindowList->bExit)
	{
		CDialog::OnClose();
	}
}

void CSkinWindow::OnCancel()
{
	CDialog::OnCancel();
}

void CSkinWindow::AddItem(SkinItem *sItem)
{
	SkinItemNode *sItemNode=new SkinItemNode;
	sItemNode->item=sItem;
	SkinItemNode *sNode=m_sItemHead;
	while(sNode->next!=NULL&&sNode->next->item->m_iTabID<=sItem->m_iTabID)
		sNode=sNode->next;
	sItemNode->next=sNode->next;
	sNode->next=sItemNode;
}

SkinItem* CSkinWindow::GetItem(CPoint point)
{
	SkinComboBox *comboBox;
	Color cMapColor;
	Status status = m_sArt->pMap->GetPixel(point.x,point.y,&cMapColor);
	if (status != Ok)
	{
		return NULL;
	}
	SkinItemNode *sNode=m_sItemHead;
	while( (sNode=sNode->next) != NULL)
		if(sNode->item->m_iType==SK_COMBOBOX&&((SkinComboBox*)sNode->item)->m_bExpand)
		{
			comboBox=(SkinComboBox*)sNode->item;
			CRect cRect=comboBox->m_cExpandRect;
			if(point.x<cRect.right&&point.x>cRect.left&&point.y<cRect.bottom&&point.y>cRect.top)
			{
				Status status = comboBox->m_sArt->pMap->GetPixel(point.x-cRect.left,point.y-cRect.top,&cMapColor);
				if (status != Ok)
				{
					return NULL;
				}
				sNode=m_sItemHead;
				while( (sNode=sNode->next) != NULL)
					if((sNode->item->m_cMapColor).GetValue() == cMapColor.GetValue())
						if(	sNode->item->m_iType==SK_LISTBOX&&((SkinListBox*)sNode->item)->m_sParentComboBox==comboBox||
							sNode->item->m_iType==SK_SCROLLBAR&&((SkinScrollBar*)sNode->item)->m_sParentListBox->m_sParentComboBox==comboBox||
							sNode->item->m_iType==SK_SCROLLARROW&&((SkinScrollArrow*)sNode->item)->m_sParentListBox->m_sParentComboBox==comboBox)
							return sNode->item;
				return NULL;
			}
		}
	
	sNode=m_sItemHead;
	while( (sNode=sNode->next) != NULL)
		if((sNode->item->m_cMapColor).GetValue() == cMapColor.GetValue())
			return sNode->item;
	return NULL;
}

SkinItem* CSkinWindow::GetItem(LPCTSTR lName)
{
	SkinItemNode *sNode=m_sItemHead;
	while( (sNode=sNode->next) != NULL)
		if(sNode->item->m_cName==lName)
			return sNode->item;
	return NULL;
}

void CSkinWindow::TabItem(BOOL bNext)
{
	SkinItemNode *sNode;
	if(bNext)
	{
		if(m_sTabItem!=NULL)
		{
			m_sFocusItem=m_sTabItem;
			sNode=m_sItemHead;
			while( (sNode=sNode->next) != NULL && sNode->item!=m_sTabItem);
			while(sNode!=NULL&&sNode->next!=NULL)
				if(sNode->next->item->m_bIsEnable&&sNode->next->item->m_iTabID!=0)
				{
					m_sTabItem=sNode->next->item;
					SetFocusItem(sNode->next->item);
					return;
				}
				else
					sNode=sNode->next;
		}
		sNode=m_sItemHead;
		while( (sNode=sNode->next)!=NULL && sNode->item->m_iTabID==0);
		while(sNode!=NULL)
			if(sNode->item->m_bIsEnable)
			{
				m_sTabItem=sNode->item;
				SetFocusItem(sNode->item);
				return;
			}
			else
				sNode=sNode->next;
	}
	else
	{
		if(m_sTabItem!=NULL)
		{
			m_sFocusItem=m_sTabItem;
			sNode=m_sItemHead;
			SkinItem *sItem=NULL;
			while( (sNode=sNode->next) != NULL && sNode->item!=m_sTabItem)
				if(sNode->item->m_bIsEnable&&sNode->item->m_iTabID!=0)
					sItem=sNode->item;
			if(sItem!=NULL)
			{
				m_sTabItem=sItem;
				SetFocusItem(sItem);
				return;
			}
			while( (sNode=sNode->next) != NULL)
				if(sNode->item->m_bIsEnable&&sNode->item->m_iTabID!=0)
					sItem=sNode->item;
			if(sItem!=NULL)
			{
				m_sTabItem=sItem;
				SetFocusItem(sItem);
				return;
			}
		}
	}
	SetFocusItem(NULL);
}

void CSkinWindow::SetFocusItem(SkinItem *sItem)
{
	if(m_sFocusItem==sItem)
		return;
	if(m_sFocusItem!=NULL)
	{
		m_sFocusItem->m_bIsFocus=FALSE;
		if(m_sFocusItem->m_iType!=SK_COMBOBOX)
			UpdateView();	
	}
	if(sItem == NULL|| (sItem != NULL && !sItem->m_bIsEnable))
	{
		m_sFocusItem=NULL;
		return;
	}
	m_sFocusItem=sItem;
	if(sItem->m_iType!=SK_BROWSER)
		SetFocus();
	if(sItem->m_iType==SK_TEXTFIELD)
	{
		((SkinTextField*)sItem)->SetFocus();
	}
	sItem->m_bIsFocus=TRUE;
	UpdateView();
}

void CSkinWindow::SetHoverItem(SkinItem *sItem)
{
	if(sItem==m_sHoverItem)
		return;
	if(m_sHoverItem!=NULL)
	{
		m_sHoverItem->m_bIsHover=FALSE;
//		if(m_sHoverItem->m_iType==SK_LISTBOX)
//		{
//			int iOld=((SkinListBox*)m_sHoverItem)->m_iHoverLine;
//			((SkinListBox*)m_sHoverItem)->m_iHoverLine=-1;
//			((SkinListBox*)m_sHoverItem)->DrawLine(iOld-((SkinListBox*)m_sHoverItem)->m_iFirstLine);
//		}
		m_sWindowList->Event(TEXT("loseHover"),m_sHoverItem);
		m_sHoverItem->m_cParentWnd->UpdateView();
		m_sHoverItem=NULL;
	}
	if(sItem!=NULL&&sItem->m_bIsEnable&&!sItem->m_bPlainText)
		if(sItem->m_iType==SK_BUTTON||sItem->m_iType==SK_LISTBOX||sItem->m_iType==SK_COMBOBOX)
		{
			m_sHoverItem=sItem;
			sItem->m_bIsHover=TRUE;
			m_sWindowList->Event(TEXT("getHover"),m_sHoverItem);
			m_sHoverItem->m_cParentWnd->UpdateView();
		}

}

void CSkinWindow::ClickItem()
{
	if(!m_sFocusItem->m_bIsEnable)
		return;
	if(m_sFocusItem->m_bToggle)
	{
		m_sFocusItem->m_bIsSelected=!m_sFocusItem->m_bIsSelected;
		if(m_sFocusItem->m_bIsSelected)
			m_sWindowList->Event(TEXT("toggleOn"),m_sFocusItem);
		else
			m_sWindowList->Event(TEXT("toggleOff"),m_sFocusItem);

		UpdateView();
		if(m_sFocusItem->m_sGroup!=NULL)
			m_sFocusItem->m_sGroup->SetSelected(m_sFocusItem);
	}
	switch(m_sFocusItem->m_iType)
	{
	case SK_BUTTON:
		m_sWindowList->Event(TEXT("click"),m_sFocusItem);
		break;
	case SK_TEXTFIELD:
		m_sWindowList->Event(TEXT("click"),m_sFocusItem);
		break;
	case SK_PASSWORDFIELD:
		m_sWindowList->Event(TEXT("click"),m_sFocusItem);
		break;
	case SK_COMBOBOX:
		m_sWindowList->Event(TEXT("click"),m_sFocusItem);
		if(((SkinComboBox*)m_sFocusItem)->m_bExpand)
			((SkinComboBox*)m_sFocusItem)->Close();
		else
			((SkinComboBox*)m_sFocusItem)->Expand();
		break;
	}
}

UINT ThreadProc(LPVOID pParam)
{
	XMLLabel *xLabel = (XMLLabel*)pParam;

	SkinWindowNode *sNode = (theApp.m_sWindowList)->sWindowHead;
	while((sNode = sNode->next) != NULL)
		if(sNode->pLabel == xLabel)
			break;

	CSkinWindow *cWindow=new CSkinWindow(theApp.m_sWindowList,xLabel);
	sNode->window = cWindow;

	cWindow->DoModal();

	cWindow->Release();

	delete cWindow;
	cWindow = NULL;
	
	return 0;
}

//////////////////////////////////////////SkinWindowList/////////////////////////////////////////////////
SkinWindowList::SkinWindowList()
{
	sWindowHead=new SkinWindowNode;
	sWindowHead->next=NULL;
	sWindowHead->window=NULL;
	sStyleHead=new SkinStyleNode;
	sStyleHead->next=NULL;
	sStyleHead->style=NULL;
	sOptionHead=new SkinOptionNode;
	sOptionHead->next=NULL;
	sOptionHead->option=NULL;
	sGroupHead = new SkinGroupNode;
	sGroupHead->next = NULL;
	sGroupHead->group = NULL;
	bExit=false;
}

SkinWindowList::~SkinWindowList()
{
	Release();
}
void SkinWindowList::Release()
{
	if (sWindowHead)
	{
		SkinWindowNode* pNode = sWindowHead;
		while (pNode != NULL)
		{
			SkinWindowNode* pDel = pNode;
			pNode = pNode->next;
			delete pDel;
		}
		
		sWindowHead = NULL;
	}
	if (sStyleHead)
	{
		SkinStyleNode* pNode = sStyleHead;
		while (pNode != NULL)
		{
			SkinStyleNode* pDel = pNode;
			pNode = pNode->next;
			delete pDel;
		}
		
		sStyleHead = NULL;
	}
	if (sOptionHead)
	{
		SkinOptionNode* pNode = sOptionHead;
		while (pNode != NULL)
		{
			SkinOptionNode* pDel = pNode;
			pNode = pNode->next;
			delete pDel;
		}
		
		sOptionHead = NULL;
	}
	if (sGroupHead)
	{
		SkinGroupNode* pNode = sGroupHead;
		while (pNode != NULL)
		{
			SkinGroupNode* pDel = pNode;
			pNode = pNode->next;
			delete pDel;
		}
		
		sGroupHead = NULL;
	}
}

void SkinWindowList::AddStyle(XMLLabel *xLabel)
{
	SkinStyle *sStyle=new SkinStyle(xLabel);
	sStyle->LoadStyle();
	SkinStyleNode *sNode=new SkinStyleNode;
	sNode->next=sStyleHead->next;
	sNode->style=sStyle;
	sStyleHead->next=sNode;
}

SkinStyle* SkinWindowList::GetStyle(LPCTSTR lName)
{
	if(lName==NULL)
		return NULL;
	SkinStyleNode *sNode=sStyleHead;
	while((sNode=sNode->next)!=NULL)
		if(sNode->style->m_cName==lName)
			return sNode->style;
	return NULL;
}

void SkinWindowList::AddOption(XMLLabel *xLabel)
{
	SkinOption *sOption=new SkinOption(this,xLabel);
	sOption->LoadOption();
	SkinOptionNode *sNode=new SkinOptionNode;
	sNode->next=sOptionHead->next;
	sNode->option=sOption;
	sOptionHead->next=sNode;
}

SkinOption* SkinWindowList::GetOption(LPCTSTR lName)
{
	if(lName==NULL)
		return NULL;
	SkinOptionNode *sNode=sOptionHead;
	while((sNode=sNode->next)!=NULL)
		if(sNode->option->m_cName==lName)
			return sNode->option;
	return NULL;
}

CString SkinWindowList::GetOptionValue(LPCTSTR lName)
{
	if(lName==NULL)
		return "";
	CString c;
	SkinOptionNode *sNode=sOptionHead;
	while((sNode=sNode->next)!=NULL)
	{
		c=sNode->option->m_xAttrib->GetAttribute(lName);
		if(c!="")
			return c;
	}
	return "";
}

void SkinWindowList::AddGroup(XMLLabel *xLabel)
{
	SkinGroup *sGroup=new SkinGroup(this,xLabel);
	SkinGroupNode *sNode=new SkinGroupNode;
	sNode->next=sGroupHead->next;
	sNode->group=sGroup;
	sGroupHead->next=sNode;
}

SkinGroup* SkinWindowList::GetGroup(LPCTSTR lName)
{
	if(lName==NULL)
		return NULL;
	SkinGroupNode *sNode=sGroupHead;
	while((sNode=sNode->next)!=NULL)
		if(sNode->group->m_cName==lName)
			return sNode->group;
	return NULL;
}

CSkinWindow* SkinWindowList::AddWindow(XMLLabel *xLabel)
{
	SkinWindowNode *sNode=new SkinWindowNode;
	sNode->wthread=NULL;
	sNode->pLabel = xLabel;
	sNode->m_iState = SkinWindowNode::STATE_TO_INIT;
	sNode->next=sWindowHead->next;
	sWindowHead->next=sNode;
	
	if((CString)xLabel->Attrib->GetAttribute(TEXT("MainWindow"))==TEXT("true"))
	{
		ShowWindow(xLabel->Attrib->GetAttribute(TEXT("Name")));
	}

	return NULL;
}

void SkinWindowList::CloseWindow(CSkinWindow* cWindow)
{
	if(cWindow==NULL)
		return;

	if(cWindow->m_bIsMainWindow)
		Exit();
	else
	{
		if(IsWindowVisible(cWindow->m_hWnd))
			cWindow->ShowWindow(SW_HIDE);
	}
}

void SkinWindowList::ShowWindow(LPCTSTR strName)
{

	SkinWindowNode *sNode = sWindowHead;
	while((sNode = sNode->next) != NULL)
		if((CString)sNode->pLabel->Attrib->GetAttribute(TEXT("Name")) == strName)
			break;
	if (sNode == NULL)
	{
		return;
	}
	if(sNode->m_iState == SkinWindowNode::STATE_LIVE)
	{
		sNode->window->BringWindowToTop();
		sNode->window->SetForegroundWindow();
		sNode->window->ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		sNode->wthread=AfxBeginThread(ThreadProc, sNode->pLabel);
			
		while(sNode->m_iState != SkinWindowNode::STATE_LIVE)
			Sleep(10);
		sNode->window->BringWindowToTop();
		sNode->window->SetForegroundWindow();
		sNode->window->ShowWindow(SW_SHOWNORMAL);
	}
}

void SkinWindowList::Exit()
{
	LoadDllFunction(TEXT("StopThread"),TEXT(""));
	bExit=true;
}

void SkinWindowList::HideAllWindows()
{
	SkinWindowNode *sNode=sWindowHead;
	while((sNode=sNode->next)!=NULL)
		if(sNode->m_iState == SkinWindowNode::STATE_LIVE)
			sNode->window->ShowWindow(SW_HIDE);
}

CSkinWindow* SkinWindowList::GetWindow(LPCTSTR lName)
{
	if(lName==NULL)
		return NULL;
	SkinWindowNode *sNode=sWindowHead;
	while((sNode=sNode->next)!=NULL)
		if(sNode->window && sNode->window->m_cName==lName)
			return sNode->window;
	return NULL;
}

SkinItem* SkinWindowList::GetItem(LPCTSTR lName)
{
	if(lName==NULL)
		return NULL;
	SkinWindowNode *sNode=sWindowHead;
	SkinItem *sItem = NULL;
	while((sNode=sNode->next)!=NULL)
	{
		if (sNode->window)
			sItem = sNode->window->GetItem(lName);
		if(sItem!=NULL)
			return sItem;
	}
	return NULL;
}

BOOL SkinWindowList::Event(CString cCommand,SkinItem *sItem)
{
	XMLLabelAttribute *xAttrib=new XMLLabelAttribute;
	Attribute *attrib;
	attrib=new Attribute;
	attrib->cName=TEXT("Name");
	attrib->cValue=cCommand;
	xAttrib->AddAttribute(attrib);
	BOOL b=OnEvent(xAttrib,sItem);
	xAttrib->Release();
	delete xAttrib;
	return b;
}

BOOL SkinWindowList::OnEvent(XMLLabelAttribute *xAttirb,SkinItem *sItem)
{
	BOOL b;
	CString command;
	CSkinWindow *sWindow=NULL;
	SkinItem *s;
	CString c;
	command=xAttirb->GetAttribute(TEXT("Name"));
	if((c=xAttirb->GetAttribute(TEXT("Item")))!="")
		s=GetItem(c);
	else
		s=NULL;
	if(s!=NULL)
		sItem=s;
	if((c=xAttirb->GetAttribute(TEXT("Window")))!="")
	{
		if(command==TEXT("ShowWindow"))
			ShowWindow(c);		
		sWindow=GetWindow(c);
	}
	if(sWindow==NULL&&sItem!=NULL)
		sWindow=sItem->m_cParentWnd;
	//	Global event
	b=TRUE;
	if(command==TEXT("Exit"))
		Exit();
	else if(command==TEXT("HideAllWindows"))
		HideAllWindows();
	else if(command==TEXT("PlaySound"))
		PlaySound(xAttirb->GetAttribute(TEXT("FileName")),NULL,SND_FILENAME|SND_ASYNC);
	else if(command==TEXT("SaveOption"))
	{
		c=xAttirb->GetAttribute(TEXT("Option"));
		if(c==""&&sItem!=NULL)
			c=sItem->m_xLabel->Attrib->GetAttribute(TEXT("Option"));
		if(c!="")
		{
			SkinOption *o=GetOption(c);
			if(o!=NULL)
				o->SaveOption();
		}
	}
	else if(command==TEXT("LoadOption"))
	{
		c=xAttirb->GetAttribute(TEXT("Option"));
		if(c==""&&sItem!=NULL)
			c=sItem->m_xLabel->Attrib->GetAttribute(TEXT("Option"));
		if(c!="")
		{
			SkinOption *o=GetOption(c);
			if(o!=NULL)
				o->LoadOption(TRUE);
		}
	}
	else
		b=FALSE;
	//	SkinWindow event
	if(!b&&sWindow!=NULL)
	{
		b=TRUE;
		if(command==TEXT("MinimizeWindow"))
		{
			sWindow->ShowWindow(SW_MINIMIZE);
			sWindow->SetHoverItem(NULL);
		}
		else if(command==TEXT("CloseWindow"))
			CloseWindow(sWindow);
		else if(command==TEXT("MessageBox"))
		{
			ShowWindow(c);
			MessageBox(sWindow->m_hWnd,xAttirb->GetAttribute(TEXT("Text")),xAttirb->GetAttribute(TEXT("Caption")),MB_OK);
		}
		else if (command == TEXT("OpenXunyou"))
		{
			OnXunyou();
		}
		else
			b=FALSE;
	}
	//	SkinItem event
	if(!b&&sItem!=NULL)
	{
		b=TRUE;
		if(command==TEXT("LaunchExternal"))
		{
			TCHAR path[1000];
			GetCurrentDirectory(1000, path);
			_tcscat(path, TEXT("/"));
			_tcscat(path, c);
			c=path;
			int i=c.GetLength();
			while(c.GetAt(i-1)!=TEXT('/')&&c.GetAt(i-1)!=TEXT('\\'))
				i--;
			ShellExecute(sWindow->GetSafeHwnd(),TEXT("open"),c,
						xAttirb->GetAttribute(TEXT("Param")),c.Left(i),SW_SHOWNORMAL);
		}
		else if(command==TEXT("BrowserLink"))
		{
			ShellExecute(sWindow->GetSafeHwnd(),TEXT("open"),
						xAttirb->GetAttribute(TEXT("URL")),NULL,NULL,SW_SHOWNORMAL);
		}
		else if(command==TEXT("SetProgressPos"))
			((SkinProgressBar*)sItem)->SetPos(String2Int(xAttirb->GetAttribute(TEXT("Pos"))));
		else if(command==TEXT("SetFocus"))
		{
			sWindow->m_sTabItem=sItem;
			sWindow->SetFocusItem(sItem);
		}
		else if(command==TEXT("SetURL"))
		{
			if(sItem->m_iType==SK_BROWSER)
			{
				CString stype = xAttirb->GetAttribute(TEXT("ServerType"));
				if (stype == TEXT("0")) // 内服
				{
					((SkinBrowser*)sItem)->m_cURL=xAttirb->GetAttribute(TEXT("URL"));
				}

				CNewSkinApp* pApp = (CNewSkinApp*)AfxGetApp();
				if (!IsArcNorthAmerica || (IsArcNorthAmerica && pApp->StartByArcNorthAmerica()))
					((SkinBrowser*)sItem)->BrowseURL();
			}
		}
		else if(command==TEXT("SetText"))
		{
			if (sItem == NULL)
			{
				return FALSE;
			}
			sItem->m_xLabel->Attrib->SetAttribute(TEXT("Text"),xAttirb->GetAttribute(TEXT("Text")));
			if(sItem->m_iType==SK_BUTTON)
				((SkinButton*)sItem)->m_cButtonText=xAttirb->GetAttribute(TEXT("Text"));

			sWindow->UpdateView();
		}
		else if(command==TEXT("SetEnable"))
		{
			c=xAttirb->GetAttribute(TEXT("Enable"));
			if(c==TEXT("true"))
				sItem->m_bIsEnable=TRUE;
			else if(c==TEXT("false"))
				sItem->m_bIsEnable=FALSE;

			sWindow->UpdateView();
		}
		else if(command==TEXT("SetSelected"))
		{
			c=xAttirb->GetAttribute(TEXT("Selected"));
			if(c==TEXT("true"))
				sItem->m_bIsSelected=TRUE;
			else if(c==TEXT("false"))
				sItem->m_bIsSelected=FALSE;
			else
				sItem->m_bIsSelected=!sItem->m_bIsSelected;

			sWindow->UpdateView();
		}
		else if(command==TEXT("LoadListFromFile"))
		{
			c=xAttirb->GetAttribute(TEXT("FileName"));
			if(sItem->m_iType==SK_LISTBOX&&c!="")
			{
				((SkinListBox*)sItem)->LoadListFromFile(c);
				c=xAttirb->GetAttribute(TEXT("Line"));
				if(c!="")
					((SkinListBox*)sItem)->SetLine(String2Int(c));
				else
					((SkinListBox*)sItem)->SetLine(-1);

				sWindow->UpdateView();
			}
		}
		else if(command==TEXT("SetListLine"))
		{
			if(sItem->m_iType==SK_LISTBOX)
			{
				c=xAttirb->GetAttribute(TEXT("Line"));
				if(c!="")
					((SkinListBox*)sItem)->SetLine(String2Int(c));
				else
					((SkinListBox*)sItem)->SetLine(-1);

				sWindow->UpdateView();
			}
		}
		else if(command==TEXT("SetPos"))
		{
			if(sItem->m_iType==SK_Slider)
			{
				c=xAttirb->GetAttribute(TEXT("Pos"));
				if(c!="")
					((SkinSlider*)sItem)->SetPos(String2Int(c));
			}
		}
		else
			b=FALSE;
	}
	CString cParam="";
	if(sItem!=NULL||sWindow!=NULL)
	{
		XMLLabel *chLabel;
		if(sItem!=NULL)
			chLabel=sItem->m_xLabel->FirstChildLabel;
		else
			chLabel=sWindow->m_xLabel->FirstChildLabel;
		while(chLabel!=NULL)
		{
			if(chLabel->LabelType==TEXT("Command"))
			{
				CString lEvent=chLabel->Attrib->GetAttribute(TEXT("Event"));
				if(lEvent==command)
				{
					b|=OnEvent(chLabel->Attrib,sItem);
				}
			}
			chLabel=chLabel->NextLabel;
		}
		if(sItem!=NULL)
		{
			TCHAR st[100];
			switch(sItem->m_iType)
			{
			case SK_BUTTON:
				_stprintf(st,TEXT("Text=%s,"),((SkinButton*)sItem)->m_cButtonText);
				cParam=st;
				if(sItem->m_bIsSelected)
					cParam+=TEXT("Selected=true");
				else
					cParam+=TEXT("Selected=false");
				break;
			case SK_LISTBOX:
				_stprintf(st,TEXT("Text=%s"),((SkinListBox*)sItem)->m_sParentComboBox->m_cButtonText);
				cParam=st;
				break;
			}
		}
	}
	
	b|=LoadDllFunction(command,cParam);
	return b;
}

BOOL CALLBACK SkinWindowList::Command(void *pList,LPCTSTR lCommand)
{
	CString cName=lCommand;
	XMLLabelAttribute *xAttrib=String2Attirb(cName);
	((SkinWindowList*)pList)->OnEvent(xAttrib,NULL);
	
	delete xAttrib;

	return TRUE;
}

typedef BOOL (CALLBACK *DllFunc)(FuncType,LPCTSTR,LPCTSTR,void *);

BOOL SkinWindowList::LoadDllFunction(LPCTSTR lName,LPCTSTR lParam)
{
	CString c=lName;
//	if(c==TEXT("Update")||c==TEXT("FullCheck")||c==TEXT("ClickYes")
//		||c==TEXT("ClickNo")||c==TEXT("StopThread"))
	UpdateFunc((FuncType)Command,lName,lParam,this);
//	DllFunc u=(DllFunc)GetProcAddress(hDLL,"DllFunc");
//	if(u)
//	{
//		BOOL b=u((FuncType)Command,lName,lParam,this);
//		return TRUE;
//	}

#ifdef _DEBUG
//	CString c;
//	c=TEXT("Can't find dll function name=");
//	c+=lName;
//	MessageBox(NULL,c,TEXT("XML error"),MB_OK);
#endif

	return FALSE;
}

SkinStyleNode::~SkinStyleNode()
{
	if (style)
	{
		delete style;
		style = NULL;
	}
}
SkinOptionNode::~SkinOptionNode()
{
	if (option)
	{
		delete option;
		option = NULL;
	}
}
SkinGroupNode::~SkinGroupNode()
{
	if (group)
	{
		delete group;
		group = NULL;
	}
}


void CSkinWindow::OnMove(int x, int y) 
{
	if (m_pWindowNode == 0)
	{
		return;
	}
	else if (m_pWindowNode->m_iState != SkinWindowNode::STATE_LIVE)
	{
		return;
	}

	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	if (m_pCtrlWnd != NULL)
	{
		::SetWindowPos(m_pCtrlWnd->GetSafeHwnd(), NULL, x + m_Browser->m_cPosition.left, y + m_Browser->m_cPosition.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		if (!m_pCtrlWnd->IsWindowVisible())
		{
			m_pCtrlWnd->ShowWindow(SW_NORMAL);
		}
	}
	for (int i=0;i<m_AnimVector.size();i++)
	{
		CAnimWindow* p = m_AnimVector[i];
		CRect r = p->GetSkinItem()->m_cPosition;
		::SetWindowPos(p->GetSafeHwnd(),NULL,x+r.left,y+r.top,0,0,SWP_NOSIZE|SWP_NOZORDER);
		if (!p->IsWindowVisible())
		{
			p->ShowWindow(SW_NORMAL);
		}
	}
	
}

void CSkinWindow::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_pCtrlWnd != NULL)
	{
		if (nType == SIZE_MINIMIZED)
		{
			m_pCtrlWnd->ShowWindow(SW_HIDE);
		}
		else
		{
			m_pCtrlWnd->ShowWindow(SW_NORMAL);
		}
	}
	for (int i=0;i<m_AnimVector.size();i++)
	{
		CAnimWindow* p = m_AnimVector[i];
		CRect r = p->GetSkinItem()->m_cPosition;
		
		if (nType == SIZE_MINIMIZED)
		{
			p->ShowWindow(SW_HIDE);
		}
		else
		{
			p->ShowWindow(SW_NORMAL);
		}
	}
}
TCHAR st[200];
void CSkinWindow::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (1 == nIDEvent && m_pWindowNode && m_pWindowNode->m_iState == SkinWindowNode::STATE_CLOSED)
	{
		KillTimer(1);
		if (m_pCtrlWnd)
		{
			::SendMessage(m_pCtrlWnd->m_hWnd,WM_COMMAND,IDCANCEL,0);
		}
		for (int i=0;i<m_AnimVector.size();i++)
		{
			CAnimWindow* p = m_AnimVector[i];
			::SendMessage(p->GetSafeHwnd(),WM_COMMAND,IDCANCEL,0);
		}
		
	//	SendMessage(WM_COMMAND,IDCANCEL,0);
		CDialog::OnOK();
	}
	else if (2 == nIDEvent && m_bIsMainWindow)
	{
		int progress = fFileProgress * 100;
		if (progress != LastProgress)
		{
			_stprintf(st,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=%d"),progress);
			m_sWindowList->Command(m_sWindowList,st);
			LastProgress = progress;
		}
	}
	else if (3 == nIDEvent && m_bIsMainWindow)
	{
		for (int i=0;i<m_AnimVector.size();i++)
		{
			CAnimWindow* p = m_AnimVector[i];
			p->Tick();
			p->UpdateView();
		}
	}
}

void CSkinWindow::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
}

void SkinWindowList::OnXunyou()
{
	HMODULE hHandle = ::LoadLibrary(TEXT("xunyoucall.dll"));
	if (hHandle) {
			typedef int (__stdcall * DLL_XUNYOUSTART)();
			DLL_XUNYOUSTART h_dll_xunyoustart = (DLL_XUNYOUSTART)::GetProcAddress(hHandle,"xunyou_start");
			if (h_dll_xunyoustart) {
				int ret = h_dll_xunyoustart();
				CString strText;
				strText.Format(TEXT("xunyou_start 返回值：%d"), ret);
				WriteElementLog(strText);
				WriteElementLog(NULL);
				if (ret != 0) {
					ShellExecute(NULL, TEXT("open"), TEXT("http://www.xunyou.com/w2i/"), NULL, NULL, SW_SHOWNORMAL);
				}
			} else {
				WriteElementLog(TEXT("加载xunyou_start()函数失败"));
				WriteElementLog(NULL);
			}
		FreeLibrary(hHandle);
	} else {
		WriteElementLog(TEXT("加载xunyoucall.dll失败"));
		WriteElementLog(NULL);
	}
}
