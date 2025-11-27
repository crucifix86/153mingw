// NewSkinDlg.h : header file
//
#include "SkinWindow.h"

#if !defined(AFX_NEWSKINDLG_H__CC19E6A0_B23C_4E76_9139_4F31B97FE85A__INCLUDED_)
#define AFX_NEWSKINDLG_H__CC19E6A0_B23C_4E76_9139_4F31B97FE85A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNewSkinDlg dialog

class CNewSkinDlg : public CDialog
{
// Construction
public:
	CNewSkinDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNewSkinDlg)
	enum { IDD = IDD_NEWSKIN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewSkinDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CSkinWindow *d1,*d2;

	// Generated message map functions
	//{{AFX_MSG(CNewSkinDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSKINDLG_H__CC19E6A0_B23C_4E76_9139_4F31B97FE85A__INCLUDED_)
