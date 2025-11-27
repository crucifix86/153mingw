// LauncherDlg.h : header file
//

#if !defined(AFX_LAUNCHERDLG_H__781A22F9_2C6A_4F64_9E38_3D40B1F78291__INCLUDED_)
#define AFX_LAUNCHERDLG_H__781A22F9_2C6A_4F64_9E38_3D40B1F78291__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLauncherDlg dialog
using namespace Gdiplus;

class CLauncherDlg : public CDialog
{
// Construction
public:
	CLauncherDlg(BOOL bFullCheck,BOOL bUpdate,CWnd* pParent = NULL);	// standard constructor
	virtual ~CLauncherDlg();
	bool ImportStringTable(const TCHAR *pszFilename);
	
	void DrawFileProgress();
	void SetFileProgress(int i);
	void DrawTotalProgress();
	void SetTotalProgress(int i);
	void DrawFileText();
	void SetFileText(LPCTSTR c);
	void DrawText();
	void SetText(LPCTSTR c);
	void UpdateView();
	Bitmap* m_bmp;
	Bitmap* m_Filled1;
	Bitmap* m_Filled2;
	Bitmap* m_Unfilled;
	Font*	m_Font;
	Font*	m_FileFont;
	CRITICAL_SECTION m_csGDIPlus;
	Gdiplus::Graphics*		m_pGraphics;
	
	int m_iFilePos;
	int m_iTotalPos;

	CString m_cText;
	CString m_cFileText;

	BOOL m_bFullCheck;
	BOOL m_bUpdate;
	
// Dialog Data
	//{{AFX_DATA(CLauncherDlg)
	enum { IDD = IDD_LAUNCHER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLauncherDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLauncherDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern BOOL bSilentUpdate;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAUNCHERDLG_H__781A22F9_2C6A_4F64_9E38_3D40B1F78291__INCLUDED_)
