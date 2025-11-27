// Launcher.h : main header file for the LAUNCHER application
//

#if !defined(AFX_LAUNCHER_H__E5AF0C79_5185_406C_B43D_5CEC3E2E6143__INCLUDED_)
#define AFX_LAUNCHER_H__E5AF0C79_5185_406C_B43D_5CEC3E2E6143__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLauncherApp:
// See Launcher.cpp for the implementation of this class
//

class CLauncherApp : public CWinApp
{
public:
	int m_nParmSum;
	CString m_cParm[10];
	CLauncherApp();
	void CalcCommandParm();
	BOOL GetNotifyWindowHandle();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLauncherApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLauncherApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern DWORD dwNotifyWindow;
extern WORD wNotifyEventID;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAUNCHER_H__E5AF0C79_5185_406C_B43D_5CEC3E2E6143__INCLUDED_)
