#if !defined(AFX_SKINWINDOW_H__825CB13D_BC3F_4EED_882B_8A80DF32CC84__INCLUDED_)
#define AFX_SKINWINDOW_H__825CB13D_BC3F_4EED_882B_8A80DF32CC84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinWindow.h : header file
//
#include "NewSkin.h"
#include <vector>

class	SkinWindowList;
class	SkinItem;
struct	XMLLabel;
struct	SkinItemNode;
struct	SkinWindowNode;
class	SkinArt;
class	SkinStyle;
class	SkinOption;
class	SkinGroup;
class	XMLLabelAttribute;
class	CControlWindow; 
class	SkinBrowser;
class	CAnimWindow;
/////////////////////////////////////////////////////////////////////////////
// CSkinWindow dialog

class CSkinWindow : public CDialog
{
// Construction
public:
	CString			m_cName;
	CString			m_cText;
	SkinArt			*m_sArt;
	SkinWindowList	*m_sWindowList;
	BOOL			m_bIsMainWindow;
	XMLLabel		*m_xLabel;
	SkinItemNode	*m_sItemHead;
	SkinItem		*m_sTabItem;
	SkinBrowser		*m_Browser;

	

	CSkinWindow(SkinWindowList *sList,XMLLabel *xLabel,CWnd* pParent = NULL);   // standard constructor

	void AddItem(SkinItem *sItem);
	SkinItem* GetItem(CPoint point);
	SkinItem* GetItem(LPCTSTR lName);
	void SetFocusItem(SkinItem *sItem);
	void SetHoverItem(SkinItem *sItem);
	void ClickItem();
	void TabItem(BOOL bNext=TRUE);

	void UpdateView();
	CDC* GetMemoryDC(){return m_dcMemory;};
	Gdiplus::Graphics* GetGraphics(){return m_pGraphics;};
	CControlWindow* GetCtrlWnd(){return m_pCtrlWnd;}
	void Release();
	CRITICAL_SECTION* GetCriticalSection() { return m_pcsGDIPlus;}
// Dialog Data
	//{{AFX_DATA(CSkinWindow)
	enum { IDD = IDD_SKINWINDOW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinWindow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	HICON			m_hIcon;
	HCURSOR			m_hCursor;
	SkinItem		*m_sHoverItem;
	SkinItem		*m_sFocusItem;
	SkinItem		*m_sClickedItem;
	CDC*			m_dcMemory;
	Gdiplus::Graphics*		m_pGraphics;
	CControlWindow*			m_pCtrlWnd;
	CRITICAL_SECTION*		m_pcsGDIPlus;
	SkinWindowNode*			m_pWindowNode;

	std::vector<CAnimWindow*> m_AnimVector;

	virtual void OnCancel();

	// Generated message map functions
	//{{AFX_MSG(CSkinWindow)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	afx_msg void OnUpdateEdit();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

struct SkinWindowNode
{
	enum{
		STATE_TO_INIT,
		STATE_LIVE,
		STATE_CLOSED,
	};
	SkinWindowNode *next;
	CWinThread *wthread;
	CSkinWindow *window;
	XMLLabel* pLabel;
	int		m_iState;

	SkinWindowNode()
	{
		next = NULL;
		wthread = NULL;
		window = NULL;
		pLabel = NULL;
		m_iState = STATE_TO_INIT;
	}

	~SkinWindowNode()
	{

	}
};

struct SkinStyleNode
{
	SkinStyleNode *next;
	SkinStyle *style;

	~SkinStyleNode();
};

struct SkinOptionNode
{
	SkinOptionNode *next;
	SkinOption *option;

	~SkinOptionNode();
};

struct SkinGroupNode
{
	SkinGroupNode *next;
	SkinGroup *group;

	~SkinGroupNode();
};

class SkinWindowList
{
public:
	HINSTANCE hDLL;
	SkinWindowList();

	~SkinWindowList();
	void Release();

	CSkinWindow* AddWindow(XMLLabel *xLabel);
	void AddStyle(XMLLabel *xLabel);
	SkinStyle* GetStyle(LPCTSTR lName);
	void AddOption(XMLLabel *xLabel);
	SkinOption* GetOption(LPCTSTR lName);
	void AddGroup(XMLLabel *xLabel);
	SkinGroup* GetGroup(LPCTSTR lName);
	CString GetOptionValue(LPCTSTR lName);
	void CloseWindow(CSkinWindow* cWindow);
	void ShowWindow(LPCTSTR strName);
	CSkinWindow* GetWindow(LPCTSTR lName);
	SkinItem* GetItem(LPCTSTR lName);
	BOOL LoadDllFunction(LPCTSTR lName,LPCTSTR lParam);
	static BOOL CALLBACK Command(void *pList,LPCTSTR lCommand);
	BOOL Event(CString cCommand,SkinItem *sItem);
	BOOL OnEvent(XMLLabelAttribute *xAttirb,SkinItem *sItem);
	void HideAllWindows();
	void Exit();
	void OnXunyou();
public:
	SkinWindowNode	*sWindowHead;
	SkinStyleNode	*sStyleHead;
	SkinOptionNode	*sOptionHead;
	SkinGroupNode	*sGroupHead;
	bool			bExit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINWINDOW_H__825CB13D_BC3F_4EED_882B_8A80DF32CC84__INCLUDED_)
