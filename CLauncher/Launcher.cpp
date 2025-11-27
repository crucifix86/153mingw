// Launcher.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Launcher.h"
#include "LauncherDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool bAutoClose;

ULONG_PTR gdiplusToken;
HMODULE hFuncInst;

typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
MYFUNC MyUpdateLayeredWindow;

/////////////////////////////////////////////////////////////////////////////
// CLauncherApp

BEGIN_MESSAGE_MAP(CLauncherApp, CWinApp)
	//{{AFX_MSG_MAP(CLauncherApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLauncherApp construction
CLauncherApp::CLauncherApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLauncherApp object

CLauncherApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLauncherApp initialization

void CLauncherApp::CalcCommandParm()
{
	CString cmdLine=::GetCommandLine();
//	MessageBox(NULL,cmdLine,NULL,MB_OK);
	int len=cmdLine.GetLength();
	int i=0;
	m_nParmSum=0;
	while(i<len)
	{
		while(i<len&&cmdLine[i]==TEXT(' '))
			i++;
		if(i>=len)
			return;
		if(cmdLine[i]==TEXT('"'))
		{
			i++;
			int j=i;
			while(i<len&&cmdLine[i]!=TEXT('"'))
				i++;
			m_cParm[m_nParmSum++]=cmdLine.Mid(j,i-j);
//			MessageBox(NULL,m_cParm[m_nParmSum-1],NULL,MB_OK);
			i++;
		}
		else
		{
			int j=i;
			while(i<len&&cmdLine[i]!=TEXT(' '))
				i++;
			m_cParm[m_nParmSum++]=cmdLine.Mid(j,i-j);
//			MessageBox(NULL,m_cParm[m_nParmSum-1],NULL,MB_OK);
		}
	}
}
DWORD dwNotifyWindow = 0;
WORD wNotifyEventID = 0;
BOOL CLauncherApp::GetNotifyWindowHandle()
{
	BOOL bGetWindow(FALSE), bGetEventID(FALSE);
	for(int i=1;i<m_nParmSum;i++) {
		if(m_cParm[i].GetLength()>3&&m_cParm[i].Left(3)==TEXT("/w:")) {
			CString tmp = m_cParm[i].Right(m_cParm[i].GetLength()-3);
			LPTSTR stopstring;
			dwNotifyWindow = wcstoul(LPCTSTR(tmp), &stopstring, 10);
			bGetWindow = TRUE;
		}
		if(m_cParm[i].GetLength()>3&&m_cParm[i].Left(3)==TEXT("/i:")) {
			CString tmp = m_cParm[i].Right(m_cParm[i].GetLength()-3);
			LPTSTR stopstring;
			DWORD dwEventID = wcstoul(LPCTSTR(tmp), &stopstring, 10);
			wNotifyEventID = WORD(dwEventID);
			bGetEventID = TRUE;
		}
	}
	HWND other = HWND(dwNotifyWindow);
	BOOL ret = bGetWindow && bGetEventID && IsWindow(other);
	if (!ret) dwNotifyWindow = 0;
	return ret;
}
BOOL CLauncherApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("PerfectWorldLauncherExist"));
	if (GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		hMutex = NULL;
		HWND hWnd=FindWindow(TEXT("PerfectWorldLauncherClass"),TEXT("Launcher"));
		if(hWnd)
		{
			SetForegroundWindow(hWnd);
			ShowWindow(hWnd,SW_SHOWNORMAL);
		}
		return FALSE;
	}
	WNDCLASS wc;
	::GetClassInfo(AfxGetInstanceHandle(),TEXT("#32770"),&wc);
	wc.lpszClassName=TEXT("PerfectWorldLauncherClass");
	AfxRegisterClass(&wc);
	
	TCHAR fn[300];
	GetModuleFileName(NULL,fn,300);
	int len=_tcslen(fn)-1;
	while(fn[len]!=TEXT('/')&&fn[len]!=TEXT('\\'))
		len--;
	fn[len]=TEXT('\0');
	SetCurrentDirectory(fn);

	CalcCommandParm();
	BOOL bFullCheck=FALSE;
	BOOL bUpdate=FALSE;
	for(int i=1;i<m_nParmSum;i++)
	{
		if(m_cParm[i]==TEXT("FullCheck"))
			bFullCheck=TRUE;
		if(m_cParm[i]==TEXT("/update"))
			bUpdate=TRUE;
		if (m_cParm[i] == TEXT("/autoclose"))
			bAutoClose = true;
		if(m_cParm[i] == TEXT("/hide"))
			bSilentUpdate = TRUE;	
	}

	hFuncInst = LoadLibrary(_T("User32.DLL"));
	MyUpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CLauncherDlg dlg(bFullCheck,bUpdate);
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	FreeLibrary(hFuncInst);
	
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return FALSE;
}
