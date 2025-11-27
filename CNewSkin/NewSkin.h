// NewSkin.h : main header file for the NEWSKIN application
//

#if !defined(AFX_NEWSKIN_H__7D0D1743_5F93_4358_8BE2_A65C320CB406__INCLUDED_)
#define AFX_NEWSKIN_H__7D0D1743_5F93_4358_8BE2_A65C320CB406__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "CC_Datatype.h"
#include "CC_Error.h"
using namespace CC_SDK;
class SkinWindowList;
class XMLFile;

/////////////////////////////////////////////////////////////////////////////
// CNewSkinApp:
// See NewSkin.cpp for the implementation of this class
//

class CNewSkinApp : public CWinApp
{
public:
	SkinWindowList *m_sWindowList;
	XMLFile *m_xFile;
	int m_nParmSum;
	CString m_cParm[10];

// 北美arc
	BOOL TryStartArcNorthAmerica();
	BOOL StartByArcNorthAmerica();
	typedef HRESULT (__cdecl * DLL_CC_LAUNCH)(wchar_t* pszGameAbbr, LANGUAGE_VERSION nLanuage);
	HRESULT StartArcNorthAmerica(wchar_t* pszGameAbbr, LANGUAGE_VERSION nLanuage);
	LANGUAGE_VERSION GetLanguageVersion();
	HMODULE m_hArcNorthAmericaHandle;
// 北美arc end
	CNewSkinApp();
	void CalcCommandParm();
	void CreateSkinWindow();
	BOOL GetLocalUpdatePack();
	BOOL PreUpdate();
	BOOL AutoClose();
	BOOL SetUpdateServer();
	BOOL SilentUpdate();
	BOOL FullCheck();
	BOOL GetNotifyWindowHandle();

	// 加参数/sepfile来使用老的散文件的更新方式
	BOOL SepFileUpdate();
	bool ImportStringTable(const TCHAR *pszFilename);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewSkinApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNewSkinApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BOOL CALLBACK NotifyOtherProgram(void *pList,LPCTSTR lCommand);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSKIN_H__7D0D1743_5F93_4358_8BE2_A65C320CB406__INCLUDED_)
