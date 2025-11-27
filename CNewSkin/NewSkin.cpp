// NewSkin.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NewSkin.h"
#include "SkinWindow.h"
#include "SkinItem.h"
#include "XMLFile.h"
#include <Tlhelp32.h>
#include "EC_HttpGet.h"
#include "Update.h"
#include <ATLBASE.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString fnOldVersion=TEXT("../config/patcher/version.sw");
const CString fnNewVersion=TEXT("../config/patcher/newver.sw");
const CString fnServerConfigFile=TEXT("../config/server.txt");

extern CString g_ClientInstallPath;
extern CString fnInstallPath;

extern CRITICAL_SECTION ThreadSection;

const CString fnArcNorthAmericaDll = TEXT("..\\launcher\\ArcSDK.dll");
extern CString fnArcNorthAmericaInstaller;
extern bool IsArcNorthAmerica;
ARC_STATE_NORTHAMERICA Arc_State_NorthAmerica = STATE_OK;

unsigned int WM_SILENTUPDATE_CANCEL = 0;
DWORD dwNotifyWindow = 0;
WORD wNotifyEventID = 0;
UINT MessageHandleThread(LPVOID pParam);

//const CString fnServer=TEXT("http://patch.world2.cn/CPW/");
//CString fnPatcherServerName=fnServer+TEXT("patcher/");

/////////////////////////////////////////////////////////////////////////////
// CNewSkinApp

BEGIN_MESSAGE_MAP(CNewSkinApp, CWinApp)
	//{{AFX_MSG_MAP(CNewSkinApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewSkinApp construction

CNewSkinApp::CNewSkinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNewSkinApp object

CNewSkinApp theApp;
ULONG_PTR gdiplusToken;
HMODULE hFuncInst;

typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
MYFUNC MyUpdateLayeredWindow;

void CNewSkinApp::CreateSkinWindow()
{
	XMLLabel *label;
	label=m_xFile->RootLabel->FirstChildLabel;
	while(label!=NULL)
	{
		if(label->LabelType==TEXT("Style"))
			m_sWindowList->AddStyle(label);
		else if(label->LabelType==TEXT("DllHandle"))
			m_sWindowList->hDLL=LoadLibrary(label->Attrib->GetAttribute(TEXT("DllName")));
		else if(label->LabelType==TEXT("Option"))
			m_sWindowList->AddOption(label);
		else if(label->LabelType==TEXT("Group"))
			m_sWindowList->AddGroup(label);
		label=label->NextLabel;
	}
	label=m_xFile->RootLabel->FirstChildLabel;
	while(label!=NULL)
	{
		if(label->LabelType==TEXT("SkinWindow"))
			m_sWindowList->AddWindow(label);
		label=label->NextLabel;
	}
	
	bool b;
	do
	{
		Sleep(100);
		SkinWindowNode *sNode=m_sWindowList->sWindowHead;
		b=false;
		while((sNode=sNode->next)!=NULL)
			if(sNode->wthread!=NULL&&sNode->m_iState == SkinWindowNode::STATE_LIVE)
			{
				b=true;
				if(m_sWindowList->bExit)
				{
				//	PostMessage(sNode->window->m_hWnd,WM_COMMAND,IDCANCEL,0);
					sNode->m_iState = SkinWindowNode::STATE_CLOSED;
					WaitForSingleObject(sNode->wthread->m_hThread,INFINITE);
				}
			}
	}while(b||!m_sWindowList->bExit);
//	int i=0;
//	exit(0);
//	SuspendThread();
}

/////////////////////////////////////////////////////////////////////////////
// CNewSkinApp initialization

//void GetServerName()
//{
//	FILE *f;
//	TCHAR s[400];
//	fnPatcherServerName=fnServer+TEXT("patcher/");
//	if((f=_tfopen(fnServerConfigFile,TEXT("rb")))!=NULL)
//	{
//		TCHAR a[200],b[200];
//		_fgettc(f);
//		while(_fgetts(s,400,f)!=NULL)
//		{
//			_stscanf(s,TEXT("%s %s"),a,b);
//			if(_tcscmp(a,TEXT("Server"))==0)
//				fnPatcherServerName=CString(b)+TEXT("patcher/");
//			if(_tcscmp(a,TEXT("PatcherServer"))==0)
//				fnPatcherServerName=b;
//		}
//		fclose(f);
//	}
//}
//

BOOL CNewSkinApp::AutoClose()
{
	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i]==TEXT("/autoclose"))
			return TRUE;
	return FALSE;
}
BOOL CNewSkinApp::SepFileUpdate()
{
	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i]==TEXT("/sepfile"))
			return TRUE;
	return FALSE;
}
BOOL CNewSkinApp::PreUpdate()
{
	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i]==TEXT("/update"))
			return TRUE;
	return FALSE;
//	FILE *fStream;
//	PATCH::DownloadManager *mgr;
//	int iNewVersion=0;
//	int iOldVersion;
//	mgr = new PATCH::DownloadManager(PATCH::BLOCKING);
//	GetServerName();
//	mgr->GetByUrl(fnPatcherServerName+TEXT("version"),fnNewVersion,NULL,0);
//	if((fStream=_tfopen(fnNewVersion,TEXT("r")))!=NULL)
//	{
//		fscanf(fStream,"%d",&iNewVersion);
//		fclose(fStream);
//		DeleteFile(fnNewVersion);
//	}
//	if(iNewVersion==0)
//		return TRUE;
//	if((fStream=_tfopen(fnOldVersion,TEXT("r")))==NULL)
//		return FALSE;
//	else
//	{
//		fscanf(fStream,"%d",&iOldVersion);
//		fclose(fStream);
//		if(iOldVersion<1)
//			return FALSE;
//	}
//	int p=iNewVersion-iOldVersion;
//	if(p!=0)
//		return FALSE;
//	return TRUE;
}

void CNewSkinApp::CalcCommandParm()
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
	for(int i=1;i<m_nParmSum;i++)
	{
		if(m_cParm[i] != TEXT("/cleanpack") && 
			m_cParm[i] != TEXT("/update") && 
			m_cParm[i] != TEXT("/autoclose") &&
			m_cParm[i] != TEXT("/sepfile"))
		{
			if (!strParamToClient.IsEmpty())
				strParamToClient += TEXT(" ");				
			strParamToClient += m_cParm[i];
		}
	}
}

BOOL CNewSkinApp::GetLocalUpdatePack()
{
	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i].GetLength()>13&&m_cParm[i].Left(13)==TEXT("/localupdate:"))
		{
			fnLocalUpdatePack=m_cParm[i].Right(m_cParm[i].GetLength()-13);
			return TRUE;
		}
	return FALSE;
}

bool CNewSkinApp::ImportStringTable(const TCHAR *pszFilename)
{
	FILE *fStream;
	if((fStream=_wfopen(pszFilename,TEXT("rb")))!=NULL)
	{
		fgetwc(fStream);
		TCHAR s[400];
		while(fgetws(s,400,fStream)!=NULL)
		{
			int n = wcslen(s) - 1;
			while( n > 0 && (s[n] == '\r' || s[n] == '\n') )
				s[n--] = '\0';
			TCHAR *str = wcsstr(s, TEXT(" ")) + 1;
			if( str )
			{
				int id = _wtoi(s);
				strTable[id] = str;
			}
		}
		fclose(fStream);
	}

	IsArcNorthAmerica = strTable[180].GetLength() != 0 && strTable[181].GetLength() != 0;
	if (IsArcNorthAmerica) {
		strTable[181].MakeLower();
		fnArcNorthAmericaInstaller = CString(TEXT("..\\element\\")) + strTable[181];
	}

	return true;
}

BOOL CNewSkinApp::StartByArcNorthAmerica()
{
	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i]==TEXT("arc:1"))
			return TRUE;
	return FALSE;
}
LANGUAGE_VERSION CNewSkinApp::GetLanguageVersion()
{
	LANGUAGE_VERSION ret(ENGLISH);
	CString strLanguage = strTable[165].Right(2);
	if (strLanguage == TEXT("EN")) ret = ENGLISH;
	else if (strLanguage == TEXT("DE")) ret = GERMAN;
	else if (strLanguage == TEXT("FR")) ret = FRENCH;
	return ret;
}
HRESULT CNewSkinApp::StartArcNorthAmerica(wchar_t* pszGameAbbr, LANGUAGE_VERSION nLanuage)
{
	if (m_hArcNorthAmericaHandle) {
		DLL_CC_LAUNCH h_dll_Start = NULL;
		h_dll_Start = (DLL_CC_LAUNCH)::GetProcAddress(m_hArcNorthAmericaHandle,"CC_LaunchClient");
		if (h_dll_Start){
			return  h_dll_Start(pszGameAbbr, nLanuage);
				WriteElementLog(TEXT("CC_LaunchClient"));
				WriteElementLog(NULL);
		}
	}
	return S_FALSE;
}

BOOL CNewSkinApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	

#ifdef RUSSIA_PATCH
	RunJadeLoader();
#endif
	// 保存输入参数
	CalcCommandParm();

	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i]==TEXT("/cleanpack"))
			bCleanPack=true;

	// 写入注册表
	ImportStringTable(TEXT("stringtab.txt"));
	TCHAR fn[300];
	GetModuleFileName(NULL,fn,300);
	TCHAR PatherPath[MAX_PATH] = {0};
	_tcscpy(PatherPath,fn);

	int len=_tcslen(fn)-1;
	while(fn[len]!=TEXT('/')&&fn[len]!=TEXT('\\'))
		len--;
	fn[len]=TEXT('\0');

	SetCurrentDirectory(fn);

	len=_tcslen(fn)-1;
	while(fn[len]!=TEXT('/')&&fn[len]!=TEXT('\\'))
		len--;
	fn[len+1]=TEXT('\0');
	TCHAR InstallPath[MAX_PATH] = {0};
	_tcscpy(InstallPath,fn);
	fnInstallPath = TEXT("\"");
	fnInstallPath += InstallPath;
	TCHAR LauncherPath[MAX_PATH] = {0};
	_stprintf(LauncherPath,TEXT("%slauncher\\launcher.exe"),InstallPath);
	TCHAR ClientPath[MAX_PATH] = {0};
	_stprintf(ClientPath,TEXT("%selement\\elementclient.exe"),InstallPath);
	g_ClientInstallPath = ClientPath;
	TCHAR UninstallPath[MAX_PATH] = {0};
	_stprintf(UninstallPath,TEXT("%suninstall.exe"),InstallPath);

	GetModuleFileName(NULL,fn,300);

	len=_tcslen(fn);
	CRegKey cReg,cReg2;
	cReg.Create(HKEY_CLASSES_ROOT,TEXT(".cup"));
	cReg.SetValue(TEXT("CPWUpdatePack"));
	cReg.Close();

	cReg.Create(HKEY_CLASSES_ROOT,TEXT("CPWUpdatePack"));
	GetModuleFileName(NULL,fn,300);
	_tcscpy(fn+len,TEXT(",0"));
	cReg.SetKeyValue(TEXT("DefaultIcon"),fn);
	cReg.SetValue(strTable[48]);
	cReg.Close();

	cReg.Create(HKEY_CLASSES_ROOT,TEXT("CPWUpdatePack\\shell"));
	cReg2.Create(cReg.m_hKey,TEXT("open"));
	_tcscpy(fn+len,TEXT(" \"/localupdate:%1\""));
	cReg2.SetKeyValue(TEXT("command"),fn);
	cReg2.Close();
	cReg.Close();

	// stringtab里的165条用于区分不同版本客户端的安装路径
	cReg.Create(HKEY_CLASSES_ROOT,strTable[165]);
	cReg.SetKeyValue(TEXT("INSTALL_PATH"),InstallPath);
	cReg.SetKeyValue(TEXT("PATCHER_PATH"),PatherPath);
	cReg.SetKeyValue(TEXT("LAUNCHER_PATH"),LauncherPath);
	cReg.SetKeyValue(TEXT("CLIENT_PATH"),ClientPath);
	cReg.SetKeyValue(TEXT("UNINSTALL_PATH"),UninstallPath);

	cReg.SetStringValue(TEXT("INSTALL_PATH"),InstallPath);
	cReg.SetStringValue(TEXT("PATCHER_PATH"),PatherPath);
	cReg.SetStringValue(TEXT("LAUNCHER_PATH"),LauncherPath);
	cReg.SetStringValue(TEXT("CLIENT_PATH"),ClientPath);
	cReg.SetStringValue(TEXT("UNINSTALL_PATH"),UninstallPath);
	cReg.Close();

	InitializeCriticalSection(&ThreadSection);
	// 尝试启动北美arc
	if (TryStartArcNorthAmerica())
		return FALSE;

	GetNotifyWindowHandle();

	if (SilentUpdate())
		bSilentUpdate = TRUE;

	SetUpdateServer();

	if(GetLocalUpdatePack())
	{
		int i=0;
		HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("PerfectWorldInternationalPatcherExist"));
		while(GetLastError()==ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hMutex);
			hMutex = NULL;
			if (bSilentUpdate) {
				NotifyOtherProgram(NULL, TEXT("Name=PatcherAlreadyRunning"));
				return FALSE;
			}
			HWND hWnd=FindWindow(TEXT("PerfectWorldInternationalPatcherClass"),NULL);
			if(hWnd)
			{
				SendMessage(hWnd,WM_CLOSE,NULL,NULL);
				Sleep(500);
			}
			hMutex = CreateMutex(NULL, FALSE, TEXT("PerfectWorldInternationalPatcherExist"));
			i++;
			if(i>=5)
			{
				MessageBox(NULL,strTable[49],NULL,MB_OK);
				return FALSE;
			}
		}
	}
	else
	{
		HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("PerfectWorldInternationalPatcherExist"));
		if (GetLastError()==ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hMutex);
			hMutex = NULL;
			if (bSilentUpdate) {
				NotifyOtherProgram(NULL, TEXT("Name=PatcherAlreadyRunning"));
				return FALSE;
			}
			HWND hWnd=FindWindow(TEXT("PerfectWorldInternationalPatcherClass"),NULL);
			if(hWnd)
			{
				SetForegroundWindow(hWnd);
				ShowWindow(hWnd,SW_SHOWNORMAL);
			}
			return FALSE;
		}
		if(PreUpdate())
			bUpdated=true;
		if (AutoClose())
		{
			bAutoClose = true;
		}
		if (SepFileUpdate())
		{
			bSepFileUpdate = true;
		}
	}
	WM_SILENTUPDATE_CANCEL = ::RegisterWindowMessage(TEXT("Arc_Cancel_SilentUpdate_Message"));
	WNDCLASS wc;
	::GetClassInfo(AfxGetInstanceHandle(),TEXT("#32770"),&wc);
	wc.lpszClassName=TEXT("PerfectWorldInternationalPatcherClass");
	AfxRegisterClass(&wc); 
	if (!bSilentUpdate) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		m_xFile=new XMLFile;
		m_sWindowList=new SkinWindowList;
#ifdef _UNICODE
		m_xFile->LoadFile(TEXT("skin/mainuni.xml"));
#else
		m_xFile->LoadFile(TEXT("skin/main.xml"));
#endif

		hFuncInst = LoadLibrary(_T("User32.DLL"));
		MyUpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");
		CreateSkinWindow();

		delete m_sWindowList;
		delete m_xFile;
		FreeLibrary(hFuncInst);

		Gdiplus::GdiplusShutdown(gdiplusToken);
	} else {
		AfxBeginThread(MessageHandleThread, 0);
		if (FullCheck()) {
			UpdateFunc(NotifyOtherProgram, TEXT("FullCheck"), TEXT(""), NULL);
		} else {
			UpdateFunc(NotifyOtherProgram, TEXT("Update"), TEXT(""), NULL);
		}
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
BOOL CNewSkinApp::TryStartArcNorthAmerica()
{
	BOOL ret = FALSE;
	if (IsArcNorthAmerica) {
		WriteElementLog(TEXT("北美arc"));
		WriteElementLog(NULL);
		if (!StartByArcNorthAmerica()) {
			WriteElementLog(TEXT("启动参数里没有arc:1"));
			WriteElementLog(NULL);
			m_hArcNorthAmericaHandle = LoadLibrary(fnArcNorthAmericaDll);
			LANGUAGE_VERSION lan_ver = GetLanguageVersion();
			int ret = StartArcNorthAmerica(TEXT("pwi"), lan_ver);
			bool bInstallerExist = PathFileExists(fnArcNorthAmericaInstaller);
			if (ret == CC_ERR_CLIENT_NOT_INSTALLED) {
				if (GetPrivateProfileIntA("Patcher", "Installer", 0, "..\\element\\patcher.ini") == 1) {
					if (bInstallerExist) Arc_State_NorthAmerica = NEED_INSTALL;
					else Arc_State_NorthAmerica = NEED_DOWNLOAD;
				} else if (bInstallerExist) {
					Arc_State_NorthAmerica = NEED_INSTALL;
					char szText[10];
					sprintf(szText, "%d", 1);
					WritePrivateProfileStringA("Patcher", "Installer", szText, "..\\element\\patcher.ini");
				}
			} else {
				Arc_State_NorthAmerica = STATE_OK;
				if (ret == S_OK) {
					WriteElementLog(TEXT("CC_LaunchClient返回S_OK，patcher即将退出"));
					WriteElementLog(NULL);
					ret = TRUE;
				}
			}
			FreeLibrary(m_hArcNorthAmericaHandle);
		} else {
			WriteElementLog(TEXT("启动参数里有arc:1"));
			WriteElementLog(NULL);
		}
	}
	return ret;
}

BOOL CNewSkinApp::SilentUpdate()
{
	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i]==TEXT("/hide"))
			return TRUE;
	return FALSE;
}
BOOL CNewSkinApp::FullCheck()
{
	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i]==TEXT("/fullcheck"))
			return TRUE;
	return FALSE;
}
BOOL CNewSkinApp::GetNotifyWindowHandle()
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
BOOL CNewSkinApp::SetUpdateServer()
{
	for(int i=1;i<m_nParmSum;i++)
		if(m_cParm[i].GetLength()>14&&m_cParm[i].Left(14)==TEXT("/updateserver:"))
		{
			UpdateServerName=m_cParm[i].Right(m_cParm[i].GetLength()-14);
			return TRUE;
		}
		return FALSE;
}
enum
{
	OK,								//无错误
	PATCHER_HAS_BEEN_RUNNING,		//patcher已经运行
	PATCHER_NEED_UPDATED,			//patcher需要更新
	WRONG_VERSION_PATCH,			//补丁包版本错误
	PATCH_VALIDATION_FAILURE,		//补丁包校验失败
	INVALID_UPDATE_SERVER,			//更新服务器需要设置
	SERVER_MAINTAIN,				//更新服务器正在维护
	SERVER_CONNECTFAILUR,			//更新服务器连接失败
	SERVER_REGION_ERROR,			//服务器不匹配
	SERVER_VERSION_ERROR,			//版本号错误，建议版本验证
	UPDATE_FILE_LIST_ERROR,			//更新文件列表错误
	UNKNOWN_ERROR,					//其他错误
};
enum
{
	UPDATE_ERROR,					// LPARAM = error code
	PROGRESS,						// LPARAM = 0 ~ 100
	COMPLETE,						// LPARAM = 0
	PATCHER_UPDATE_COMPLETE,		// LPARAM = 0
	NOTIFY_OTHER_MY_HANDLE,			
};

#define PW_PATCHER_MSG WM_USER + 2014

BOOL CALLBACK NotifyOtherProgram(void *pList,LPCTSTR lCommand)
{
//	if (dwNotifyWindow == 0) return FALSE;
	HWND other = HWND(dwNotifyWindow);
	CString cName=lCommand;
	XMLLabelAttribute *xAttrib = String2Attirb(cName);
	CString command = xAttrib->GetAttribute(TEXT("Name"));
	DWORD wparam, lparam;
	WORD event = 65535;
	if (command == TEXT("LocalPackError")) {
		event = UPDATE_ERROR;
		lparam = UNKNOWN_ERROR;

	} else if (command == TEXT("UpdatePackError")) {
		event = UPDATE_ERROR;
		lparam = PATCH_VALIDATION_FAILURE;
	} else if (command == TEXT("UpdatePatcher")) {
		event = UPDATE_ERROR;
		lparam = PATCHER_NEED_UPDATED;
	} else if (command == TEXT("PackVersionError")) {
		event = UPDATE_ERROR;
		lparam = WRONG_VERSION_PATCH;
	} else if (command == TEXT("SetProgressPos")) {
		CString item = xAttrib->GetAttribute(TEXT("Item"));
		if (item == "TotalProgress") {
			event = PROGRESS;
			lparam = String2Int(xAttrib->GetAttribute(TEXT("Pos")));
		}
	} else if (command == TEXT("SilentUpdateSucess")) {
		event = COMPLETE;
		lparam = OK;
	} else if (command == TEXT("InvalidUpdateServer")) {
		event = UPDATE_ERROR;
		lparam = INVALID_UPDATE_SERVER;		
	} else if (command == TEXT("ServerMaintaining")) {
		event = UPDATE_ERROR;
		lparam = SERVER_MAINTAIN;		
	} else if (command == TEXT("ServerConnectFailure")) {
		event = UPDATE_ERROR;
		lparam = SERVER_CONNECTFAILUR;		
	} else if (command == TEXT("ServerRegionError")) {
		event = UPDATE_ERROR;
		lparam = SERVER_REGION_ERROR;		
	} else if (command == TEXT("VersionError")) {
		event = UPDATE_ERROR;
		lparam = SERVER_VERSION_ERROR;		
	} else if (command == TEXT("UpdateFileListError")) {
		event = UPDATE_ERROR;
		lparam = UPDATE_FILE_LIST_ERROR;		
	} else if (command == TEXT("PatcherAlreadyRunning")) {
		event = UPDATE_ERROR;
		lparam = PATCHER_HAS_BEEN_RUNNING;
	} else if (command == TEXT("NotifyOtherMyMessageWndHandle")) {
		event = NOTIFY_OTHER_MY_HANDLE;
		lparam = (DWORD)pList;
	}
	if (event != 65535) {
		wparam = MAKEWPARAM(event, wNotifyEventID);
		PostMessage(other, PW_PATCHER_MSG, wparam, lparam);
	}
	delete xAttrib;

	return TRUE;
}
LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool b = false;	
	if (message == WM_SILENTUPDATE_CANCEL) {

		UpdateFunc(NULL, TEXT("StopThread"), TEXT(""), NULL);
	}

	return b ? 0 : DefWindowProc(hWnd, message, wParam, lParam);
}
ATOM _RegisterWndClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof (WNDCLASSEX); 

	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)_WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= AfxGetInstanceHandle();
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= TEXT("MessageHandleWnd");
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}
UINT MessageHandleThread(LPVOID pParam)
{
	_RegisterWndClass();
	HWND hMessageWnd = CreateWindowEx(0, TEXT("MessageHandleWnd"), TEXT(""), 0, 0, 0, 0, 0, NULL, 0, AfxGetInstanceHandle(), 0);
	NotifyOtherProgram(hMessageWnd, TEXT("Name=NotifyOtherMyMessageWndHandle"));
	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}