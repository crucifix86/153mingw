// Update.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
#include <vector>
using namespace std;

#include "SysInfo.h"
#include "Update.h"
#include "XMLFile.h"
#include "packdll.h"
#include "Shlwapi.h"
#include <Tlhelp32.h>
#include <d3d9.h>
#include <afxmt.h>

#include "PSAPI.H"
#pragma comment( lib,"PSAPI.LIB" )

#include "PackUpdater.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct ExeConfig
{
	CString FileName;
	bool ExecuteEverytime;			// true表示每次都运行，false表示有更新时才运行
	bool GiveElementPath;			// true表示带element目录参数，false表示不带
	bool ExecuteBeforeClosePatcher; // true表示运行后关闭patcher
	bool Updated;					// true表示有更新
	CString Param;					// exe启动参数
	int	HintInStringtab;			// -1表示不提示运行，否则显示stringtab.txt相应的提示对话框
	bool ExecuteByCheckVersion;		// 通过检查版本来判断是否运行
};
typedef std::vector<ExeConfig> EXEINI;
EXEINI Exes;
CString fnInstallPath;

std::vector<pair<CString,LONGLONG>> ServerConnectingTime;
CRITICAL_SECTION ServerConnectingTimeMapSection;
bool bSuggestNewServer = false;
LONGLONG SelectedServerConnectingTime = -1;
CRITICAL_SECTION ServerSection;
CString fnLocalUpdatePack="";
DWORD SaveUpdateStateSize=20000;
DWORD ReconnectTime=20000;
DWORD TotalTimeout=60000;
DWORD MaxWaitTime=10000;
DWORD ReconnectTime2=2000;
DWORD TotalTimeout2=5000;
const CString fnErrorLog=TEXT("../config/element/fileerr.log");
const CString fnErrorLog1=TEXT("../config/element/fileerr.001");
const int MaxLogFileSize=300000;
const CString fnServerConfigFile=TEXT("../config/server.txt");
CString fnServer=TEXT("http://patch.world2.cn/CPW/");
CString fnPackServer;
const int CodePage=936;
const int FileSizeOnDraw=8192;
const int NumbersOfLogFile=6;
int pid=1;
const CString StartupFile=TEXT("../element/ElementClient.exe");

const CString fnUpdateConfigFile=TEXT("../config/patcher/update.cfg");
const CString fnVideoConfigFile=TEXT("../config/patcher/video.cfg");
CString fnPIDServer=fnServer+TEXT("info/pid");
const CString fnPIDFile=TEXT("../config/pid.sw");
const CString fnPackPIDFile=TEXT("pid");
const CString LauncherTempFile=TEXT("../config/launcher/tempfile.dat");
const CString DownloadTempFile=TEXT("../config/element/tempfile.dat");
extern const CString DownloadTempFile2=TEXT("../config/element/tempfile2.dat");
const CString fnInfoURL=TEXT("http://www.world2.com.cn/CPW/info1/clientindex.html");

const char fnSystemSettings[]="../element/userdata/systemsettings.ini";
const char fnSystemInfo[]="../config/patcher/sysinfo.ini";
const CString cfnSystemSettings=TEXT("../element/userdata/systemsettings.ini");

CString fnPatcherServerName=fnServer+TEXT("patcher/");
const CString fnPatcherOldVersion=TEXT("../config/patcher/version.sw");
const CString fnPatcherNewVersion=TEXT("../config/patcher/newver.sw");

CString fnLauncherServerName=fnServer+TEXT("launcher/");
CString fnLauncherServerPath=fnLauncherServerName+TEXT("launcher/");
const CString fnLauncherPath=TEXT("../launcher/");
const CString fnLauncherCheckPath=TEXT("../config/launcher/check/");
const CString fnLauncherList=TEXT("../config/launcher/FullList.sw");
const char fnLauncherExeProcess[]="launcher.exe";

CString fnElementPatchName=fnServer+TEXT("patches/");
CString fnElementServerName=fnServer+TEXT("element/");
CString fnElementServerPath=fnElementServerName+TEXT("element/");
extern const CString fnElementOldVersion=TEXT("../config/element/version.sw");
const CString fnElementNewVersion=TEXT("../config/element/newver.sw");
const CString fnUpdateList=TEXT("../config/element/inc.sw");
const CString fnUpdateState=TEXT("../config/element/ustate.sw");
const CString fnFullCheckList=TEXT("../config/element/FullList.sw");
const CString fnFullListVersion=TEXT("../config/element/Listver.sw");
const CString fnPath=TEXT("../element/");

const CString fnFlagDDSPathToDownload=TEXT("../config/element/a");
const CString fnFlagTXTPathToDownload=TEXT("../config/element/b");


const CString fnElementLogName=TEXT("../config/element/update.");
const CString fnElementLogFile=fnElementLogName+TEXT("log");
const CString fnLauncherLogName=TEXT("../config/launcher/update.");
const CString fnLauncherLogFile=fnLauncherLogName+TEXT("log");

CString g_WindowNameInternational = TEXT("Element Client");
CString g_WindowNameDomestic = TEXT("完美世界国际版");
CString g_ClientInstallPath;
bool bIsOverseaVersion = true;

const CString fn360InstallerName = TEXT("360inst_wanmeigj.exe");
const CString fnDXWebInstaller = TEXT("dxwebsetup.exe");

int UpdateState = Updater::STATE_SEP_UPDATE;

volatile float fFileProgress = 0.f;
int LastProgress = 0;

CString UpdateServerName="";
BOOL bSilentUpdate = FALSE;

CString fnArcNorthAmericaInstaller;
bool IsArcNorthAmerica = false;
void CheckArcNorthAmericaInstaller(const TCHAR* strName);

FuncType CallFunc;
bool bFileDown;
void *pList;
FILE *fStream;
Base64Code base64;
PATCH::MD5Hash md5hash;
int iUpdatedLine;
int iFinishedOffset;
int iTotalLine;
int iLineNumber;
int iFileSize;
int iOldVersion;
int iNewVersion;
int iOldVersion1;
int iNewVersion1;
int iUpdatingVersion;
BOOL bEqual;
BOOL bResume;
int nSelect;
BOOL bStopThread;
BOOL bWritingFile;
char md5[100];
CWinThread *cThread;
SysInfo sysInfo;
bool bWaitingServer=false;
bool bSelectServer=true;
bool bAutoUpdating=false;
bool bP2SPDownload=false;
int iP2SPDownSpeed=0;
int iP2SPUpSpeed=0;
bool bQuickUpdate=false;
bool bCleanPack=false;
bool bUpdated=false;
bool bAutoClose = false;
bool bInvalidUpdateServer=false;
bool bUpdatedURL=false;
bool bUpdating=false;
bool bCheckDisk=true;
bool bCanPlay=false;
bool bUSA=false;
bool bSepFileUpdate = false;
bool bTurningFormPackUpdate = false;

CString strTable[200];
CString strParamToClient;
#ifdef SERVER_CHECK
volatile bool bServerSwitched = false;
//bool bNeedFullCheck = false;
bool bServerChecked = false;
volatile bool bServerLooksOK = false;
volatile bool bServerOK = false;
volatile bool bDonotChangeServer = false;
tm tmStartCheck = {0,0,12,31,4,112,0,0,0};

bool CheckServer(CString strServer);

#endif

CRITICAL_SECTION ThreadSection;

bool bIs360NeedUpdate = false;
static const int DX_VERSION_MAJOR = 9;
static const int DX_VERSION_MINOR = 0;
static const TCHAR DX_VERSION_LETTER = _T('c');

CEvent g_EventThreadPrepareToDie;
void ExePerUpdate();
bool ExeBeforClose();
void SuggestSetting();
void FlagUpdateNonThread();
void SaveVideoSetting();
void LoadVideoSetting(const char *fnSettingFile="");
void CalcDisplayMode();
void Play();
HRESULT GetDXVersion( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, WCHAR* pcDirectXVersionLetter );
BOOL CalFileMd5(LPCTSTR lName,char *md5);
UINT Update(LPVOID pParam);
void BeginThread(AFX_THREADPROC proc,CWinThread *&thread);
void CheckExeUpdate(TCHAR* UpdateFileName);

struct UpdateCfgEntry{
	CString name;
	CString value;
	UpdateCfgEntry(TCHAR* p1, TCHAR* p2):
	name(p1),
		value(p2)
	{

	}
	UpdateCfgEntry(){}
};

void SaveUpdateServer()
{
	if (bSilentUpdate && !UpdateServerName.IsEmpty()){
		fStream=_tfopen(fnUpdateConfigFile,TEXT("rb"));
		if(fStream==NULL)
			return;
		_fgettc(fStream);
		TCHAR s[400];
		TCHAR cItemName[100];
		TCHAR c[400];
		UpdateCfgEntry FileContent[10];
		size_t count = 0;
		while(_fgetts(s,400,fStream)!=NULL && count < 10)
		{
			_stscanf(s,TEXT("%s %s"),cItemName,c);
			FileContent[count++] = (UpdateCfgEntry(cItemName, c));
			if (FileContent[count - 1].name == TEXT("UpdateServerSelect"))
				FileContent[count - 1].value = UpdateServerName;
		}
		fclose(fStream);

		fStream=_tfopen(fnUpdateConfigFile,TEXT("wb"));
		if(fStream==NULL)
			return;
		_fputtc(0xfeff,fStream);
		size_t i = 0;
		while(i < count) {
			UpdateCfgEntry entry = FileContent[i++];
			_stprintf(s,TEXT("%s %s\r\n"), entry.name, entry.value);
			_fputts(s,fStream);			
		}

		fclose(fStream);
	}

}
void SetP2SPSpeed(const CString strSpeed,int& iSpeed)
{
	int iLen = strSpeed.GetLength();
	if (iLen == 0)
	{
		iSpeed = 0;
		return;
	}
	
	if (strSpeed.GetAt(iLen - 2) >= _T('0') && strSpeed.GetAt(iLen - 2) <= _T('9'))
	{
		if (strSpeed.GetAt(iLen - 1) == _T('k') || strSpeed.GetAt(iLen - 1) == _T('K'))
		{
			iSpeed = _tstoi(strSpeed.Left(iLen - 1)) * 1024;//字节数
			return;
		}
		else if (strSpeed.GetAt(iLen - 1) == _T('m') || strSpeed.GetAt(iLen - 1) == _T('M'))
		{
			iSpeed = _tstoi(strSpeed.Left(iLen - 1)) * 1024 * 1024;
			return;
		}
	}
	iSpeed = 0;
}

CString GetFullPath(LPCTSTR fn)
{
	TCHAR path[1000];
	GetCurrentDirectory(1000, path);
	_tcscat(path, TEXT("/"));
	_tcscat(path, fn);
	return path;
}

FILE* OpenFile(LPCTSTR name, LPCTSTR param)
{
	FILE* f=NULL;
	int i=0;
	while(i<=10&&(f=_tfopen(name, param))==NULL)
	{
		Sleep(30);
		i++;
	}
	return f;
}

void ThreadLock()
{
	EnterCriticalSection(&ThreadSection);
	if(bStopThread)
	{
		LeaveCriticalSection(&ThreadSection);
		ExitThread(0);
	}
	bWritingFile=TRUE;
	LeaveCriticalSection(&ThreadSection);
}

void ThreadUnLock()
{
	EnterCriticalSection(&ThreadSection);
	bWritingFile=FALSE;
	if(bStopThread)
	{
		LeaveCriticalSection(&ThreadSection);
		ExitThread(0);
	}
	LeaveCriticalSection(&ThreadSection);
}

void SetUpdateState(int state)
{
	char szText[10];
	sprintf(szText, "%d", state);
	WritePrivateProfileStringA("Patcher", "Updated", szText, "..\\element\\patcher.ini");
}

void SaveUpdateState(int iNewVersion1,int iLineNumber,int iFinishedOffset,int iFileSize)
{
	FILE *f;
	if((f=OpenFile(fnUpdateState,TEXT("w")))!=NULL)
	{
		fprintf(f,"%d %d %d %d\r\n",iNewVersion,iLineNumber,0,iFileSize);
		fclose(f);
	}
}
void DownloadFileByHttp(CString src, CString des, callback_t callback, unsigned int start,int iTryTimes)
{		
	FILE *fStream=NULL;		
	int try_times = 0;
	do 
	{
		PATCH::DownloadManager::GetSingleton().GetByUrl(src,des,callback,start);

		fStream=OpenFile(des,TEXT("r"));
		++try_times;
		//Sleep(30);
	} 
	while ((fStream==NULL) && (try_times<iTryTimes));

	if(fStream!=NULL)
		fclose(fStream);

	// 多线程调用WriteElementLog会有冲突
/*
	if (try_times>1)
	{
		EnterCriticalSection(&ThreadSection);
		TCHAR st[64] = {0};
		_stprintf(st,TEXT(" repeat times %d"),try_times);

		WriteElementLog(des);
		WriteElementLog(st);
		WriteElementLog(NULL);
		LeaveCriticalSection(&ThreadSection);
	}*/
}

void DownloadFile(CString src, CString des, callback_t callback, unsigned int start, bool bForceInternet = false)
{
	if(fnLocalUpdatePack=="" || bForceInternet)
	{
		int count = 5;
		if(fnServer[0]==TEXT(' ')) count = 1;
		DownloadFileByHttp(src,des,callback,start,count);
	}
	else
	{
		bFileDown=true;
		if(	src.GetLength()>fnElementServerName.GetLength()&&
			fnElementServerName==src.Left(fnElementServerName.GetLength()))
		{
			src=src.Right(src.GetLength()-fnElementServerName.GetLength());
			int i=0;
			while(i<src.GetLength()-2)
			{
				if(src[i]==TEXT('.')&&src[i+1]==('/'))
					src=src.Left(i)+src.Right(src.GetLength()-i-2);
				else
					i++;
			}
			TCHAR st1[400],st2[400];
			_tcscpy(st1,src);
			_tcscpy(st2,des);
			GetLocalUpdateFile(st1,st2);
		}
	}
}

void UCopyFile(LPCTSTR src,LPCTSTR des,BOOL b)
{
	SetFileAttributes(des, FILE_ATTRIBUTE_NORMAL);
	if(!CopyFile(src,des,b))
	{
		ThreadLock();
		FILE *f=OpenFile(fnErrorLog,TEXT("ab"));
		if(f!=NULL)
		{
			SYSTEMTIME sTime;
			GetLocalTime(&sTime);
			TCHAR s[400];
			_stprintf(s,TEXT("%d/%d/%d %02d:%02d:%02d"),
					sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
			_fputts(s,f);
			_stprintf(s,strTable[1],src,des);
			_fputts(s,f);
			_fputts(TEXT("\r\n"),f);
			fclose(f);
		}
		ThreadUnLock();
	}
}

void UDeleteFile(LPCTSTR src)
{
	SetFileAttributes(src, FILE_ATTRIBUTE_NORMAL);
	DeleteFile(src);
}

void Command(void *pList,LPCTSTR lCommand)
{
	ThreadLock();
	CallFunc(pList,lCommand);
	ThreadUnLock();
}

CString Character2WChar(char *c)
{
	CString res="";
	for(int i=0;i<strlen(c);i++)
		res+=c[i];
	return res;
}

int GetFileSize(LPCTSTR lFileName)
{
	FILE *f;
	if((f=OpenFile(lFileName,TEXT("rb")))!=NULL)
	{
		unsigned int maxsize = fseek(f,0, SEEK_END);
		maxsize = ftell(f);
		fclose(f);
		return maxsize;
	}
	else
		return -1;
}

XMLLabelAttribute* LoadOptionFile(LPCTSTR lName)
{
	FILE *fStream;
	XMLLabelAttribute *xAttrib=new XMLLabelAttribute;
	if((fStream=OpenFile(lName,TEXT("rb")))!=NULL)
	{
#ifdef _UNICODE
		_fgettc(fStream);
#endif
		TCHAR s[400];
		while(_fgetts(s,400,fStream)!=NULL)
		{
			TCHAR cItemName[100];
			TCHAR c[400];
			_stscanf(s,TEXT("%s %s"),cItemName,c);
			Attribute *attribNode=new Attribute;
			attribNode->cName=cItemName;
			attribNode->cValue=c;
			xAttrib->AddAttribute(attribNode);
		}
		fclose(fStream);
	}
	return xAttrib;
}

#ifdef SERVER_CHECK
bool CheckServer(CString strServer)
{
	// 检查backup server 上的base文件
	char md5[100];
	bool bNeedDownloadList = false;
	UDeleteFile(DownloadTempFile2);
	DownloadFile(strServer + TEXT("md5"),DownloadTempFile2,NULL,0);
	FILE *fStream=OpenFile(DownloadTempFile2,TEXT("r"));
	char md[50] = {0};
	char md5ServerList[50] = {0};
	if(fStream!=NULL)
	{
		WriteElementLog(TEXT("md5 file downloaded!"));
		WriteElementLog(NULL);
		fscanf(fStream,"%s %s",md5,md5ServerList);
		fclose(fStream);
		_strlwr(md5);
		_strlwr(md5ServerList);
		// base文件内容空，说明backup server没有准备好
		if (strlen(md5) == 0 || strlen(md5ServerList) == 0)
		{	
			WriteElementLog(TEXT("md5 file incomplete!"));
			WriteElementLog(NULL);
			return false;
		}
		if((fStream=OpenFile(fnElementOldVersion,TEXT("r")))==NULL)
		{
			WriteElementLog(TEXT("version.sw open failed!"));
			WriteElementLog(NULL);
			return false;
		}

		//能下载到标记文件，通知update线程等待下载list
		bServerLooksOK = true;
		
		fscanf(fStream,"%d %d",&iOldVersion,&iOldVersion1);
		fclose(fStream);

		CalFileMd5(TEXT("./server/updateserver.txt"),md);
		bool bSwitched = (strcmp(md,md5)==0);
		WriteElementLog(CString(TEXT("updateserver.txt md5 = ")) + md);
		WriteElementLog(NULL);

		CalFileMd5(TEXT("./server/serverlist.txt"),md);
		bServerSwitched = bSwitched && (strcmp(md,md5ServerList)==0);
		WriteElementLog(CString(TEXT("serverlist.txt md5 = ")) + md);
		WriteElementLog(NULL);

		if (bServerSwitched)
		{
			bNeedDownloadList = false;
		}
		else
		{
			bNeedDownloadList = true;
		}
	}

	if (bNeedDownloadList)
	{
		UDeleteFile(DownloadTempFile2);
		DownloadFile(strServer + TEXT("updateserver.txt"),DownloadTempFile2,NULL,0);
		fStream=OpenFile(DownloadTempFile2,TEXT("r"));
		if (fStream)
		{
			CalFileMd5(DownloadTempFile2,md);
			if (strcmp(md,md5) == 0)
			{
				bServerLooksOK = true;
				UCopyFile(DownloadTempFile2,TEXT("./server/updateserver.txt"),FALSE);
				WriteElementLog(TEXT("updateserver.txt switched!"));
				WriteElementLog(NULL);
			}
			else
			{
				bServerLooksOK = false;
				WriteElementLog(TEXT("downloaded updateserver.txt deffers form original!"));
				WriteElementLog(NULL);
			}
			
		}
		else
		{
			// 没有下载到txt文件，说明backup server没有准备好
			WriteElementLog(TEXT("download updateserver.txt failed!"));
			WriteElementLog(NULL);
			bServerLooksOK = false;
		}

		UDeleteFile(DownloadTempFile2);
		DownloadFile(strServer + TEXT("serverlist.txt"),DownloadTempFile2,NULL,0);
		fStream=OpenFile(DownloadTempFile2,TEXT("r"));
		if (fStream)
		{
			CalFileMd5(DownloadTempFile2,md);
			if (strcmp(md,md5ServerList) == 0)
			{
				bServerLooksOK = true;
				UCopyFile(DownloadTempFile2,TEXT("./server/serverlist.txt"),FALSE);
				WriteElementLog(TEXT("serverlist.txt switched!"));
				WriteElementLog(NULL);
			}
			else
			{
				bServerLooksOK = false;
				WriteElementLog(TEXT("downloaded serverlist.txt deffers form original!"));
				WriteElementLog(NULL);
			}
			
		}
		else
		{
			// 没有下载到txt文件，说明backup server没有准备好
			bServerLooksOK = false;
			WriteElementLog(TEXT("download serverlist.txt failed!"));
			WriteElementLog(NULL);
		}
	}

	UDeleteFile(DownloadTempFile2);

	return bNeedDownloadList;
}

#endif
struct SpeedTestIndexInfo
{
	int index;
	TCHAR *pStr;
	bool used;
};
std::vector<SpeedTestIndexInfo*> ToDel;
void DelSpeedTestIndexInfo()
{
	int len = ToDel.size();
	for (int i = 0;i < len;++i)
	{
		if (ToDel[i]->used)
		{
			delete [](ToDel[i]->pStr);
			ToDel[i]->pStr = NULL;
			delete ToDel[i];
			ToDel[i] = NULL;
		}
	}
}
UINT ConnectionSpeedTest(LPVOID pParam)
{
	SpeedTestIndexInfo* info = (SpeedTestIndexInfo*)pParam;
	CString strUrl(info->pStr);
	int index = info->index;
	info->used = true;
	LARGE_INTEGER liStart,liEnd;

	EnterCriticalSection(&ServerConnectingTimeMapSection);
	CString ServerName(ServerConnectingTime[index].first);
	LeaveCriticalSection(&ServerConnectingTimeMapSection);
	CString localName = fnPIDFile + ServerName;
	if (!QueryPerformanceCounter(&liStart))
	{
		return 0;
	}
	DownloadFile(strUrl + TEXT("info/pid"),localName,NULL,0);
	if(!QueryPerformanceCounter(&liEnd))
	{
		return 0;
	}
	int iNewPID=0;
	bool bIsValidServer = false;
	if((fStream=OpenFile(localName,TEXT("r"))) != NULL)
	{
		fscanf(fStream,"%d",&iNewPID);
		fclose(fStream);
		if (iNewPID == pid)
		{
			bIsValidServer = true;
		}
	}
	if (bIsValidServer)
	{
		EnterCriticalSection(&ServerConnectingTimeMapSection);
		ServerConnectingTime[index].second = liEnd.QuadPart - liStart.QuadPart;
		LeaveCriticalSection(&ServerConnectingTimeMapSection);
	}
	UDeleteFile(localName);
	return 0;
}
void GetServer()
{
	bInvalidUpdateServer=true;
	EnterCriticalSection(&ServerSection);
	ThreadLock();
	FILE *f;
	if((f=OpenFile(fnErrorLog,TEXT("rb")))==NULL)
	{
		if((f=OpenFile(fnErrorLog,TEXT("wb")))!=NULL)
		{
			_fputtc(0xfeff,f);
			fclose(f);
		}
	}
	else 
	{
		fclose(f);
		if(GetFileSize(fnErrorLog)>1000000)
		{
			UCopyFile(fnErrorLog,fnErrorLog1,false);
			if((f=OpenFile(fnErrorLog,TEXT("wb")))!=NULL)
			{
				_fputtc(0xfeff,f);
				fclose(f);
			}
		}
	}

	fnLauncherServerName=fnServer+TEXT("launcher/");
	fnElementServerName=fnServer+TEXT("element/");
	fnElementPatchName=fnServer+TEXT("patches/");
	XMLLabelAttribute *xOption=LoadOptionFile(fnUpdateConfigFile);
	CString ctemp;
	ctemp=xOption->GetAttribute(TEXT("SelectServerEveryTime"));
	if(ctemp==TEXT("0") || bAutoClose)
		bSelectServer=false;
	ctemp=xOption->GetAttribute(TEXT("P2SPDownload"));
	if (bIsOverseaVersion)
	{
		bP2SPDownload = false;
	}
	else if (ctemp==TEXT("1"))
	{
		bP2SPDownload = true;
	}
	else
		bP2SPDownload=false;
	ctemp=xOption->GetAttribute(TEXT("UpSpeedSelect"));
	SetP2SPSpeed(ctemp,iP2SPUpSpeed);
	ctemp=xOption->GetAttribute(TEXT("DownSpeedSelect"));
	SetP2SPSpeed(ctemp,iP2SPDownSpeed);
	ctemp=xOption->GetAttribute(TEXT("AutoUpdating"));
	if(ctemp==TEXT("1") || bAutoClose)
		bAutoUpdating=true;
	if (bSilentUpdate && !UpdateServerName.IsEmpty()) {
		ctemp = UpdateServerName;
	} else {
		ctemp=xOption->GetAttribute(TEXT("UpdateServerSelect"));
	}
	TCHAR cServer[300];
	TCHAR cPackServer[300] = {0};
	TCHAR cName[300];
	TCHAR s[400];
	if(ctemp!="")
	{
		if((f=OpenFile(TEXT("./server/updateserver.txt"),TEXT("rb")))!=NULL)
		{
			TCHAR c;
			_ftscanf(f,TEXT("%c"),&c);
			int index = 0;
			while(_fgetts(s,400,f)!=NULL)
			{
				int iLen=_tcslen(s);
				int i,j;
				i=0;
				while(i<iLen&&s[i]!=TEXT('"'))
					i++;
				if(i==iLen)
					continue;
				i++;
				j=i;
				while(i<iLen&&s[i]!=TEXT('"'))
					i++;
				if(i==iLen)
					continue;
				_tcsncpy(cName,s+j,i-j);
				cName[i-j]=TEXT('\0');
				i++;
				while(i<iLen&&s[i]!=TEXT('"'))
					i++;
				if(i==iLen)
					continue;
				i++;
				j=i;
				while(i<iLen&&s[i]!=TEXT('"'))
					i++;
				if(i==iLen)
					continue;
				_tcsncpy(cServer,s+j,i-j);
				cServer[i-j]=TEXT('\0');
#ifdef SERVER_CHECK
				if(ctemp==cName || (bServerLooksOK && bInvalidUpdateServer))
#else
				if(ctemp == cName)
#endif
				{
					i++;
					while(i<iLen&&s[i]!=TEXT('"'))
						i++;
					if(i!=iLen)
					{
						i++;
						j=i;
						while(i<iLen&&s[i]!=TEXT('"'))
							i++;
						if(i!=iLen)
						{
							_tcsncpy(cPackServer,s+j,i-j);
							cPackServer[i-j]=TEXT('\0');
						}
					}
					bInvalidUpdateServer=false;
					fnServer=cServer;
					fnPackServer=cPackServer;
					fnLauncherServerName=fnServer+TEXT("launcher/");
					fnElementServerName=fnServer+TEXT("element/");
					fnElementPatchName=fnServer+TEXT("patches/");
					fnPatcherServerName=fnServer+TEXT("patcher/");
		
					if(!bUpdatedURL)
					{
						TCHAR st[400];
						if(fnServer[0]==TEXT(' '))
						{
							bQuickUpdate=true;
							_stprintf(st,TEXT("Name=SetURL,Item=UpdateBrowser,ServerType=0,URL=%s"),fnServer.Mid(1)+TEXT("info/index.html"));
							Command(pList,st);
						}
						else
						{
							_stprintf(st,TEXT("Name=SetURL,Item=UpdateBrowser,ServerType=1,URL=%s"),fnInfoURL);
							Command(pList,st);
						}
						bUpdatedURL=true;
					}
				}
				else if(fnLocalUpdatePack == "")
				{
					SpeedTestIndexInfo* info = new SpeedTestIndexInfo;
					info->index = index;
					info->pStr = (cServer[0] == TEXT(' ') ? cServer + 1 : cServer);
					info->used = false;
					ToDel.push_back(info);
					EnterCriticalSection(&ServerConnectingTimeMapSection);
					ServerConnectingTime.push_back(make_pair(cName,-1));
					LeaveCriticalSection(&ServerConnectingTimeMapSection);
					AfxBeginThread(ConnectionSpeedTest,info);
					index++;
				}
			}
			fclose(f);
		}
		delete xOption;
	}

	fnPIDServer=fnServer+TEXT("info/pid");
	fnLauncherServerPath=fnLauncherServerName+TEXT("launcher/");
	fnElementServerPath=fnElementServerName+TEXT("element/");
	ThreadUnLock();
	LeaveCriticalSection(&ServerSection);
}


CString cElog="";

void WriteElementLog(LPCTSTR c,BOOL reWrite)
{
	FILE *fLog;
	if(reWrite)
	{
		int size=GetFileSize(fnElementLogFile);
		if(size==-1||size>=MaxLogFileSize)
		{
			for(int i=NumbersOfLogFile;i>0;i--)
			{
				TCHAR t1[10];
				TCHAR t2[10];
				_stprintf(t1,TEXT("%03d"),i);
				_stprintf(t2,TEXT("%03d"),i-1);
				if(i!=1)
					CopyFile(fnElementLogName+t2,fnElementLogName+t1,FALSE);
				else
					CopyFile(fnElementLogFile,fnElementLogName+t1,FALSE);
			}
			UDeleteFile(fnElementLogFile);
			ThreadLock();
			if((fLog=OpenFile(fnElementLogFile,TEXT("wb")))!=NULL)
			{
				_fputtc(0xfeff,fLog);
				fclose(fLog);
			}
			ThreadUnLock();
		}
	}
	if(c==NULL)
	{
		ThreadLock();
		if((fLog=OpenFile(fnElementLogFile,TEXT("rb")))==NULL)
		{
			if((fLog=OpenFile(fnElementLogFile,TEXT("wb")))!=NULL)
			{
				_fputtc(0xfeff,fLog);
				fclose(fLog);
			}
		}
		else
			fclose(fLog);
		if((fLog=OpenFile(fnElementLogFile,TEXT("ab")))!=NULL)
		{
			SYSTEMTIME sTime;
			GetLocalTime(&sTime);
			TCHAR s[400];
			_stprintf(s,TEXT("%d/%d/%d %02d:%02d:%02d"),
					sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
			_fputts(s,fLog);
			_fputts(cElog,fLog);
			_fputts(TEXT("\r\n"),fLog);
			fclose(fLog);
		}
		ThreadUnLock();
		cElog="";
	}
	else
		cElog+=c;
}

CString cLlog="";

void WriteLauncherLog(LPCTSTR c,BOOL reWrite=FALSE)
{
	FILE *fLog;
	if(reWrite)
	{
		int size=GetFileSize(fnLauncherLogFile);
		if(size==-1||size>=MaxLogFileSize)
		{
			for(int i=NumbersOfLogFile;i>0;i--)
			{
				TCHAR t1[10];
				TCHAR t2[10];
				_stprintf(t1,TEXT("%03d"),i);
				_stprintf(t2,TEXT("%03d"),i-1);
				if(i!=1)
					CopyFile(fnLauncherLogName+t2,fnLauncherLogName+t1,FALSE);
				else
					CopyFile(fnLauncherLogFile,fnLauncherLogName+t1,FALSE);
			}
			UDeleteFile(fnLauncherLogFile);
			ThreadLock();
			if((fLog=OpenFile(fnLauncherLogFile,TEXT("wb")))!=NULL)
			{
				_fputtc(0xfeff,fLog);
				fclose(fLog);
			}
			ThreadUnLock();
		}
	}
	if(c==NULL)
	{
		ThreadLock();
		if((fLog=OpenFile(fnLauncherLogFile,TEXT("rb")))==NULL)
		{
			if((fLog=OpenFile(fnLauncherLogFile,TEXT("wb")))!=NULL)
			{
				_fputtc(0xfeff,fLog);
				fclose(fLog);
			}
		}
		else
			fclose(fLog);
		if((fLog=OpenFile(fnLauncherLogFile,TEXT("ab")))!=NULL)
		{
			SYSTEMTIME sTime;
			GetLocalTime(&sTime);
			TCHAR s[400];
			_stprintf(s,TEXT("%d/%d/%d %02d:%02d:%02d"),
					sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
			_fputts(s,fLog);
			_fputts(cLlog,fLog);
			_fputts(TEXT("\r\n"),fLog);
			fclose(fLog);
		}
		ThreadUnLock();
		cLlog="";
	}
	else
		cLlog+=c;
}

typedef int(* callback_k)(int status, int progress, const wchar_t *result);
typedef DWORD (WINAPI *asyncscan)(callback_k pCallbackFunc);

int CallbackFunc(int status,int progress,wchar_t* result)
{
	TCHAR st[200];
	switch(status)
	{
	case 0:
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[164]);
		break;
	case 1:
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[164]);
		_stprintf(st,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
			strTable[162]+result,strTable[161]);
		Command(pList,st);
		break;
	case 2:
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[163]);
		break;
	default:
		break;
	}
	_stprintf(st,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=%d"),progress);
	Command(pList,st);
	return 0;
}

BOOL bAntiTrojan=FALSE;
void AntiTrojan()
{
	if(fnLocalUpdatePack!="")
		return;
	if(bAntiTrojan)
		return;
	bAntiTrojan=TRUE;
	HMODULE hDll;
	hDll = LoadLibrary(_TEXT("../launcher/AntiTrojan.dll"));
	if(hDll == 0)
		return;
	asyncscan MyAsyncScan = (asyncscan)GetProcAddress(hDll,"AsyncScan");
	if(MyAsyncScan != 0)
		MyAsyncScan((callback_k)CallbackFunc);
}

bool CheckDiskFreeSpace(bool bUpdate)
{
	if( !bCheckDisk || bSilentUpdate)
		return true;
	
	DWORD dwSectorPerCluster, dwBytesPerSector, dwFreeClusters, dwTotalClusters;
	if( GetDiskFreeSpace(NULL, &dwSectorPerCluster, &dwBytesPerSector, 
			&dwFreeClusters, &dwTotalClusters))
	{
		if( ((UINT64)dwSectorPerCluster) * dwBytesPerSector * dwFreeClusters < ((UINT64)40) * 1024 * 1024 )
		{
			WriteElementLog(strTable[50]);
			WriteElementLog(NULL);
			Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
			Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[52]);
			if(bUpdate)
				Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[53]);
			else
				Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[54]);
			Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[55]);
			Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[56]);
			Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[57]);
			nSelect=0;
			cThread->SuspendThread();
			if(nSelect==-1)
			{
				if(bUpdate)
					Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[58]);
				else
					Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[59]);
				Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
				AntiTrojan();
				Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
				Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
				Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
				
				// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
				Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

				return false;
			}
			else
				bCheckDisk=false;
		}
	}
	return true;
}

BOOL CalFileMd5(LPCTSTR lName,char *md5)
{
	FILE *f;
	PATCH::MD5Hash m;
	char buf[FileSizeOnDraw+1];
	int iRead;
	UINT iSize=GetFileSize(lName);
	if((f=OpenFile(lName,TEXT("rb")))!=NULL)
	{
		TCHAR st[200];
		UINT iReadSize=0;
		if (UpdateState == Updater::STATE_SEP_UPDATE)
		{
			Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));
		}
		do
		{
			iRead=fread(buf,sizeof(char),FileSizeOnDraw,f);
			m.Update(buf,iRead);
			iReadSize+=iRead;
			if (UpdateState == Updater::STATE_SEP_UPDATE)
			{
				UINT progressPos = (UINT)((double)iReadSize/iSize*100);
				if (progressPos % 33 == 0)
				{
					_stprintf(st,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=%d"),progressPos);
					Command(pList,st);
				}
			}
		}while(iRead==FileSizeOnDraw);
		fclose(f);
		if (UpdateState == Updater::STATE_SEP_UPDATE)
		{
			Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=100"));
		}
	}
	UINT i=50;
	m.Final();
	m.GetString(md5,i);
	return TRUE;
}

BOOL CalMemMd5(unsigned char *buf,int size,char *md5)
{
	if (UpdateState == Updater::STATE_SEP_UPDATE)
	{
		Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));
	}
	PATCH::MD5Hash m;
	int pos=0;
	TCHAR st[200];
	while(pos+FileSizeOnDraw<size)
	{
		m.Update((char*)buf+pos,FileSizeOnDraw);
		pos+=FileSizeOnDraw;
		if (UpdateState == Updater::STATE_SEP_UPDATE)
		{
			UINT progressPos = (UINT)((double)pos/size*100);
			if (progressPos % 33 == 0)
			{
				_stprintf(st,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=%d"),progressPos);
				Command(pList,st);
			}
		}
	}
	if(pos<size)
		m.Update((char*)buf+pos,size-pos);
	m.Final();
	UINT i=50;
	m.GetString(md5,i);
	if (UpdateState == Updater::STATE_SEP_UPDATE)
	{
		Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=100"));
	}
	return TRUE;
}

void MakeDir(LPCTSTR dir,int r)
{
	r--;
	while(r>0&&dir[r]!=TEXT('/')&&dir[r]!=TEXT('\\'))
		r--;
	if(r==0)
		return;
	MakeDir(dir,r);
	TCHAR t[400];
	_tcscpy(t,dir);
	t[r]=TEXT('\0');
	_tmkdir(t);
}

void DeleteDir(LPCTSTR dir,int r)
{
	r--;
	while(r>0&&dir[r]!=TEXT('/')&&dir[r]!=TEXT('\\'))
		r--;
	if(r==0)
		return;
	TCHAR t[400];
	_tcscpy(t,dir);
	t[r]=TEXT('\0');
	_trmdir(t);
	DeleteDir(dir,r);
}

char fp[350];

void TransferPath(LPSTR srcName)
{
	char fn1[350];
	if(srcName[0]=='/')
	{
		strcpy(fn1+1,srcName);
		fn1[0]='.';
		strcpy(srcName,fn1);
		int i=strlen(srcName)-1;
		while(srcName[i]!='/')
			i--;
		strncpy(fp,srcName,i+1);
		fp[i+1]='\0';
	}
	else
	{
		strcpy(fn1,srcName);
		int j=strlen(fp);
		strcpy(srcName,fp);
		strcpy(srcName+j,fn1);
	}
}

BOOL TransferName(LPSTR srcName,LPTSTR desName)
{
	TransferPath(srcName);
	int sLen,i,j,k,l;
	char s[400];
	wchar_t ucode[400];
	char p[400];
	sLen=strlen(srcName);
	j=0;k=0;
	for(i=0;i<sLen;i++)
		if(!base64.IsCode(srcName[i]))
		{
			if(i!=j)
			{
				strncpy(s,srcName+j,i-j);
				s[i-j]='\0';
				base64.Decode(s,p);
				l=MultiByteToWideChar(CP_UTF8,0,p,-1,ucode,400);
				if(l>0)
				{
					l--;
#ifdef _UNICODE
					_tcscpy(desName+k,ucode);
					k+=l;
#else
					int o;
					o=WideCharToMultiByte(CodePage,0,ucode,-1,p,400,NULL,NULL);
					if(o>0)
					{
						o--;
						_tcscpy(desName+k,p);
						k+=o;
					}
#endif
				}
			}
			desName[k]=srcName[i];
			k++;
			j=i+1;
		}
	TCHAR st[200];
	if(i!=j)
	{
		strncpy(s,srcName+j,i-j);
		s[i-j]='\0';
		base64.Decode(s,p);
		l=MultiByteToWideChar(CP_UTF8,0,p,-1,ucode,400);
		if(l>0)
		{
			l--;
#ifdef _UNICODE
			_tcscpy(desName+k,ucode);
			k+=l;
			if (UpdateState == Updater::STATE_SEP_UPDATE)
			{
				_stprintf(st,TEXT("Name=SetText,Item=LoadFileName,Text=%s"),ucode);
				Command(pList,st);
			}
#else
			int o;
			o=WideCharToMultiByte(CodePage,0,ucode,-1,p,400,NULL,NULL);
			if(o>0)
			{
				o--;
				_tcscpy(desName+k,p);
				k+=o;
				_stprintf(st,TEXT("Name=SetText,Item=LoadFileName,Text=%s"),p);
				Command(pList,st);
			}
#endif
		}
	}
	desName[k]=TEXT('\0');
	return TRUE;
}

//=====================================FullCheck=============================================
int FullCheckCallback(int status,PATCH::Downloader* worker)
{
	TCHAR st[200];
	int errcode;
	int iPos = 0;
	switch(status)
	{
	case PATCH::DOWNLOAD_START:
		iFileSize=worker->GetSize();
		Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));
		bEqual=FALSE;
		break;
	case PATCH::BLOCK_DONE:
		iFinishedOffset=worker->GetOffset();
		iPos = (int)((double)iFinishedOffset/iFileSize*100);
		if (iPos % 25 == 0)
		{
			_stprintf(st,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=%d"),iPos);
			Command(pList,st);
		}
		break;
	case PATCH::FILE_DONE:
		Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=100"));
//		md5hash=worker->GetHash();
//		char md[50];
//		unsigned int i;
//		WriteElementLog(TEXT("    md5="));
//		md5hash.GetString(md,i);
//		WriteElementLog(Character2WChar(md));
//		WriteElementLog(NULL);
		bFileDown=true;
		break;
	case PATCH::NET_ERROR:
		bFileDown=false;
		errcode=worker->GetError();
		_stprintf(st,TEXT("%d"),errcode);
		WriteElementLog(strTable[2]);
		WriteElementLog(st);
		WriteElementLog(NULL);
		break;
	case PATCH::RET_ERROR:
		bFileDown=false;
		int err=worker->GetError();
		switch(err)
		{
		case PATCH::ERR_URL_PARSE:
			WriteElementLog(strTable[3]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_GET_ADDR:
			WriteElementLog(strTable[4]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_OVERFLOW:
			WriteElementLog(strTable[5]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_CREATE_FILE:
			WriteElementLog(strTable[6]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_REQ_DENIED:
			WriteElementLog(strTable[7]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_CONNECTION:
			WriteElementLog(strTable[8]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_UNKNOWN:
			WriteElementLog(strTable[9]);
			WriteElementLog(NULL);
			break;
		default:
			WriteElementLog(strTable[10]);
			WriteElementLog(NULL);
			break;
		}
		break;
	}
	return 0;
}

int CheckNewVersion()
{
	MakeDir(fnElementNewVersion,_tcsclen(fnElementNewVersion));
	FILE *fStream;
	UDeleteFile(fnElementNewVersion);
	EnterCriticalSection(&ServerSection);
	DownloadFile(fnElementServerName+TEXT("version"),fnElementNewVersion,NULL,0);
	LeaveCriticalSection(&ServerSection);
	if((fStream=OpenFile(fnElementNewVersion,TEXT("r")))!=NULL)
	{
		fscanf(fStream,"%d %d",&iNewVersion,&iNewVersion1);
		fclose(fStream);
		UDeleteFile(fnElementNewVersion);
	}
	else
		return -1;
	if(iNewVersion==0)
		return 1;
	return 0;
}

void Update360()
{
	if (bIs360NeedUpdate)
	{
		bIs360NeedUpdate = false;
		Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
		Command(pList,TEXT("Name=SetText,Item=Tip1,Text="));
		Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[161]);
		Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[90]);

		Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[56]);
		Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[57]);
		nSelect=0;
		cThread->SuspendThread();

		if (nSelect == 1)
		{
			TCHAR path[1000];
			GetCurrentDirectory(1000, path);
			CString parentPath(path);
			int iPos = parentPath.ReverseFind(TEXT('\\'));
			parentPath = parentPath.Left(iPos);
			parentPath += TEXT("\\element\\");
			parentPath += fn360InstallerName;
			int ret = (int)::ShellExecute(NULL,TEXT("open"),parentPath,NULL,TEXT("..\\element"),SW_SHOWNORMAL);
		}
	}
}
int FileFullCheck()
{
	int iTotalLine=0;
	int iLineNumber=0;
	FILE *fStream;
	char s[400];
	char fn[350];
	TCHAR path[400];
	TCHAR szText[300];
	UDeleteFile(fnUpdateState);

	WriteElementLog(strTable[22]);
	WriteElementLog(NULL);
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[23]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
	UDeleteFile(fnFullCheckList);
	EnterCriticalSection(&ServerSection);
	DownloadFile(fnElementServerName+TEXT("files.md5"),fnFullCheckList,NULL,0);
	LeaveCriticalSection(&ServerSection);
	if((fStream=OpenFile(fnFullListVersion,TEXT("w")))!=NULL)
	{
		fprintf(fStream,"%d %d\n",iNewVersion,iNewVersion1);
		fclose(fStream);
	}
	
	if(!PATCH::Signature::GetInstance()->Verify(fnFullCheckList))
		return 0;
	if((fStream=OpenFile(fnFullCheckList,TEXT("r")))==NULL)
		return 0;
	while(fgets(s,400,fStream)!=NULL)
	{
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		iTotalLine++;
	}
	if(iTotalLine==0)
	{
		fclose(fStream);
		iOldVersion=iNewVersion;
		return 1;
	}
	fseek(fStream,0,SEEK_SET);
	MakeDir(fnPath,_tcsclen(fnPath));
	Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
	WriteElementLog(strTable[28]);
	WriteElementLog(NULL);
	int iPos = 0;
	int iLastPos = 0;
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[13]);
	do
	{
		if(fgets(s,400,fStream)==NULL)
			break;
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		sscanf(s,"%s %s",md5,fn);
		TransferName(fn,path);
		WriteElementLog(strTable[14]);
		WriteElementLog(path);
		WriteElementLog(NULL);

//		WriteElementLog(TEXT("目标md5="));
//		WriteElementLog(Character2WChar(md5));
//		WriteElementLog(NULL);
		DWORD size;
		unsigned char *fbuf=NULL;
		ThreadLock();
		bool bInPack=IsFileInPack(path);
		char md[50];
		if(!bInPack)
		{
			MakeDir(fnPath+path,_tcsclen(fnPath+path));
			CalcCompressFile(fnPath+path,size,fbuf);
		}
		else
			GetFileFromPack(path,size,fbuf);
		ThreadUnLock();

			_stprintf(szText, TEXT("Name=SetText,Item=LoadFileName,Text=%s"), path);
			Command(pList,szText);

		if(fbuf!=NULL)
		{
			CalMemMd5(fbuf,size,md);
			ReleasePackBuf(fbuf);
			if(strcmp(md,md5)==0)
				bEqual=TRUE;
			else
				bEqual=FALSE;
		}
		else
			bEqual=FALSE;
		int i=0;
		if(!bEqual)
		{
			WriteElementLog(strTable[15]);
			WriteElementLog(NULL);
		}
		iFinishedOffset=0;
		DWORD starttime=::GetTickCount();
		DWORD lasttime=starttime;
		DWORD lastselecttime=starttime;
		DWORD nowtime=starttime;
		while(!bEqual)
		{
			if(!bEqual&&i>=1)
			{
				do{
					Sleep(200);
					ThreadUnLock();
					nowtime=::GetTickCount();
				}while(DWORD(nowtime-lasttime)<=ReconnectTime);
				lasttime=nowtime;
				if(DWORD(nowtime-lastselecttime)>=TotalTimeout)
				{
					int LastVersion=iNewVersion;
					int ret=CheckNewVersion();
					if(ret==1)
					{
						fclose(fStream);
						return 2;
					}
					if(ret==0 && iNewVersion!=LastVersion)
					{
						fclose(fStream);
						Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[60]);
						Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[61]);
						WriteElementLog(strTable[60]);
						WriteElementLog(strTable[61]);
						WriteElementLog(NULL);
						Sleep(1000);
						return FileFullCheck();
					}
					else
						iNewVersion=LastVersion;
					Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
					Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[62]);
					Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[63]);
					Command(pList,TEXT("Name=SetText,Item=Tip3,Text="));
					Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[64]);
					Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[65]);
					nSelect=bSilentUpdate ? 1 : 0;
					TCHAR st[200];
					do{
						Sleep(100);
						ThreadUnLock();
						if(nSelect==-1)
							return 0;
						if(nSelect==1)
							break;
						nowtime=::GetTickCount();
						_stprintf(st,TEXT("Name=SetText,Item=Tip3,Text=(")+strTable[66]+TEXT(")"),10-(nowtime-lasttime)/1000);
						Command(pList,st);
					}while(DWORD(nowtime-lasttime)<=MaxWaitTime);
					nSelect=0;
					lastselecttime=nowtime;
					Command(pList,TEXT("Name=CloseWindow,Window=Choose"));
				}
			}
			i++;
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[34]);
			Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));
			WriteElementLog(strTable[33]);
			WriteElementLog(path);
			WriteElementLog(NULL);
			bEqual=FALSE;
			bFileDown=false;
			if( !CheckDiskFreeSpace(false) )
				return -1;
			EnterCriticalSection(&ServerSection);
			DownloadFile(fnElementServerPath+fn,DownloadTempFile,(callback_t)FullCheckCallback,iFinishedOffset);
			LeaveCriticalSection(&ServerSection);
			if(bFileDown)
			{
				Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[13]);
				CalFileMd5(DownloadTempFile,md);
				if(strcmp(md,md5)==0)
					bEqual=TRUE;
				else
					bEqual=FALSE;
				iFinishedOffset=0;
			}
			else
				bEqual=false;
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[34]);
			if(!bEqual)
			{
				WriteElementLog(strTable[15]);
				WriteElementLog(NULL);
			}
			ThreadLock();
			if(bEqual)
				if(bInPack)
					AddFileToPack(path,DownloadTempFile);
				else
				{
					if(!DecompressFile(DownloadTempFile,fnPath+path))
					{
						WriteElementLog(strTable[18]);
						WriteElementLog(NULL);
						bEqual=FALSE;
					}
					UDeleteFile(DownloadTempFile);
				}

			ThreadUnLock();
//			return 0;
//			DownloadFile(fnElementServerPath+fn,fnPath+path,(callback_t)FullCheckCallback,0);
		}
		iLineNumber++;
		TCHAR st[200];
		iPos = iLineNumber*100/iTotalLine;
		if (iPos != iLastPos && (iPos % 5 == 0 || bSilentUpdate))
		{
			iLastPos = iPos;
			_stprintf(st,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=%d"),iPos);
			Command(pList,st);
		}
	}while(TRUE);
	fclose(fStream);
	if((fStream=OpenFile(fnElementOldVersion,TEXT("w")))!=NULL)
	{
		_ftprintf(fStream,TEXT("%d %d\n"),iNewVersion,iNewVersion1);
		fclose(fStream);
		FillPackVersion(fnElementOldVersion);
	}
	iOldVersion=iNewVersion;

	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text= "));
	SetUpdateState(1);
	return 1;
}
void ServerConnectionFailed()
{
	if (!bTurningFormPackUpdate)
	{
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[26]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[71]);
		Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
		AntiTrojan();
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

		bCanPlay=true;
		bAutoClose = false;
	}
	if (bSilentUpdate) {
		Command(pList,TEXT("Name=ServerConnectFailure"));
	}
	WriteElementLog(strTable[26]);
	WriteElementLog(NULL);
}
void ServerMaintenance()
{
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[25]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[73]);
	Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
	AntiTrojan();
	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
	Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
	// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
	Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));
	if (bSilentUpdate) {
		Command(pList,TEXT("Name=ServerMaintaining"));
	}
	
	bCanPlay=true;
	bAutoClose = false;
	WriteElementLog(strTable[25]);
	WriteElementLog(NULL);
}
void LocalVersionError()
{
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[99]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[96]);
	Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[97]);
	AntiTrojan();
	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
	Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
	// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
	Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));
	if (bSilentUpdate) {
		Command(pList,TEXT("Name=VersionError"));
	}
	bCanPlay=true;
	bAutoClose = false;
	WriteElementLog(strTable[99]);
	WriteElementLog(NULL);
}
UINT FullCheck(LPVOID pParam)
{
	if( !CheckDiskFreeSpace(false) )
		return 0;

	ThreadUnLock();
	if (!bSilentUpdate) {
		Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
		Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[67]);
		Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[68]);
		Command(pList,TEXT("Name=SetText,Item=Tip3,Text="));
		Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[56]);
		Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[57]);
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=false"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=false"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=false"));

		nSelect=0;
		cThread->SuspendThread();
		if(nSelect==-1)
		{
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[69]);
			Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
			AntiTrojan();
			Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
			// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

			return 0;
		}
	}
	WriteElementLog(TEXT("开始版本验证..."));
	WriteElementLog(NULL);
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[70]);
	MakeDir(fnPIDFile,_tcslen(fnPIDFile));
	EnterCriticalSection(&ServerSection);
	DownloadFile(fnPIDServer,fnPIDFile,NULL,0);
	LeaveCriticalSection(&ServerSection);
	FILE *fStream;
	int iNewPID=0;
	if((fStream=OpenFile(fnPIDFile,TEXT("r")))==NULL)
	{
		ServerConnectionFailed();
		return 0;
	}

	fscanf(fStream,"%d",&iNewPID);
	fclose(fStream);
	UDeleteFile(fnPIDFile);
	if(iNewPID==0)
	{
		ServerMaintenance();
		return 0;
	}
	if(iNewPID!=pid)
	{
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[27]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[74]);
		Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
		AntiTrojan();
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));
		if (bSilentUpdate) {
			Command(pList,TEXT("Name=ServerRegionError"));
		}
		bCanPlay=true;
		return 0;
	}
	BOOL b=FALSE;
	iNewVersion=0;
	iOldVersion=0;
	iNewVersion1=0;
	iOldVersion1=0;
	int ret=CheckNewVersion();
	SaveUpdateServer();
	switch(ret)
	{
	case 1:
		ServerMaintenance();
		return 0;
	case -1:
		ServerConnectionFailed();
		return 0;
	}
	if(bSelectServer && !bSilentUpdate)
	{
		Command(pList,TEXT("Name=ShowWindow,Window=UpdateSetting"));
		bWaitingServer=true;
		nSelect=0;
		cThread->SuspendThread();
		if(nSelect==-1)
		{
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[69]);
			Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
			Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
			AntiTrojan();
			Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
			// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

			bCanPlay=true;
			return 0;
		}
	}
	ThreadLock();
	PackInitialize(true);
	ThreadUnLock();
	ret=FileFullCheck();
	CheckPack();

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[151]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[113]);
	ThreadLock();
	PackFinalize();
	ThreadUnLock();
	TCHAR st[400];
	switch(ret)
	{
	case -1:
		break;
	case 0:
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[76]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
		Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
		AntiTrojan();
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

		WriteElementLog(strTable[59]);
		WriteElementLog(NULL);
		break;
	case 2:
		Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
		ServerMaintenance();
		break;
	default:
		_stprintf(st,TEXT("Name=SetText,Item=ElementVersion,Text=")+strTable[77],iOldVersion);
		Command(pList,st);
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[30]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
		Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
		Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		AntiTrojan();
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

		bCanPlay=true;
		WriteElementLog(strTable[30]);
		WriteElementLog(NULL);
		if (bSilentUpdate)
			Command(pList, TEXT("Name=SilentUpdateSucess"));
	}
	
	return 0;
}

//=======================================Update===============================================

int iLastOffset;

int UpdateCallback(int status,PATCH::Downloader* worker)
{
	TCHAR st[200];
	int errcode;
	int iPos = 0;
	switch(status)
	{
	case PATCH::DOWNLOAD_START:
		iFileSize=worker->GetSize();
		Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));
		bEqual=FALSE;
		iLastOffset=0;
		break;
	case PATCH::BLOCK_DONE:
		iFinishedOffset=worker->GetOffset();
		iPos = (int)((double)iFinishedOffset/iFileSize*100);
		if (iPos % 25 == 0)
		{
			_stprintf(st,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=%d"),iPos);
			Command(pList,st);
		}
		Updater::ShowDownloadSpeed(iFinishedOffset);
		break;
	case PATCH::FILE_DONE:
		Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=100"));
		bFileDown=true;
		break;
	case PATCH::NET_ERROR:
		bFileDown=false;
		errcode=worker->GetError();
		_stprintf(st,TEXT("%d"),errcode);
		WriteElementLog(strTable[2]);
		WriteElementLog(st);
		WriteElementLog(NULL);
		break;
	case PATCH::RET_ERROR:
		bFileDown=false;
		int err=worker->GetError();
		switch(err)
		{
		case PATCH::ERR_URL_PARSE:
			WriteElementLog(strTable[3]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_GET_ADDR:
			WriteElementLog(strTable[4]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_OVERFLOW:
			WriteElementLog(strTable[5]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_CREATE_FILE:
			WriteElementLog(strTable[6]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_REQ_DENIED:
			WriteElementLog(strTable[7]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_CONNECTION:
			WriteElementLog(strTable[8]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_UNKNOWN:
			WriteElementLog(strTable[9]);
			WriteElementLog(NULL);
			break;
		default:
			WriteElementLog(strTable[10]);
			WriteElementLog(NULL);
			break;
		}
		break;
	}
	return 0;
}
bool CheckLocal()
{
	FILE *fStream;
	if((fStream=OpenFile(fnElementOldVersion,TEXT("r")))==NULL)
		return false;

	fscanf(fStream,"%d %d",&iOldVersion,&iOldVersion1);
	fclose(fStream);
	if(iOldVersion<1)
		return false;
	return true;
}
// 仅在这里打开一次pck文件包以减少准备时间
bool CheckPackVersion(bool bFirstGet)
{
	bool ret = true;
	if(bFirstGet)
	{
		ThreadLock();
		if (PackInitialize())
		{
			int pverson = GetPackVersion();
			if(pverson < iOldVersion)
				iOldVersion = pverson;
			ret = true;
			//PackFinalize();
		}
		else
		{
			ret = false;
		}
		ThreadUnLock();
	}

	return ret;
}
UINT GetNewVersion(bool bFirstGet)
{
	if (!CheckLocal())
	{	
		return Updater::LOCAL_VERSION_ERROR;
	}
	if (!CheckPackVersion(bFirstGet))
	{
		return Updater::PACKVERSION_GETTING_FAILED;
	}
	MakeDir(fnElementNewVersion,_tcsclen(fnElementNewVersion));
	UDeleteFile(fnElementNewVersion);
	EnterCriticalSection(&ServerSection);
	DownloadFile(fnElementServerName+TEXT("version"),fnElementNewVersion,NULL,0);
	LeaveCriticalSection(&ServerSection);
	if((fStream=OpenFile(fnElementNewVersion,TEXT("r")))!=NULL)
	{
		fscanf(fStream,"%d %d",&iNewVersion,&iNewVersion1);
		fclose(fStream);
		UDeleteFile(fnElementNewVersion);
	}
	else
	{
		PackFinalize();
		return Updater::FAILED_DOWNLOADING_VERSION_FILE;
	}
	if(iNewVersion==0)
	{
		PackFinalize();
		return Updater::SERVER_MAINTENANCE;
	}
	int p=iNewVersion-iOldVersion;
	if(p<0)
	{
		PackFinalize();
		return Updater::SERVER_VERSION_LOW;
	}
	if(p==0)
	{
		PackFinalize();
		return Updater::VERSION_EQUAL_WITH_SERVER;
	}

	return Updater::NEED_UPDATE;
}
void StopUpdate()
{
	if(fnLocalUpdatePack=="")
	{
		WriteElementLog(strTable[11]);
		WriteElementLog(NULL);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=UpdateFileListError"));
			return;
		}
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[106]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[107]);
		Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
		Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[157]);
		Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[158]);
		Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[159]);
		Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[152]);
		Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[160]);
		nSelect=0;
		cThread->SuspendThread();
	}
	else
	{
		TCHAR szText[200];
		_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
			strTable[134],strTable[131]);
		Command(pList,szText);
		PackFinalize();
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=UpdatePackError"));
			return;
		}
		if (UpdateState == Updater::STATE_SEP_UPDATE)
		{
			CallFunc(pList,TEXT("Name=Exit"));
		}
	}
}
void GetUpdateListFile()
{
	iUpdatedLine=0;
	iFinishedOffset=0;

	int l=iNewVersion-iOldVersion;
	int k;
	if(l<=10)
		k=l;
	else
	{
		int i=2;
		k=10;
		while(k<l)
		{
			k+=i;
			i++;
		}
	}

	if( k > l ){
		k = l;
	}

	TCHAR st[100];
	WriteElementLog(strTable[22]);
	WriteElementLog(NULL);
	_stprintf(st,fnElementServerName+TEXT("v-%d.inc"),k);
	if (UpdateState == Updater::STATE_SEP_UPDATE)
	{
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[22]);
	}
	EnterCriticalSection(&ServerSection);
	DownloadFile(st,fnUpdateList,NULL,0);
	LeaveCriticalSection(&ServerSection);
}

bool ResumeLastUpdate()
{
	BOOL bNoUpdateListFile;
	FILE* f;
	if(fnLocalUpdatePack!="")
	{
		UDeleteFile(fnUpdateState);
		UDeleteFile(fnUpdateList);
	}
	else
	{
		f=OpenFile(fnUpdateState,TEXT("r"));
		if(f!=NULL)
		{
			BOOL bUpdateStateError=FALSE;
			if(fscanf(f,"%d %d %d %d",
				&iUpdatingVersion,&iUpdatedLine,&iFinishedOffset,&iFileSize)==EOF)
			{
				bUpdateStateError=TRUE;
				WriteElementLog(strTable[78]);
				WriteElementLog(NULL);
				Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[78]);
				Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[79]);
				Sleep(100);
				iUpdatingVersion=0;iUpdatedLine=0;iFinishedOffset=0;iFileSize=0;
			}
			fclose(f);
			if(iUpdatingVersion!=iNewVersion)
				bUpdateStateError=TRUE;
			if(!bUpdateStateError&&!bAutoClose&&!bSilentUpdate)
			{
				Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
				Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[80]);
				Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[81]);
				Command(pList,TEXT("Name=SetText,Item=Tip3,Text="));
				Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[82]);
				Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[83]);
				nSelect=0;
				cThread->SuspendThread();
				if(nSelect==-1)
				{
					UDeleteFile(fnUpdateState);
					UDeleteFile(fnUpdateList);
					iUpdatingVersion=0;iUpdatedLine=0;iFinishedOffset=0;iFileSize=0;
				}
			}
			else if (bUpdateStateError)
			{
				UDeleteFile(fnUpdateState);
				UDeleteFile(fnUpdateList);
				iUpdatingVersion=0;iUpdatedLine=0;iFinishedOffset=0;iFileSize=0;
			}
		}
		if((f=OpenFile(fnUpdateList,TEXT("r")))==NULL)
			bNoUpdateListFile=TRUE;
		else
		{
			bNoUpdateListFile=FALSE;
			fclose(f);
		}
	}
	if(iUpdatingVersion!=iNewVersion||bNoUpdateListFile||fnLocalUpdatePack!="")
	{	
		GetUpdateListFile();
	}
	return true;
}
bool CheckDirectX()
{
	// 检查directx版本
	DWORD dwDXMajor = 0;
	DWORD dwDXMinor = 0;
	TCHAR cDXLetter = ' ';
	bool  bNeedUpdateDirectX = false;
	HRESULT ret = GetDXVersion( &dwDXMajor, &dwDXMinor, &cDXLetter );
	if (FAILED(ret))
	{
		WriteElementLog(TEXT("获取DirectX版本失败！！"));
		WriteElementLog(NULL);
	}
	//判断主版本是不是9.0c
	if( dwDXMajor < DX_VERSION_MAJOR ||
		( dwDXMajor == DX_VERSION_MAJOR && dwDXMinor < DX_VERSION_MINOR ) ||
		( dwDXMajor == DX_VERSION_MAJOR && dwDXMinor == DX_VERSION_MINOR && cDXLetter < DX_VERSION_LETTER ) )
	{
		bNeedUpdateDirectX = true;
	}
	else
	{
		TCHAR pathSys32[MAX_PATH];
		GetSystemDirectory(pathSys32,MAX_PATH);
		_tcscat(pathSys32,TEXT("\\d3dx9_42.dll"));

		if (!PathFileExists(pathSys32))
		{
			bNeedUpdateDirectX = true;
		}	
	}

	if (bNeedUpdateDirectX)
	{
		TCHAR path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, path);
		CString parentPath(path);
		int iPos = parentPath.ReverseFind(TEXT('\\'));
		parentPath = parentPath.Left(iPos);
		parentPath += TEXT("\\element\\");
		parentPath += fnDXWebInstaller;

		TCHAR strlog[200];

		_stprintf(strlog,TEXT("当前DirectX版本： %d.%d%c"),dwDXMajor,dwDXMinor,cDXLetter);
		WriteElementLog(strlog);
		WriteElementLog(NULL);
		if (PathFileExists(parentPath))
		{
			Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
			Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[162]);
			Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[163]);
			Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[90]);

			Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[56]);
			Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[57]);
			nSelect=0;
			cThread->SuspendThread();

			if (nSelect == 1)
			{
				int ret = (int)::ShellExecute(NULL,TEXT("open"),parentPath,NULL,TEXT("..\\element"),SW_SHOWNORMAL);
			}
		}
		else
		{
			Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
			Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[162]);
			Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[163]);
			Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[164]);
			Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[152]);
			Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[160]);
			nSelect=0;
			cThread->SuspendThread();
		}
	}
	return true;
}
bool FinishUpdate()
{
	if (bSilentUpdate)
		Command(pList, TEXT("Name=SilentUpdateSucess"));
	else
		Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
	if(fnLocalUpdatePack!="")
	{
		TCHAR szText[200];
		_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
			strTable[145],strTable[130]);
		Command(pList,szText);
		if (bSilentUpdate)
			return true;

		if (UpdateState == Updater::STATE_SEP_UPDATE)
		{
			CallFunc(pList,TEXT("Name=Exit"));
		}
	}
	ExePerUpdate();
	if (bAutoClose)
	{
		if (bSilentUpdate)
			return true;

		CallFunc(pList,TEXT("Name=Exit"));
	}

	//FlagUpdateNonThread();

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[117]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
	Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
	if(fnLocalUpdatePack=="")
	{
		AntiTrojan();
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

	}
	bCanPlay=true;
	WriteElementLog(TEXT("更新完成"));
	WriteElementLog(NULL);
	if (ExeBeforClose())
	{
		if (bSilentUpdate)
			return true;
		CallFunc(pList,TEXT("Name=Exit"));
	}
//	CheckDirectX();

	return true;
}
void PackOpenFailed()
{
	if (bSilentUpdate) {
		Command(pList, TEXT("Name=LocalPackError"));
		return;
	}
	if (fnLocalUpdatePack == "")
	{
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[95]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[96]);
		Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[97]);
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=false"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=false"));

		bCanPlay=false;
		bAutoClose = false;
		ThreadUnLock();
	}
	else
	{
		ThreadUnLock();
		TCHAR szText[200];
		_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
			strTable[138],strTable[131]);
		Command(pList,szText);
		if (UpdateState == Updater::STATE_SEP_UPDATE)
		{
			CallFunc(pList,TEXT("Name=Exit"));
		}
	}
}
bool DealWithVersionError(int ve)
{
	if (!bSepFileUpdate)
	{
		return true;
	}
	TCHAR szText[200];
	if(ve == Updater::FAILED_DOWNLOADING_VERSION_FILE)
	{
		ServerConnectionFailed();
		return false;
	}
	else if (ve == Updater::SERVER_MAINTENANCE)
	{
		if(fnLocalUpdatePack=="")
		{
			ServerMaintenance();
		}
		else
		{	
			if (bSilentUpdate) {
				Command(pList,TEXT("Name=PackVersionError"));
				return false;
			}
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[140],strTable[131]);
			Command(pList,szText);
			if (UpdateState == Updater::STATE_SEP_UPDATE)
			{
				CallFunc(pList,TEXT("Name=Exit"));
			}
		}
	}
	else if (ve == Updater::LOCAL_VERSION_ERROR)
	{
		_stprintf(szText,TEXT("Name=SetText,Item=ElementVersion,Text=")+strTable[98]);
		bAutoClose = false;
		Command(pList,szText);
		if (fnLocalUpdatePack=="")
		{
			LocalVersionError();
		}
		else
		{
			if (bSilentUpdate) {
				Command(pList,TEXT("Name=PackVersionError"));
				return false;
			}
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[142],strTable[131]);
			Command(pList,szText);
			if (UpdateState == Updater::STATE_SEP_UPDATE)
			{		
				CallFunc(pList,TEXT("Name=Exit"));
			}
		}
	}
	else if (ve == Updater::SERVER_VERSION_LOW)
	{
		if (fnLocalUpdatePack=="")
		{
			ServerMaintenance();
		}
		else
		{
			if (bSilentUpdate) {
				Command(pList,TEXT("Name=PackVersionError"));
				return false;
			}
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[141],strTable[131]);
			Command(pList,szText);
			if (UpdateState == Updater::STATE_SEP_UPDATE)
			{
				CallFunc(pList,TEXT("Name=Exit"));
			}

		}
	}
	else if (ve == Updater::VERSION_STOP_UPDATE)
	{
		StopUpdate();
	}
	else if (ve == Updater::PACKVERSION_GETTING_FAILED)
	{
		PackOpenFailed();
		return false;
	}
	return true;
}
void PackListError()
{
	WriteElementLog(strTable[168]);
	WriteElementLog(NULL);
	if (bSilentUpdate) {
		Command(pList, TEXT("Name=UpdateFileListError"));
	}
	//Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[168]);
	//Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[169]);
	//Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
	//Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[12]);
	//Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[97]);
	//Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[97]);
	//Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[152]);
	//Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[160]);
	//nSelect=0;
	//cThread->SuspendThread();
}
void MD5ListError()
{
	WriteElementLog(strTable[148]);
	WriteElementLog(NULL);
	if (bSilentUpdate) {
		Command(pList, TEXT("Name=UpdateFileListError"));
	}
	//Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[148]);
	//Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[169]);
	//Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
	//Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[148]);
	//Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[97]);
	//Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[97]);
	//Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[152]);
	//Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[160]);
	//nSelect=0;
	//cThread->SuspendThread();
}
void PackDownloadError()
{
	if (bSilentUpdate) {
		Command(pList, TEXT("Name=ServerConnectFailure"));
		return;
	}
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[62]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[169]);
	Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
	Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[62]);
	Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[97]);
	Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[97]);
	Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[152]);
	Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[160]);
	nSelect=0;
	cThread->SuspendThread();
}
void DealWithUpdateError(int iError)
{
	TCHAR szText[200];
	if (iError == Updater::PACK_OPEN_FALIED)
	{
		WriteElementLog(strTable[139]);
		WriteElementLog(NULL);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=UpdatePackError"));
		}
		//Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[169]);
		//Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[131]);
		//_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
		//	strTable[139],strTable[131]);
		//Command(pList,szText);
	}
	else if (iError == Updater::PACK_CLIENT_NOT_FIT)
	{
		WriteElementLog(strTable[149]);
		WriteElementLog(NULL);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=UpdatePackError"));
		}
		//Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[169]);
		//Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[131]);
		//_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
		//	strTable[149],strTable[131]);
		//Command(pList,szText);
	}
	else if (iError == Updater::PACK_VERSION_ERROR)
	{
		WriteElementLog(strTable[147]);
		WriteElementLog(NULL);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=PackVersionError"));
		}
		//_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
		//	strTable[147],strTable[131]);
		//Command(pList,szText);
	}
	else if (iError == Updater::PACK_UPDATELIST_ERROR || iError == Updater::PACK_UPDATELIST_VERIFY_FAILED)
	{
		WriteElementLog(strTable[11]);
		WriteElementLog(NULL);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=UpdateFileListError"));
		}
		//StopUpdate();
	}
	else if (iError == Updater::PACK_VERSION_TOO_HIGH)
	{
		WriteElementLog(strTable[135]);
		WriteElementLog(NULL);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=PackVersionError"));
		}
		//Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[169]);
		//Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[131]);
		//_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
		//	strTable[135],strTable[131]);
		//Command(pList,szText);
	}
	else if (iError == Updater::PACK_DISK_NOT_ENOUGH_SPACE)
	{
		WriteElementLog(strTable[115]);
		WriteElementLog(NULL);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=LocalPackError"));
		}
		//Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[115]);
		//Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[131]);
		//_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
		//	strTable[50],strTable[131]);
		//Command(pList,szText);
	}
	else if (iError == Updater::PACK_FILE_CORRUPTED)
	{
		WriteElementLog(strTable[136]);
		WriteElementLog(NULL);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=LocalPackError"));
		}
		//Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[136]);
		//Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[131]);
		//_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
		//	strTable[136],strTable[131]);
		//Command(pList,szText);
	}
}
// 
// packname:更新包的文件名；numpacks：一共有多少更新包；packnum：现在更新到第几个更新包，从0开始
// 返回0表示更新成功
int UpdateFromPack(const char* packname,int numpacks,int packnum)
{
	fnLocalUpdatePack = packname;
	iUpdatedLine=0;
	iTotalLine=0;
	iLineNumber=0;
	FILE *fStream(NULL);
	char s[400];
	char fn[350];
	TCHAR path[400];
	bool bFileEqual = false;
	ASSERT(packnum < numpacks);
	// 打开离线包并查看pid
	TCHAR st[300];
	_tcscpy(st,fnLocalUpdatePack);
	if(!UseLocalUpdatePack(st))
	{
		return Updater::PACK_OPEN_FALIED;
	}
	if(GetLocalUpdateFile(fnPackPIDFile,fnPIDFile))
	{
		FILE *fStream=OpenFile(fnPIDFile,TEXT("r"));
		int iNewPID=0;
		if(fStream!=NULL)
		{
			fscanf(fStream,"%d",&iNewPID);
			fclose(fStream);
			UDeleteFile(fnPIDFile);
			if(iNewPID!=pid)
			{
				return Updater::PACK_CLIENT_NOT_FIT;
			}
		}
	}
	else
	{
		return Updater::PACK_OPEN_FALIED;
	}

	int versionstat = GetNewVersion(false);
	if (versionstat == Updater::NEED_UPDATE)
	{
		// 读取updatelist
		GetUpdateListFile();
		if((fStream=OpenFile(fnUpdateList,TEXT("r")))==NULL)
		{
			return Updater::PACK_UPDATELIST_ERROR;
		}
		// 验证updatelist
		if(!PATCH::Signature::GetInstance()->Verify(fnUpdateList))
		{
			fclose(fStream);
			return Updater::PACK_UPDATELIST_VERIFY_FAILED;
		}
		// 验证version
		fgets(s,400,fStream);
		int versionFrom,versionTo;
		char o[30];
		sscanf(s,"%s%d%d",o,&versionFrom,&versionTo);
		if(versionTo!=iNewVersion)
		{
			fclose(fStream);
			return Updater::PACK_UPDATELIST_ERROR;
		}
		if(versionFrom>iOldVersion)
		{
			fclose(fStream);
			PackFinalize();
			return Updater::PACK_VERSION_TOO_HIGH;
		}
		fseek(fStream,0,SEEK_SET);
		// 计算列表内有多少文件
		while(fgets(s,400,fStream)!=NULL)
		{
			if(s[0]=='#')
				continue;
			if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
				break;
			iTotalLine++;
		}

		if (iTotalLine > 0)
		{
			fseek(fStream,0,SEEK_SET);
			MakeDir(fnPath,_tcsclen(fnPath));

			int iProgressStart = packnum * 100 / numpacks;
			int iProgressEnd = (packnum + 1) * 100 / numpacks;
			int iProgressLast = iProgressStart;
			float fProgressStep = (float)(iProgressEnd - iProgressStart) / (float)iTotalLine;
			_stprintf(st,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=%d"),iProgressLast);
			Command(pList,st);
			WriteElementLog(strTable[88]+fnLocalUpdatePack);
			WriteElementLog(NULL);
			char md[50];
			TCHAR tempName[MAX_PATH];
			do
			{
				if(fgets(s,400,fStream)==NULL)
					break;
				if(s[0]=='#')
					continue;
				if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
					break;
				sscanf(s,"%s %s",md5,fn);
				TransferName(fn,path);

				_tcscpy(tempName,path);
				_tcslwr(tempName);
				CheckExeUpdate(tempName);
				if(IsArcNorthAmerica)				
					CheckArcNorthAmericaInstaller(tempName);


				bool bInPack=IsFileInPack(path);
				if(md5[0]=='-')
				{
					WriteElementLog(strTable[36]);
					WriteElementLog(path);
					WriteElementLog(NULL);
					ThreadLock();
					if(bInPack)
						RemoveFileFromPack(path);
					else
						UDeleteFile(fnPath+path);
					ThreadUnLock();
				}
				else
				{
					bFileEqual=FALSE;
					unsigned char *fbuf=NULL;
					DWORD size;
					ThreadLock();
					if(bInPack)
						GetFileFromPack(path,size,fbuf);
					else
					{
						MakeDir(fnPath+path,_tcsclen(fnPath+path));
						CalcCompressFile(fnPath+path,size,fbuf);
					}
					ThreadUnLock();
					if(fbuf!=NULL)
					{
						CalMemMd5(fbuf,size,md);
						ReleasePackBuf(fbuf);
						if(strcmp(md,md5+1)==0)
							bFileEqual=TRUE;
						else
							bFileEqual=FALSE;
					}
					else
						bFileEqual=FALSE;
					if(bFileEqual)
					{
						WriteElementLog(strTable[31]);
						WriteElementLog(path);
						WriteElementLog(NULL);
					}

					while(!bFileEqual)
					{
						WriteElementLog(strTable[51]);
						WriteElementLog(path);
						WriteElementLog(NULL);
						bFileEqual=FALSE;
						if( !CheckDiskFreeSpace(true) )
							return Updater::PACK_DISK_NOT_ENOUGH_SPACE;
						DownloadFile(fnElementServerPath+fn,DownloadTempFile,NULL,0);
						CalFileMd5(DownloadTempFile,md);
						if(strcmp(md,md5+1)==0)
							bFileEqual=TRUE;
						else
							bFileEqual=FALSE;

						if(!bFileEqual)
						{
							WriteElementLog(strTable[15]);
							WriteElementLog(NULL);
						}
						else
						{
							ThreadLock();
							if(bInPack)
								AddFileToPack(path,DownloadTempFile);
							else
							{
								if(!DecompressFile(DownloadTempFile,fnPath+path))
								{
									WriteElementLog(strTable[18]);
									WriteElementLog(NULL);
									bFileEqual=FALSE;
								}
								UDeleteFile(DownloadTempFile);
							}
							ThreadUnLock();
						}
						ThreadUnLock();
						if(!bFileEqual)
						{
							PackFinalize();
							return Updater::PACK_FILE_CORRUPTED;
						}
					}
				}
				iLineNumber++;
				int iProgressNow = iProgressStart + iLineNumber * fProgressStep;
				if (iProgressNow > iProgressLast)
				{
					iProgressLast = iProgressNow;
					_stprintf(st,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=%d"),iProgressNow);
					Command(pList,st);
				}
			}while(TRUE);
		}
	}
	else if(versionstat == Updater::VERSION_EQUAL_WITH_SERVER)
	{
		FinishUpdate();
	}
	else
	{
		DealWithVersionError(versionstat);
		return Updater::PACK_VERSION_ERROR;
	}

	fclose(fStream);
	if((fStream=OpenFile(fnElementOldVersion,TEXT("w")))!=NULL)
	{
		_ftprintf(fStream,TEXT("%d %d\n"),iNewVersion,iNewVersion1);
		fclose(fStream);
		FillPackVersion(fnElementOldVersion);
	}
	iOldVersion=iNewVersion;
	UDeleteFile(fnUpdateList);

	fnLocalUpdatePack = "";
	return 0;
}

void CheckExeUpdate(TCHAR* UpdateFileName)
{
	for(int i = 0;i < Exes.size();i++)
	{
		if (!Exes[i].ExecuteByCheckVersion && !Exes[i].Updated && _tcsstr(UpdateFileName,Exes[i].FileName))
		{
			Exes[i].Updated = true;
		}
	}
}
int UpdateFile()
{
	iUpdatedLine=0;
	iFinishedOffset=0;
	iTotalLine=0;
	iLineNumber=0;
	FILE *fStream(NULL);
	char s[400];
	char fn[350];
	TCHAR path[400];

	ResumeLastUpdate();

	if((fStream=OpenFile(fnUpdateList,TEXT("r")))==NULL)
	{
		StopUpdate();
		return 0;
	}
	if(!PATCH::Signature::GetInstance()->Verify(fnUpdateList))
	{
		fclose(fStream);
		if(fnLocalUpdatePack=="")
		{
			WriteElementLog(strTable[11]);
			WriteElementLog(NULL);
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[12]);
			Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[73]);
			if (bSilentUpdate) {
				Command(pList, TEXT("Name=UpdateFileListError"));
			}
		}
		else
		{
			TCHAR szText[200];
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[134],strTable[131]);
			Command(pList,szText);
			PackFinalize();
			if (bSilentUpdate) {
				Command(pList, TEXT("Name=UpdatePackError"));
				return 0;
			}
			CallFunc(pList,TEXT("Name=Exit"));
		}
		return 0;
	}
	fgets(s,400,fStream);
	int versionFrom,versionTo;
	char o[30];
	sscanf(s,"%s%d%d",o,&versionFrom,&versionTo);
	if(versionTo!=iNewVersion)
	{
		fclose(fStream);
		StopUpdate();
		return 0;
	}
	if(versionFrom>iOldVersion)
	{
		fclose(fStream);
		if(fnLocalUpdatePack=="")
		{
			WriteElementLog(strTable[11]);
			WriteElementLog(NULL);
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[12]);
			Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[73]);
			if (bSilentUpdate) {
				Command(pList, TEXT("Name=UpdateFileListError"));
			}
		}
		else
		{
			TCHAR szText[200];
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[135],strTable[131]);
			Command(pList,szText);
			PackFinalize();
			if (bSilentUpdate) {
				Command(pList, TEXT("Name=UpdatePackError"));
				return 0;
			}
			CallFunc(pList,TEXT("Name=Exit"));
		}
		return 0;
	}
	fseek(fStream,0,SEEK_SET);
	while(fgets(s,400,fStream)!=NULL)
	{
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		iTotalLine++;
	}
	if(iTotalLine>0)
	{
		fseek(fStream,0,SEEK_SET);
		while(iLineNumber<iUpdatedLine)
		{
			if(fgets(s,400,fStream)==NULL)
			{
				WriteElementLog(strTable[11]);
				WriteElementLog(NULL);
				Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[12]);
				Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[73]);
				if (bSilentUpdate) {
					Command(pList, TEXT("Name=UpdatePackError"));
				}
				return 0;
			}
			if(s[0]=='#')
				continue;
			if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
				break;
			iLineNumber++;
			sscanf(s,"%s %s",md5,fn);
			TransferPath(fn);
		}

		bUpdating=true;
		SetUpdateState(0);
		MakeDir(fnPath,_tcsclen(fnPath));
		TCHAR st[200];
		_stprintf(st,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=%d"),iLineNumber*100/iTotalLine);
		Command(pList,st);
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[34]);
		WriteElementLog(strTable[84]);
		WriteElementLog(NULL);
		char md[50];
		int iPos = 0;
		TCHAR tempName[MAX_PATH];
		do
		{
			if(fgets(s,400,fStream)==NULL)
				break;
			if(s[0]=='#')
				continue;
			if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
				break;
			sscanf(s,"%s %s",md5,fn);
			TransferName(fn,path);

			_tcscpy(tempName,path);
			_tcslwr(tempName);
			CheckExeUpdate(tempName);
			if (IsArcNorthAmerica)
				CheckArcNorthAmericaInstaller(tempName);

			bool bInPack=IsFileInPack(path);
			if(md5[0]=='-')
			{
				WriteElementLog(strTable[36]);
				WriteElementLog(path);
				WriteElementLog(NULL);
				ThreadLock();
				if(bInPack)
					RemoveFileFromPack(path);
				else
					UDeleteFile(fnPath+path);
				ThreadUnLock();
			}
			else
			{
				bEqual=FALSE;
				unsigned char *fbuf=NULL;
				DWORD size;
				ThreadLock();
				if(bInPack)
					GetFileFromPack(path,size,fbuf);
				else
				{
					MakeDir(fnPath+path,_tcsclen(fnPath+path));
					CalcCompressFile(fnPath+path,size,fbuf);
				}
				ThreadUnLock();
				if(fbuf!=NULL)
				{
					CalMemMd5(fbuf,size,md);
					ReleasePackBuf(fbuf);
					if(strcmp(md,md5+1)==0)
						bEqual=TRUE;
					else
						bEqual=FALSE;
				}
				else
					bEqual=FALSE;
				if(bEqual)
				{
					iFinishedOffset=0;
					WriteElementLog(strTable[31]);
					WriteElementLog(path);
					WriteElementLog(NULL);
				}
				if(iFinishedOffset!=0)
					bResume=TRUE;
				else
					bResume=FALSE;
				int i=0;
				DWORD starttime=::GetTickCount();
				DWORD lasttime=starttime;
				DWORD lastselecttime=starttime;
				DWORD nowtime=starttime;
				while(!bEqual)
				{
					i++;
					if(bResume)
						WriteElementLog(strTable[32]);
					else
						WriteElementLog(strTable[33]);
					WriteElementLog(path);
					WriteElementLog(NULL);
				//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[34]);
				//	Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));
					bEqual=FALSE;
					bFileDown=false;
					if( !CheckDiskFreeSpace(true) )
						return -1;
					EnterCriticalSection(&ServerSection);
					DownloadFile(fnElementServerPath+fn,DownloadTempFile,(callback_t)UpdateCallback,iFinishedOffset);
					LeaveCriticalSection(&ServerSection);
					if(bFileDown)
					{
						iFinishedOffset=0;
					//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[13]);
						CalFileMd5(DownloadTempFile,md);
						if(strcmp(md,md5+1)==0)
							bEqual=TRUE;
						else
							bEqual=FALSE;
					//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[34]);

						if(!bEqual)
						{
							WriteElementLog(strTable[15]);
							WriteElementLog(NULL);
						}
						else
						{
							ThreadLock();
							if(bInPack)
								AddFileToPack(path,DownloadTempFile);
							else
							{
								if(!DecompressFile(DownloadTempFile,fnPath+path))
								{
									WriteElementLog(strTable[18]);
									WriteElementLog(NULL);
									bEqual=FALSE;
								}
								UDeleteFile(DownloadTempFile);
							}
							ThreadUnLock();
						}
					}
					else
						bEqual=false;
					ThreadUnLock();
					if(!bEqual)
					{
						if(fnLocalUpdatePack!="")
						{
							TCHAR szText[200];
							_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
								strTable[136],strTable[131]);
							Command(pList,szText);
							PackFinalize();
							if (bSilentUpdate) {
								Command(pList, TEXT("Name=UpdatePackError"));
								return 0;
							}
							CallFunc(pList,TEXT("Name=Exit"));
						}
						do{
							Sleep(200);
							ThreadUnLock();
							nowtime=::GetTickCount();
						}while(DWORD(nowtime-lasttime)<=ReconnectTime);
						lasttime=nowtime;
						if(DWORD(nowtime-lastselecttime)>=TotalTimeout)
						{
							int LastVersion=iNewVersion;
							int ret=GetNewVersion(false);
							if(ret==Updater::SERVER_MAINTENANCE)
							{
								fclose(fStream);
								return 2;
							}
							if(ret==Updater::NEED_UPDATE && iNewVersion!=LastVersion)
							{
								fclose(fStream);
								Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[60]);
								Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[61]);
								WriteElementLog(strTable[60]);
								WriteElementLog(strTable[61]);
								WriteElementLog(NULL);
								Sleep(1000);
								bUpdating=false;
								return UpdateFile();
							}
							else
								iNewVersion=LastVersion;

							Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
							Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[62]);
							Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[63]);
							Command(pList,TEXT("Name=SetText,Item=Tip3,Text="));
							Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[64]);
							Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[65]);
							nSelect=bSilentUpdate ? 1 : 0;
							TCHAR st[200];
							do{
								Sleep(100);
								ThreadUnLock();
								if(nSelect==-1)
									return 0;
								if(nSelect==1)
									break;
								nowtime=::GetTickCount();
								_stprintf(st,TEXT("Name=SetText,Item=Tip3,Text=(")+strTable[66]+TEXT(")"),10-(nowtime-lasttime)/1000);
								Command(pList,st);
							}while(DWORD(nowtime-lasttime)<=MaxWaitTime);
							nSelect=0;
							lastselecttime=nowtime;
							Command(pList,TEXT("Name=CloseWindow,Window=Choose"));
						}
					}
				}
			}
			iLineNumber++;iFinishedOffset=0;
			TCHAR st[200];
			iPos = (int)((double)iLineNumber/iTotalLine*100);
			if (iPos % 20 == 0 || bSilentUpdate)
			{
				_stprintf(st,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=%d"),iPos);
				Command(pList,st);
			}
		}while(TRUE);
	}
	fclose(fStream);
	if((fStream=OpenFile(fnElementOldVersion,TEXT("w")))!=NULL)
	{
		_ftprintf(fStream,TEXT("%d %d\n"),iNewVersion,iNewVersion1);
		fclose(fStream);
		FillPackVersion(fnElementOldVersion);
	}
	iOldVersion=iNewVersion;

	UDeleteFile(fnUpdateState);
	UDeleteFile(fnUpdateList);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text= "));
	SetUpdateState(1);
	return 1;
}

int LauncherCallback(int status,PATCH::Downloader* worker)
{
	TCHAR st[200];
	int errcode;
	switch(status)
	{
	case PATCH::DOWNLOAD_START:
		iFileSize=worker->GetSize();
		Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));
		bEqual=FALSE;
		break;
	case PATCH::BLOCK_DONE:
		iFinishedOffset=worker->GetOffset();
		_stprintf(st,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=%d"),(int)((double)iFinishedOffset/iFileSize*100));
		Command(pList,st);
		break;
	case PATCH::FILE_DONE:
		Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=100"));
//		md5hash=worker->GetHash();
//		bEqual=md5hash.Equal(md5);
		break;
	case PATCH::NET_ERROR:
		bFileDown=false;
		errcode=worker->GetError();
		_stprintf(st,TEXT("%d"),errcode);
		WriteElementLog(strTable[2]);
		WriteElementLog(st);
		WriteElementLog(NULL);
		break;
	case PATCH::RET_ERROR:
		int err=worker->GetError();
		switch(err)
		{
		case PATCH::ERR_URL_PARSE:
			WriteElementLog(strTable[3]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_GET_ADDR:
			WriteElementLog(strTable[4]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_OVERFLOW:
			WriteElementLog(strTable[5]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_CREATE_FILE:
			WriteElementLog(strTable[6]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_REQ_DENIED:
			WriteElementLog(strTable[7]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_CONNECTION:
			WriteElementLog(strTable[8]);
			WriteElementLog(NULL);
			break;
		case PATCH::ERR_UNKNOWN:
			WriteElementLog(strTable[9]);
			WriteElementLog(NULL);
			break;
		default:
			WriteElementLog(strTable[10]);
			WriteElementLog(NULL);
			break;
		}
		break;
	}
	return 0;
}

int CheckPatcherVersion()
{
	FILE *fStream(NULL);
	int iNewVersion=0;
	int iOldVersion;
	EnterCriticalSection(&ServerSection);
	DownloadFile(fnPatcherServerName+TEXT("version"),fnPatcherNewVersion,NULL,0,bSilentUpdate);
	LeaveCriticalSection(&ServerSection);
	if((fStream=OpenFile(fnPatcherNewVersion,TEXT("r")))!=NULL)
	{
		fscanf(fStream,"%d",&iNewVersion);
		fclose(fStream);
		DeleteFile(fnElementNewVersion);
	}
	if(iNewVersion==0)
		return 1;
	if((fStream=OpenFile(fnPatcherOldVersion,TEXT("r")))==NULL)
		return 0;
	else
	{
		fscanf(fStream,"%d",&iOldVersion);
		fclose(fStream);
		if(iOldVersion<1)
			return 0;
	}
	int p=iNewVersion-iOldVersion;
	if(p>0)
		return 2;
	if(p<0)
		return 0;
	return 1;
}

BOOL FullCheckLauncher()
{
	iLineNumber=0;
	iTotalLine=0;
	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT("PerfectWorldLauncherExist"));
	if (GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		HWND hWnd=FindWindow(TEXT("PerfectWorldLauncherClass"),TEXT("Launcher"));
		if(hWnd!=NULL)
		{
			WriteLauncherLog(strTable[85]);
			WriteLauncherLog(NULL);
			::SendMessage(hWnd,WM_CLOSE,NULL,NULL);
			Sleep(1000);
			if(FindWindow(TEXT("PerfectWorldLauncherClass"),TEXT("Launcher"))!=NULL)
			{
				WriteLauncherLog(strTable[86]);
				WriteLauncherLog(NULL);
				return FALSE;
			}
		}
	}
	else
		CloseHandle(hMutex);

	MakeDir(fnLauncherList,_tcsclen(fnLauncherList));
	WriteLauncherLog(strTable[22]);
	WriteLauncherLog(NULL);
	EnterCriticalSection(&ServerSection);
	DownloadFile(fnLauncherServerName+TEXT("files.md5"),fnLauncherList,NULL,0,bSilentUpdate);
	LeaveCriticalSection(&ServerSection);
	if(!PATCH::Signature::GetInstance()->Verify(fnLauncherList))
	{
		WriteLauncherLog(strTable[12]);
		WriteLauncherLog(NULL);
		return FALSE;
	}
	FILE *fStream(NULL);
	char fn[350];
	char s[400];
	TCHAR path[300];
	int iTotalFile=0;
	if((fStream=OpenFile(fnLauncherList,TEXT("r")))==NULL)
		return FALSE;
	while(fgets(s,400,fStream)!=NULL)
		iTotalLine++;
	BOOL *b=new BOOL[iTotalLine];
	fseek(fStream,0,SEEK_SET);
	MakeDir(fnLauncherCheckPath,_tcsclen(fnLauncherCheckPath));
	if (!bSilentUpdate)
		Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[13]);
	WriteLauncherLog(strTable[28]);
	WriteLauncherLog(NULL);
	do
	{
		if(fgets(s,400,fStream)==NULL)
			break;
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		sscanf(s,"%s %s",md5,fn);
		TransferName(fn,path);
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[13]);
		WriteLauncherLog(strTable[14]);
		WriteLauncherLog(path);
		WriteLauncherLog(NULL);
		char md[50];
		DWORD size;
		unsigned char *fbuf=NULL;
		ThreadLock();
		MakeDir(fnLauncherPath+path,_tcsclen(fnLauncherPath+path));
		CalcCompressFile(fnLauncherPath+path,size,fbuf);
		ThreadUnLock();
		if(fbuf!=NULL)
		{
			CalMemMd5(fbuf,size,md);
			ReleasePackBuf(fbuf);
			if(strcmp(md,md5)==0)
				bEqual=TRUE;
			else
				bEqual=FALSE;
		}
		else
			bEqual=FALSE;
		int i=0;
		b[iLineNumber]=FALSE;
		DWORD starttime=::GetTickCount();
		DWORD lasttime=starttime;
		DWORD nowtime=starttime;
		while(!bEqual)
		{
			WriteLauncherLog(strTable[15]);
			WriteLauncherLog(NULL);
			if(!bEqual&&i>=1)
			{
				do{
					Sleep(200);
					ThreadUnLock();
					nowtime=::GetTickCount();
				}while(DWORD(nowtime-lasttime)<=ReconnectTime2);
				lasttime=nowtime;
				if(DWORD(nowtime-starttime)>=TotalTimeout2)
				{
					delete b;
					return FALSE;
				}
			}
			i++;
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[16]);
			WriteLauncherLog(strTable[17]);
			WriteLauncherLog(path);
			WriteLauncherLog(NULL);
			MakeDir(fnLauncherCheckPath+path,_tcsclen(fnLauncherCheckPath+path));
			bFileDown=false;
			EnterCriticalSection(&ServerSection);
			DownloadFile(fnLauncherServerPath+fn,LauncherTempFile,(callback_t)FullCheckCallback,0,bSilentUpdate);
			LeaveCriticalSection(&ServerSection);
			if(bFileDown)
			{
				ThreadLock();
				CalFileMd5(LauncherTempFile,md);
				if(strcmp(md,md5)==0)
				{
					bEqual=TRUE;
					if(!DecompressFile(LauncherTempFile,fnLauncherCheckPath+path))
					{
						WriteLauncherLog(strTable[18]);
						WriteLauncherLog(NULL);
						bEqual=FALSE;
					}
					UDeleteFile(LauncherTempFile);
				}
				else
					bEqual=FALSE;
				ThreadUnLock();
			}
			else
				bEqual=FALSE;
			b[iLineNumber]=TRUE;
		}
		if(b[iLineNumber])
			iTotalFile++;
		iLineNumber++;
		if (!bSilentUpdate)
			Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
	}while(TRUE);
	if(iTotalFile>0)
	{
		fseek(fStream,0,SEEK_SET);
		WriteLauncherLog(strTable[19]);
		WriteLauncherLog(NULL);
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[20]);
		if (!bSilentUpdate)
			Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
		int i=0;
		iLineNumber=0;
		do
		{
			if(fgets(s,400,fStream)==NULL)
				break;
			if(s[0]=='#')
				continue;
			if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
				break;
			sscanf(s,"%s %s",md5,fn);
			TransferName(fn,path);

			if(b[iLineNumber])
			{
				WriteLauncherLog(strTable[21]);
				WriteLauncherLog(path);
				WriteLauncherLog(NULL);
				MakeDir(fnLauncherPath+path,_tcsclen(fnLauncherPath+path));
				UCopyFile(fnLauncherCheckPath+path,fnLauncherPath+path,FALSE);
				UDeleteFile(fnLauncherCheckPath+path);
				DeleteDir(fnLauncherCheckPath+path,_tcsclen(fnLauncherCheckPath+path));
				i++;
				TCHAR st[200];
				_stprintf(st,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=%d"),i*100/iTotalFile);
				if (!bSilentUpdate)
					Command(pList,st);
			}
			iLineNumber++;
		}while(TRUE);
	}
	fclose(fStream);
//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=验证完成"));
//	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
	WriteLauncherLog(strTable[30]);
	WriteLauncherLog(NULL);
	DeleteDir(fnLauncherCheckPath,_tcsclen(fnLauncherCheckPath));
	if (!bSilentUpdate)
		Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
	delete b;
	UDeleteFile(fnLauncherList);
	return TRUE;
}
int lastpos = 0;
bool CALLBACK DisplayProgress(int pos)
{
	TCHAR st[200];
	if (pos != lastpos && (pos == 0 || pos % 25 == 0 || bSilentUpdate))
	{
		lastpos = pos;
		_stprintf(st,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=%d"),pos);
		Command(pList,st);
	}

	return true;
}
#ifdef SERVER_CHECK

UINT ServerSwitch(LPVOID pParam)
{
	char th[100] = {0};
	char t[100] = "aHR0cDovL3cyaXBhdGNoLmludGVybmF0aW9uYWwud2FubWVpLmNvbS9DUFcvc3dpdGNoLw==";

	base64.Decode(t,th);
	CheckServer(th);

	if (!bDonotChangeServer)
	{
		GetServer();
		bServerSwitched = true;
	}
	return 0;

}
#endif

Updater* GetUpdater()
{
	if (bSepFileUpdate)
	{
		return SepFileUpdater::GetSingletonPtr();
	}
	if (fnLocalUpdatePack == "")
	{
		UpdateState = Updater::STATE_PACK_UPDATE;
		return PackUpdater::GetSingletonPtr();
	}
	else return SepFileUpdater::GetSingletonPtr();
}

bool CleanUp()
{
	ThreadUnLock();
	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=false"));
	Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=false"));
	// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
	Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=false"));
	if (!bSilentUpdate)
		Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
	ThreadUnLock();
	
	if(bCleanPack)
	{
		ThreadLock();
		if(PackInitialize())
		{
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=文件包碎片整理中..."));
			Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=请稍候"));
			Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
			ThreadLock();
			bool b=CleanUpPackFile(DisplayProgress,true);
			ThreadUnLock();
			Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
			if(b)
			{
				Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=文件包整理完成"));
				Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
			}
			else
			{
				Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=磁盘剩余空间不足"));
				Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=整理失败"));
			}
			bCleanPack=false;
			PackFinalize();
		}
		ThreadUnLock();
		AntiTrojan();
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));
		
		return false;
	}
	return true;
}
bool SelectServer()
{
	if(fnLocalUpdatePack=="")
	{
		if (bInvalidUpdateServer && bSilentUpdate) {
			Command(pList, TEXT("Name=InvalidUpdateServer"));
			return false;
		}
		while(bInvalidUpdateServer)
		{
			Command(pList,TEXT("Name=ShowWindow,Window=UpdateSetting"));
			bWaitingServer=true;
			cThread->SuspendThread();
			if(bInvalidUpdateServer)
			{
				TCHAR szText[200];
				_stprintf(szText,TEXT("Name=MessageBox,Window=UpdateSetting,Text=%s,Caption=%s"),
					strTable[137],strTable[132]);
				Command(pList,szText);
			}
		}
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[24]);
		WriteElementLog(strTable[24]);
		WriteElementLog(NULL);
	}
	//else
	//{
	//	ThreadLock();
	//	if(!PackInitialize())
	//	{
	//		ThreadUnLock();
	//		TCHAR szText[200];
	//		_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
	//			strTable[138],strTable[131]);
	//		Command(pList,szText);
	//		CallFunc(pList,TEXT("Name=Exit"));
	//	}
	//	else
	//		PackFinalize();
	//	ThreadUnLock();
	//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text="+strTable[87]));
	//	WriteElementLog(strTable[88]+fnLocalUpdatePack);
	//	WriteElementLog(NULL);
	//}
	return true;
}
bool UpdatePatcher()
{
	int i=CheckPatcherVersion();
	if(i!=1)
	{
		if (bAutoClose)
		{
			if (i == 2)
			{
				ShellExecute(NULL,TEXT("open"),GetFullPath(TEXT("../launcher/launcher.exe")),TEXT("/update /autoclose"),TEXT("../launcher"),SW_SHOWNORMAL);
			}
			else
			{
				ShellExecute(NULL,TEXT("open"),GetFullPath(TEXT("../launcher/launcher.exe")),TEXT("FullCheck /autoclose"),TEXT("../launcher"),SW_SHOWNORMAL);
			}
			CallFunc(pList,TEXT("Name=Exit"));
			
		}
		else
		{
			Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
			if(i==2)
			{
				Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[89]);
				Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[90]);
				Command(pList,TEXT("Name=SetText,Item=Tip3,Text="));
				Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[91]);
			}
			else
			{
				Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[38]);
				Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[94]);
				Command(pList,TEXT("Name=SetText,Item=Tip3,Text="));
				Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[93]);
			}
			Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[92]);
			Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
			Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=false"));
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=false"));
			// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=false"));
			
			nSelect=0;
			cThread->SuspendThread();
			if(nSelect==1)
			{
				Command(pList,TEXT("Name=ShowWindow,Window=UpdateSetting"));
				bWaitingServer=true;
				nSelect=0;
				cThread->SuspendThread();
				if(nSelect==1)
				{
					if(i==2)
						ShellExecute(NULL,TEXT("open"),GetFullPath(TEXT("../launcher/launcher.exe")),TEXT("/update"),TEXT("../launcher"),SW_SHOWNORMAL);
					else
						ShellExecute(NULL,TEXT("open"),GetFullPath(TEXT("../launcher/launcher.exe")),TEXT("FullCheck"),TEXT("../launcher"),SW_SHOWNORMAL);
					CallFunc(pList,TEXT("Name=Exit"));
				}
			}
		}
	}
	return true;
}
#ifdef RUSSIA_PATCH
void RunJadeLoader()
{
	TCHAR fn[300];
	GetModuleFileName(NULL,fn,300);
	int len=_tcslen(fn)-1;
	while(fn[len]!=TEXT('/')&&fn[len]!=TEXT('\\'))
		len--;
	fn[len]=TEXT('\0');
	SetCurrentDirectory(fn);

	TCHAR installpath[300], fnlauncher[300], fnjadeloader[300];
	_tcscpy(installpath, fn);
	int len1=_tcslen(installpath)-1;
	while(len1>0&&installpath[len1]!=TEXT('/')&&installpath[len1]!=TEXT('\\'))
		len1--;
	installpath[len1]=TEXT('\0');

	_stprintf(fnlauncher, TEXT("%s\\launcher\\launcher.exe"), installpath);
	_stprintf(fnjadeloader, TEXT("%s\\launcher\\-gup-\\pwloadern.exe"), installpath);
	FILE *ff;
	if( ff = _wfopen(fnjadeloader, TEXT("rb")) )
	{
		fclose(ff);
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		TCHAR szParam[1024];
		_stprintf(szParam, TEXT("-skipinstalldialogs \"nativeroot=%s\" \"nativelauncher=%s\""),installpath,fnlauncher);
		CreateProcess(fnjadeloader, // No module name (use command line). 
			szParam,		 // Command line. 
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			0,                // No creation flags. 
			NULL,             // Use parent's environment block. 
			NULL,		      // Use parent's starting directory. 
			&si,              // Pointer to STARTUPINFO structure.
			&pi );             // Pointer to PROCESS_INFORMATION structure.
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		ExitProcess(0);
	}

}
#endif
bool CheckServerStat()
{
	if(fnLocalUpdatePack!="")
	{
		TCHAR st[300];
		_tcscpy(st,fnLocalUpdatePack);
		if(!UseLocalUpdatePack(st))
		{
			TCHAR szText[200];
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[139],strTable[131]);
			Command(pList,szText);
			if (bSilentUpdate) {
				Command(pList,TEXT("Name=UpdatePackError"));
				return false;
			}
			CallFunc(pList,TEXT("Name=Exit"));
		}
		if(GetLocalUpdateFile(fnPackPIDFile,fnPIDFile))
		{
			FILE *fStream=OpenFile(fnPIDFile,TEXT("r"));
			int iNewPID=0;
			if(fStream!=NULL)
			{
				fscanf(fStream,"%d",&iNewPID);
				fclose(fStream);
				UDeleteFile(fnPIDFile);
				if(iNewPID!=pid)
				{
					TCHAR szText[200];
					_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
						strTable[149],strTable[131]);
					Command(pList,szText);
					if (bSilentUpdate) {
						Command(pList,TEXT("Name=ServerRegionError"));
						return false;
					}
					CallFunc(pList,TEXT("Name=Exit"));
				}
			}
		}
	}
	else
	{
		MakeDir(fnPIDFile,_tcslen(fnPIDFile));
		UDeleteFile(fnPIDFile);
		bool bQueryPerformanceFailed = false;
		LARGE_INTEGER liStart,liEnd;
		if (!QueryPerformanceCounter(&liStart))
		{
			bQueryPerformanceFailed = true;
		}
		EnterCriticalSection(&ServerSection);
		DownloadFile(fnPIDServer,fnPIDFile,NULL,0);
		LeaveCriticalSection(&ServerSection);
		if (!QueryPerformanceCounter(&liEnd))
		{
			bQueryPerformanceFailed = true;
		}
		if (!bQueryPerformanceFailed)
		{
			SelectedServerConnectingTime = liEnd.QuadPart - liStart.QuadPart;
		}
		FILE *fStream=OpenFile(fnPIDFile,TEXT("r"));
		int iNewPID=0;
		if(fStream!=NULL)
		{
			fscanf(fStream,"%d",&iNewPID);
			fclose(fStream);
			UDeleteFile(fnPIDFile);
			if(iNewPID==0)
			{
				if (!bSilentUpdate)
					Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
				ServerMaintenance();
				return false;
			}
		}
		else
		{
			ServerConnectionFailed();
			return false;
		}
		if(iNewPID!=pid)
		{
			if (bSilentUpdate) {
				Command(pList,TEXT("Name=ServerRegionError"));
				return false;
			}
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[27]);
			Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[74]);
			Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
			AntiTrojan();
			Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
			// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

			bCanPlay=true;
			//delete mgr;
			return false;
		}
		SaveUpdateServer();
		if(!bUpdated && !bSilentUpdate)
		{
			UpdatePatcher();
		}
		if (!bSilentUpdate)
			FullCheckLauncher();
#ifdef RUSSIA_PATCH
		RunJadeLoader();
#endif
	
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[150]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[113]);
		//ThreadLock();
		//if(!PackInitialize())
		//{
		//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[95]);
		//	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[96]);
		//	Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[97]);
		//	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
		//	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		//	Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=false"));
		//	// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		//	Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=false"));

		//	bCanPlay=false;
		//	bAutoClose = false;
		//	ThreadUnLock();
		//
		//	return false;
		//}
		//else
		//	PackFinalize();
		//ThreadUnLock();
	}
	return true;
}
#ifdef SERVER_CHECK
void ServerSwitchBegin()
{
	if (!bServerChecked)
	{
		bServerChecked = true;
		time_t ltime;
		time(&ltime);
		time_t tStart = mktime(&tmStartCheck);
		if (ltime >= tStart)
		{
			AfxBeginThread(ServerSwitch,NULL);
		}
		
	}
}
void ServerSwitchEnd()
{
	if (bServerLooksOK)
	{
		while(bServerLooksOK && !bServerSwitched)
		{
			Sleep(100); 
		}
	}
	if (!bServerLooksOK)
	{
		bDonotChangeServer = true;
	}
}
#endif

bool bServerListDownloaded = false;
UINT DownLoadServerList(LPVOID pParam)
{
	if (bServerListDownloaded) return 0;
	if (strTable[171].IsEmpty()) return 0;

	UDeleteFile(DownloadTempFile2);
	DownloadFile(strTable[171],DownloadTempFile2,NULL,0, bSilentUpdate);
	fStream=OpenFile(DownloadTempFile2,TEXT("r"));
	if (fStream)
	{
		fclose(fStream);
		UCopyFile(DownloadTempFile2,TEXT("./server/serverlist.txt"),FALSE);
		UDeleteFile(DownloadTempFile2);
		bServerListDownloaded = true;
		WriteElementLog(TEXT("serverlist.txt switched!"));
		WriteElementLog(NULL);
	}
	else
	{
		WriteElementLog(TEXT("download serverlist.txt failed!"));
		WriteElementLog(NULL);
	}
	return 0;
}

CString SuggestNewServer()
{
	CString ret(TEXT(""));
	int len = ServerConnectingTime.size();
	if (SelectedServerConnectingTime != -1)
	{
		for (int i = 0;i < len;++i)
		{
			if (ServerConnectingTime[i].second != -1 && 
				ServerConnectingTime[i].second < SelectedServerConnectingTime)
			{
				ret = ServerConnectingTime[i].first;
				break;
			}
		}
	}
	return ret;
}

bool SetUpdateServer()
{
	if (bSilentUpdate) return true;
	bool b = true;
	bool bSelectedServer=false;
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[100]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[101]);
	//若需要更新，在CheckPackVersion时就已经代开文件包了，这里不用再次打开
	//ThreadLock();
	//b=PackInitialize(false);
	//ThreadUnLock();
	//if(!b)
	//{
	//	PackFinalize();
	//	PackOpenFailed();
	//	return false;
	//}

	if(fnLocalUpdatePack==""&&!bAutoUpdating)
	{
		Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
		Command(pList,TEXT("Name=SetText,Item=Tip1,Text=")+strTable[102]);
		Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[103]);
		Command(pList,TEXT("Name=SetText,Item=Tip3,Text="));
		Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[104]);
		Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[105]);
		nSelect=0;
		cThread->SuspendThread();
		if(nSelect==-1)
		{
			PackFinalize();
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[106]);
			Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[107]);
			Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
			AntiTrojan();
			Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
			// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

			bCanPlay=true;
			return false;
		}
	}
	if(fnLocalUpdatePack==""&&bSelectServer&&!bSelectedServer)
	{
		CString SuggestServer = SuggestNewServer();
		if (SuggestServer.GetLength() > 0)
		{
			TCHAR szText[200];
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[170] + SuggestServer,TEXT(""));
			Command(pList,szText);
		}
		Command(pList,TEXT("Name=ShowWindow,Window=UpdateSetting"));
		bWaitingServer=true;
		nSelect=0;
		cThread->SuspendThread();
		if(nSelect==-1)
		{
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[108]);
			Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
			Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[97]);
			AntiTrojan();
			Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
			// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

			bCanPlay=false;
			WriteElementLog(strTable[58]);
			WriteElementLog(NULL);
			return false;
		}
	}
	return true;
}
bool CheckPack()
{
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[110]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[111]);
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[112]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[113]);
	Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
	ThreadLock();
	bool b=CleanUpPackFile(DisplayProgress);
	ThreadUnLock();
	Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
	if(b)
	{
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[114]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
	}
	else
	{
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[115]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[116]);
	}
	return true;
}
bool UpdateImpl()
{
	UDeleteFile(fnUpdateList);
	int uret=UpdateFile();
	bUpdating=false;

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[151]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[113]);
	ThreadLock();
	PackFinalize();
	ThreadUnLock();

	if(uret==-1)
		return false;
	else if(uret==0)
	{
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[109]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[73]);
		Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[97]);
		AntiTrojan();
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

		bCanPlay=false;
		WriteElementLog(strTable[58]);
		WriteElementLog(NULL);
		SaveUpdateState(iNewVersion,iLineNumber,iFinishedOffset,iFileSize);
		return false;
	}
	else if(uret==2)
	{
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[25]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[73]);
		Command(pList,TEXT("Name=SetText,Item=Play,Text=")+strTable[72]);
		AntiTrojan();
		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
		// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
		Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

		bCanPlay=true;
		WriteElementLog(TEXT("服务器维护中，更新中断"));
		WriteElementLog(NULL);
		SaveUpdateState(iNewVersion,iLineNumber,iFinishedOffset,iFileSize);
		if (bSilentUpdate) {
			Command(pList, TEXT("Name=ServerMaintaining"));
		}
		return false;
	}

	if(uret==1)
	{
		TCHAR st[400];
		_stprintf(st,TEXT("Name=SetText,Item=ElementVersion,Text=")+strTable[77],iNewVersion);
		Command(pList,st);
	}
	CheckPack();
	return true;
}

template<> SepFileUpdater* Singleton<SepFileUpdater>::ms_Singleton = 0;

bool SepFileUpdater::Update()
{
	// 散文件方式不显示下载速度
	Updater::SetDownloadParam(0,0,0);
	return UpdateImpl();
}

int SepFileUpdater::GetVersion(bool bFirstGet)
{
	return GetNewVersion(bFirstGet);
}
void TurnToSepFileUpdate()
{
	if (!bSepFileUpdate && fnLocalUpdatePack == "")
	{
		bSepFileUpdate = true;
		bTurningFormPackUpdate = true;
		WriteElementLog(TEXT("Turn to seperate-files-updating method"));
		WriteElementLog(NULL);
		BeginThread(Update, cThread);
	}
}

void DealWithArcNorthAmerica()
{
	if (Arc_State_NorthAmerica == NEED_INSTALL) {
		Command(pList, TEXT("Name=ShowWindow,Window=Choose"));
		Command(pList, TEXT("Name=SetText,Item=Tip1,Text=") + strTable[174]);
		Command(pList, TEXT("Name=SetText,Item=Tip2,Text=") + strTable[175]);
		Command(pList, TEXT("Name=SetText,Item=Tip3,Text="));
		Command(pList, TEXT("Name=SetText,Item=Yes,Text=") + strTable[176]);
		Command(pList, TEXT("Name=SetText,Item=No,Text=") + strTable[177]);
		nSelect = 0;
		cThread->SuspendThread();
		if(nSelect==1)
			ShellExecute(NULL, TEXT("open"), fnArcNorthAmericaInstaller, NULL, NULL, SW_SHOWNORMAL);
		CallFunc(pList,TEXT("Name=Exit"));
	}else if (Arc_State_NorthAmerica == NEED_DOWNLOAD) {
		Command(pList, TEXT("Name=ShowWindow,Window=Choose"));
		Command(pList, TEXT("Name=SetText,Item=Tip1,Text=") + strTable[178]);
		Command(pList, TEXT("Name=SetText,Item=Tip2,Text=") + strTable[179]);
		Command(pList, TEXT("Name=SetText,Item=Tip3,Text=") + strTable[180]);
		Command(pList, TEXT("Name=SetText,Item=Yes,Text=") + strTable[176]);
		Command(pList, TEXT("Name=SetText,Item=No,Text=") + strTable[177]);
		nSelect = 0;
		cThread->SuspendThread();
		if(nSelect==1)
			ShellExecute(NULL, TEXT("open"), strTable[180], NULL, NULL, SW_SHOWNORMAL);
		CallFunc(pList,TEXT("Name=Exit"));
	}
}
void CheckArcNorthAmericaInstaller(const TCHAR* strName)
{
	if (_tcsstr(strName, strTable[181])) {
		char szText[10];
		sprintf(szText, "%d", 1);
		WritePrivateProfileStringA("Patcher", "Installer", szText, "..\\element\\patcher.ini");
	}
}

UINT Update(LPVOID pParam)
{
	// 更新准备
	CWinThread* thread = AfxBeginThread(DownLoadServerList,NULL);
	if (bSilentUpdate) {
		WaitForSingleObject(thread->m_hThread, INFINITE);
		int i=CheckPatcherVersion();
		if(i!=1) {
			Command(pList,TEXT("Name=UpdatePatcher"));
			return 0;			
		}
		FullCheckLauncher();
	}
	else if (IsArcNorthAmerica)
		DealWithArcNorthAmerica();

#ifdef SERVER_CHECK
	ServerSwitchBegin();
#endif
	// 整理pck包
	if (!CleanUp())
	{
		return 0;
	}
	if( !CheckDiskFreeSpace(true) )
	{
		return 0;
	}	
	// 选择更新服务器
	if (!SelectServer())
		return 0;
	// 检查服务器pid
	if (!CheckServerStat())
	{
		return 0;
	}
#ifdef SERVER_CHECK
	ServerSwitchEnd();
#endif
	// 更新开始
	iNewVersion = 0;
	iOldVersion = 0;
	iNewVersion1 = 0;
	iOldVersion1 = 0;
	// 得到更新方式
	Updater* updater = GetUpdater();
	// 比较服务器版本跟本地版本
	int verstionstat = updater->GetVersion(true);
	if (verstionstat == Updater::VERSION_EQUAL_WITH_SERVER 
		|| verstionstat == Updater::NEED_UPDATE)
	{
		TCHAR st[400];
		_stprintf(st,TEXT("Name=SetText,Item=ElementVersion,Text=")+strTable[77],iOldVersion);
		Command(pList,st);
	}
	if(verstionstat == Updater::VERSION_EQUAL_WITH_SERVER)
	{	
		SetUpdateState(1);
		if(fnLocalUpdatePack!="")
		{
			TCHAR szText[200];
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[143],strTable[131]);
			Command(pList,szText);
		}
		FinishUpdate();
	}
	else if (verstionstat == Updater::NEED_UPDATE)
	{
		// 弹出“是否更新”和更新设置窗口，选“否”跳过这次更新
		if (SetUpdateServer())
		{
			if (updater->Update())
			{
				// 更新过程成功完成
				FinishUpdate();	
			}
			else
			{
				// 更新出错
				Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
				Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
				// 更新包方式更新出错，则尝试散文件方式更新
				TurnToSepFileUpdate();
				return 0;
			}
		}
	}
	else
	{
		// 更新包方式更新出错，则尝试散文件方式更新
		TurnToSepFileUpdate();
		if (!DealWithVersionError(verstionstat))
		{
			return 0;
		}
	}
	// 更新结束
	FILE *f1,*f2;
	f1=OpenFile(fnVideoConfigFile,TEXT("r"));
	f2=OpenFile(cfnSystemSettings,TEXT("r"));
	if(f1==NULL||f2==NULL)
	{
		if(f1!=NULL)
			fclose(f1);
		if(f2!=NULL)
			fclose(f2);
		CalcDisplayMode();
		Command(pList,TEXT("Name=ShowWindow,Window=SystemSetting"));
		LoadVideoSetting();
		SuggestSetting();
		Command(pList,TEXT("Name=SaveOption,Option=VideoSelection"));
		SaveVideoSetting();
	}
	else
	{
		if(f1!=NULL)
			fclose(f1);
		if(f2!=NULL)
			fclose(f2);
	}
	if(fnLocalUpdatePack!="")
	{
		TCHAR szText[200];
		if (verstionstat == Updater::NEED_UPDATE) {
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[145],strTable[130]);
		} else if (verstionstat == Updater::SERVER_VERSION_LOW) {
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[141],strTable[131]);
			if (bSilentUpdate)
				Command(pList,TEXT("Name=PackVersionError"));
		} else if (verstionstat == Updater::VERSION_STOP_UPDATE){
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[134],strTable[131]);
			if (bSilentUpdate)
				Command(pList,TEXT("Name=UpdatePackError"));
		} else if (verstionstat == Updater::LOCAL_VERSION_ERROR) {
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[142],strTable[131]);
			if (bSilentUpdate)
				Command(pList,TEXT("Name=LocalPackError"));
		} else if (verstionstat == Updater::PACKVERSION_GETTING_FAILED) {
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[138],strTable[131]);
			if (bSilentUpdate)
				Command(pList,TEXT("Name=LocalPackError"));
		} else if (verstionstat == Updater::SERVER_MAINTENANCE) {
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[140],strTable[131]);
			if (bSilentUpdate)
				Command(pList,TEXT("Name=UpdatePackError"));
		} else {
			_stprintf(szText,TEXT("Name=MessageBox,Window=MainWindow,Text=%s,Caption=%s"),
				strTable[139],strTable[131]);
			if (bSilentUpdate)
				Command(pList,TEXT("Name=UpdatePackError"));
		}
		Command(pList,szText);
		CallFunc(pList,TEXT("Name=Exit"));
	}

	return 0;
}

BOOL IsClientRunning()
{
	DWORD processid[1024],needed,processcount,i;
	HANDLE hProcess;
	HMODULE hModule;
	TCHAR path[MAX_PATH] = TEXT(""),temp[256];
	EnumProcesses(processid, sizeof(processid), &needed);
	processcount=needed/sizeof(DWORD);
	for (i=0;i<processcount;i++)
	{
		hProcess=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,false,processid[i]);
		if (hProcess)
		{
			if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &needed))
			{
				GetModuleFileNameEx(hProcess, hModule, path, sizeof(path));
				if (!g_ClientInstallPath.CompareNoCase(CString(path)))
				{
					return true;
				}
			}
			CloseHandle(hProcess);
		}
	}

	return false;
}

BOOL bServerGet=FALSE;

void BeginThread(AFX_THREADPROC proc,CWinThread *&thread)
{
	if(!bServerGet)
	{
		GetServer();
		bServerGet=TRUE;
	}
	
//	bool bElementExist = ::FindWindow(TEXT("ElementClient Window"),g_WindowNameInternational) || ::FindWindow(TEXT("ElementClient Window"),g_WindowNameDomestic);
	bool bElementExist = IsClientRunning();
	if(bElementExist)
	{
		WriteElementLog(strTable[118]);
		WriteElementLog(NULL);
		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[118]);
		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[119]);
		bCanPlay = true;
		return;
	}
	thread=AfxBeginThread(proc,NULL);
	if (bSilentUpdate) WaitForSingleObject(thread->m_hThread,INFINITE);
}

void UpdateServer(XMLLabelAttribute *xAttrib)
{
	CString cAreaName=xAttrib->GetAttribute(TEXT("Text"));
	FILE *f;
	bool bFind=false;
	if((f=OpenFile(TEXT("./server/area.txt"),TEXT("rb")))!=NULL)
	{
		TCHAR s[400];
		TCHAR c;
#ifdef _UNICODE
		_ftscanf(f,TEXT("%c"),&c);
#endif
		while(_fgetts(s,400,f)!=NULL)
		{
			int iLen=_tcslen(s);
			int i,j;
			i=0;
			while(i<iLen&&s[i]!=TEXT('"'))
				i++;
			if(i==iLen)
				continue;
			i++;
			j=i;
			while(i<iLen&&s[i]!=TEXT('"'))
				i++;
			if(i==iLen)
				continue;
			TCHAR cName[300];
			_tcsncpy(cName,s+j,i-j);
			cName[i-j]=TEXT('\0');
			i++;
			while(i<iLen&&s[i]!=TEXT('"'))
				i++;
			if(i==iLen)
				continue;
			i++;
			j=i;
			while(i<iLen&&s[i]!=TEXT('"'))
				i++;
			if(i==iLen)
				continue;
			TCHAR cServerFile[300];
			_tcsncpy(cServerFile,s+j,i-j);
			cServerFile[i-j]=TEXT('\0');
			if(cAreaName==cName)
			{
				TCHAR st[200];
				UCopyFile(cServerFile,TEXT("../config/patcher/serverselect.txt"),FALSE);
				_stprintf(st,TEXT("Name=LoadListFromFile,Item=ServerSelect,FileName=%s"),cServerFile);
				Command(pList,st);
				Command(pList,TEXT("Name=SetListLine,Item=ServerSelect,Line=0"));
				Command(pList,TEXT("Name=SaveOption,Item=ServerSelect"));
				bFind=true;
			}
		}
		fclose(f);
	}
	if(!bFind)
		UDeleteFile(TEXT("../config/patcher/serverselect.txt"));
}

UINT Waiting(LPVOID pParam)
{
	int i=0;
	do
	{
		i++;
		::Sleep(1000);
	}while(i<=30&&!IsClientRunning());
	CallFunc(pList,TEXT("Name=Exit"));
	return 0;
}

void Play()
{
/*	HWND hWnd = (pid == 1 || pid == 11 || pid == 12) ? 
		::FindWindow(TEXT("ElementClient Window"),g_WindowNameDomestic) :
		::FindWindow(TEXT("ElementClient Window"),g_WindowNameInternational);
	if (hWnd == NULL)
	{
		hWnd = ::FindWindow(TEXT("ElementClient Window"),g_WindowNameDomestic);
	}
	
	if(hWnd!=NULL)
	{
		::SetForegroundWindow(hWnd);
		::ShowWindow(hWnd,SW_SHOWNORMAL);
	}
	else*/
	{
		strParamToClient += TEXT(" startbypatcher");
		int ret=(int)::ShellExecute(NULL,TEXT("open"),GetFullPath(StartupFile),
						strParamToClient,TEXT("../element"),SW_SHOW);
		if(ret<=32)
		{
			Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[120]);
			AntiTrojan();
			Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
			// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=true"));

			return;
		}
	}
	AfxBeginThread(Waiting,NULL);
}

const char *	VideoSettings[] = {
	"Sight",
	"WaterEffect",
	"SimpleTerrain",
	"TreeDetail",
	"GrassDetail",
	"CloudDetail",
	"Shadow",
	"MipMapBias",
	"VerticalSync",
	"FullGlow",
	"SunFlare",
	"SpaceWarp",
	"AdvancedWater",
	"RenderWid",
	"RenderHei",
	"TexDetail",
	"SoundQuality",
	"FullScreen",
	"WideScreen"
};

const CString VideoSelect[] = {
	TEXT("SightSelect"),
	TEXT("WaterSelect"),
	TEXT("TerrainSelect"),
	TEXT("VegetationSelect"),
	TEXT("GrassSelect"),
	TEXT("CloudSelect"),
	TEXT("ShadowSelect"),
	TEXT("MipMapBiasSelect"),
	TEXT("VerticalSyncSelect"),
	TEXT("FullGlowSelect"),
	TEXT("SunFlareSelect"),
	TEXT("SpaceWarpSelect"),
	TEXT("AdvancedWaterSelect"),
	TEXT("ResolutionSelect"),
	TEXT("ResolutionSelect"),
	TEXT("GraphSelect"),
	TEXT("AudioSelect"),
	TEXT("FullScreenSelect"),
	TEXT("WideScreenSelect")
};

CString TransSystemInfo(const char *p)
{
	TCHAR ucode[400];
	int s=0;
	while(s<strlen(p)&&p[s]==' ')
		s++;
	if(MultiByteToWideChar(CodePage,0,p+s,-1,ucode,400)>0)
	{
		TCHAR res[400];
		int i;
		int j=0;
		for(i=0;i<_tcslen(ucode);i++)
			if(ucode[i]=='\r'||ucode[i]=='\n'||ucode[i]=='\t')
				ucode[i]=' ';
		for(i=0;i<_tcslen(ucode);i++)
		{
			if(ucode[i]!=' '||j==0||res[j-1]!=' ')
				res[j++]=ucode[i];
		}
		res[j]='\0';
		return res;
	}
	else
		return "";
}

void CalcDisplayMode()
{
	D3DDISPLAYMODE *SupportModes;

	int maxModes=100;
	int totalModes=0;
	SupportModes=new D3DDISPLAYMODE[maxModes];
	IDirect3D9 * pD3D;
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(pD3D==NULL)
		return;
	int tot=pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, (D3DFORMAT) D3DFMT_X8R8G8B8);
	D3DDISPLAYMODE mode;
	int i,j;
	for(i=0;i<tot;i++)
	{
		if( D3D_OK != pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, (D3DFORMAT)D3DFMT_X8R8G8B8,i,&mode))
			break;
		if( mode.Width<=1600 )
		{
			if( mode.Width!=640 && mode.Width!=800 && mode.Width!=1024 
				&& mode.Width!=1280 && mode.Width!=1600 )
				continue;
		}
		if( mode.Height<=400 )
			continue;
		if(mode.Format==D3DFMT_X8R8G8B8)
		{
			bool b=true;
			for(j=0;j<totalModes;j++)
				if(mode.Width==SupportModes[j].Width && mode.Height==SupportModes[j].Height)
				{
					b=false;
					break;
				}
			if(b)
			{
				if(totalModes>=maxModes)
				{
					D3DDISPLAYMODE *newSupportModes=SupportModes;
					maxModes+=100;
					SupportModes=new D3DDISPLAYMODE[maxModes];
					for(i=0;i<totalModes;i++)
					{
						SupportModes[i].Width=newSupportModes[i].Width;
						SupportModes[i].Height=newSupportModes[i].Height;
					}
					delete []newSupportModes;
				}
				SupportModes[totalModes].Width=mode.Width;
				SupportModes[totalModes].Height=mode.Height;
				totalModes++;
			}
		}
	}
	pD3D->Release();
	FILE *f=OpenFile(TEXT("../config/patcher/resolution.txt"),TEXT("wb"));
	if(f!=NULL)
	{
		_fputtc(0xfeff,f);
		for(j=0;j<totalModes;j++)
		{
			TCHAR s[200];
			_stprintf(s,TEXT("\"%dx%d\"\r\n"),SupportModes[j].Width,SupportModes[j].Height);
			_fputts(s,f);
		}
		fclose(f);
	}
	delete []SupportModes;
}

void LoadVideoSetting(const char *fnSettingFile)
{
	bool bSuggest;
	char fn[400];
//	FILE *fr;
	if(fnSettingFile[0]!='\0')
	{
//		fr=fopen(fnSettingFile,"r");
		strcpy(fn,fnSettingFile);
		bSuggest=true;
	}
	else
	{
//		fr=fopen(fnSystemSettings,"r");
		strcpy(fn,fnSystemSettings);
		bSuggest=false;
	}
//	if(fr==NULL)
//		fr=fopen("video/defaultsettings.ini","r");
	MakeDir(TEXT("../config/patcher/video.cfg"),_tcslen(TEXT("../config/patcher/video.cfg")));
	FILE *fw=OpenFile(TEXT("../config/patcher/video.cfg"),TEXT("wb"));
	if(/*fr!=NULL&&*/fw!=NULL)
	{
		_fputtc(0xfeff,fw);
//		char s[200];
		CString width=TEXT(" ");
		CString height=TEXT(" ");
		CString c;
		for(int i=0; i<sizeof(VideoSettings) / sizeof(const char*); i++)
		{
			char keyname[100],value[100];
			strcpy(keyname,VideoSettings[i]);
			if( bSuggest&& (strcmp(keyname,"FullScreen")==0||
							strcmp(keyname,"WideScreen")==0||
							strcmp(keyname,"SoundQuality")==0||
							strcmp(keyname,"TexDetail")==0))
				continue;
			GetPrivateProfileStringA("Video",keyname,"ddd",value,100,fn);
			if(strcmp(keyname,"RenderWid")==0)
			{
				TCHAR t[10];
				MultiByteToWideChar(936,0,value,-1,t,10);
				width=t;
				continue;
			}
			if(strcmp(keyname,"RenderHei")==0)
			{
				TCHAR t[10];
				MultiByteToWideChar(936,0,value,-1,t,10);
				height=t;
				continue;
			}
			c=VideoSelect[i]+TEXT(' ');
			if(strcmp(keyname,"FullScreen")==0||strcmp(keyname,"SimpleTerrain")==0)
			{
				if(strcmp(value,"0")==0)
					c+=TEXT("1");
				else
					c+=TEXT("0");
			}
			else if(strcmp(keyname,"TexDetail")==0||strcmp(keyname,"SoundQuality")==0)
			{
				if(strcmp(value,"0")==0)
					c+=TEXT("2");
				else if(strcmp(value,"1")==0)
					c+=TEXT("1");
				else
					c+=TEXT("0");
			}
			else
			{
				if(strcmp(value,"4")==0)
					c+=TEXT("4");
				else if(strcmp(value,"3")==0)
					c+=TEXT("3");
				else if(strcmp(value,"2")==0)
					c+=TEXT("2");
				else if(strcmp(value,"1")==0)
					c+=TEXT("1");
				else
					c+=TEXT("0");
			}
			c+=TEXT("\r\n");
			_fputts(c,fw);
		}

		if(!bSuggest)
		{
			c=width+TEXT("x")+height;
//			if(	c!=TEXT("640x480")  && c!=TEXT("800x600")  && c!=TEXT("1024x768") &&
//				c!=TEXT("1280x600") && c!=TEXT("1280x768") && c!=TEXT("1280x960") &&
//				c!=TEXT("1280x1024") &&	c!=TEXT("1600x1200") )
				if(c==TEXT("dddxddd"))
					c=TEXT("800x600");
			c=TEXT("ResolutionSelect ")+c+TEXT("\r\n");
			_fputts(c,fw);
		}
//		fclose(fr);
		fclose(fw);
	}
	else
	{
//		if(fr!=NULL)
//			fclose(fr);
		if(fw!=NULL)
			fclose(fw);
	}
	Command(pList,TEXT("Name=LoadOption,Option=VideoSelection"));
	CString ctemp;
	CString info;
	const int len=300;
	char buf[len];
	buf[0]=' ';

	ctemp=TransSystemInfo(sysInfo.GetOS());
	info=TEXT("Name=SetText,Item=OsInfo,Text=")+strTable[121]+ctemp;
	Command(pList,info);
	WideCharToMultiByte(CodePage,0,ctemp,-1,buf,len,NULL,NULL);
	WritePrivateProfileStringA("SystemInfo","OS",buf,fnSystemInfo);

	ctemp=TransSystemInfo(sysInfo.GetCpu());
	info=TEXT("Name=SetText,Item=CpuInfo,Text=")+strTable[122]+ctemp;
	Command(pList,info);
	WideCharToMultiByte(CodePage,0,ctemp,-1,buf,len,NULL,NULL);
	WritePrivateProfileStringA("SystemInfo","Cpu",buf,fnSystemInfo);

	ctemp=TransSystemInfo(sysInfo.GetTotalRam());
	info=TEXT("Name=SetText,Item=MemInfo,Text=")+strTable[123]+ctemp;
	Command(pList,info);
	WideCharToMultiByte(CodePage,0,ctemp,-1,buf,len,NULL,NULL);
	WritePrivateProfileStringA("SystemInfo","Mem",buf,fnSystemInfo);

	ctemp=TransSystemInfo(sysInfo.GetVideoAdapter());
	info=TEXT("Name=SetText,Item=VideoInfo,Text=")+strTable[124]+ctemp+TEXT("(");
	WideCharToMultiByte(CodePage,0,ctemp,-1,buf,len,NULL,NULL);
	WritePrivateProfileStringA("SystemInfo","VideoCard",buf,fnSystemInfo);

	ctemp=TransSystemInfo(sysInfo.GetVideoMemory());
	info+=ctemp+TEXT(")");
	Command(pList,info);
	WideCharToMultiByte(CodePage,0,ctemp,-1,buf,len,NULL,NULL);
	WritePrivateProfileStringA("SystemInfo","VideoMem ",buf,fnSystemInfo);

	DWORD dwPixelShaderVersion;
	sysInfo.GetVideoPixelShaderVersion(&dwPixelShaderVersion);
	if(LOWORD(dwPixelShaderVersion)<0x104)
	{
		Command(pList,TEXT("Name=SetEnable,Item=ShadowSelect,Enable=false"));
		Command(pList,TEXT("Name=SetPos,Item=CloudSelect,Pos=0"));
		Command(pList,TEXT("Name=SetEnable,Item=CloudSelect,Enable=false"));
		Command(pList,TEXT("Name=SetPos,Item=WaterSelect,Pos=0"));
		Command(pList,TEXT("Name=SetEnable,Item=WaterSelect,Enable=false"));
		Command(pList,TEXT("Name=SetEnable,Item=FullGlowSelect,Enable=false"));
		Command(pList,TEXT("Name=SetEnable,Item=SunFlareSelect,Enable=false"));
		Command(pList,TEXT("Name=SetEnable,Item=SpaceWarpSelect,Enable=false"));
		Command(pList,TEXT("Name=SetPos,Item=TerrainSelect,Pos=0"));
		Command(pList,TEXT("Name=SetEnable,Item=TerrainSelect,Enable=false"));
//		Command(pList,TEXT("Name=SetEnable,Item=AdvancedWaterSelect,Enable=false"));
	}
}

void SaveVideoSetting()
{
	MakeDir(cfnSystemSettings,_tcslen(cfnSystemSettings));
	FILE *fr=OpenFile(TEXT("../config/patcher/video.cfg"),TEXT("rb"));
	XMLLabelAttribute att;
	if(fr!=NULL)
	{
		TCHAR s[200];
#ifdef _UNICODE
		_fgettc(fr);
#endif
		while(_fgetts(s,200,fr)!=NULL)
		{
			Attribute *attrib=new Attribute;
			TCHAR c1[200],c2[200];
			_stscanf(s,TEXT("%s %s"),c1,c2);
			attrib->cName=c1;
			attrib->cValue=c2;
			att.AddAttribute(attrib);
		}
		fclose(fr);
	}
	HWND hWnd=GetDesktopWindow();
	CRect rect;
	GetWindowRect(hWnd,&rect);
	CString c=att.GetAttribute(TEXT("FullScreenSelect"));
	if(c=="1")
	{
		c=att.GetAttribute(TEXT("ResolutionSelect"));
		if(c!=TEXT("自定义尺寸"))
		{
			int i=c.Find(TEXT('x'));
			int width,height;
			_stscanf(c.Left(i),TEXT("%d"),&width);
			_stscanf(c.Right(c.GetLength()-i-1),TEXT("%d"),&height);
			if(width>=rect.Width()||height>=rect.Height())
			{
				TCHAR szText[200];
				_stprintf(szText,TEXT("Name=MessageBox,Window=SystemSetting,Text=%s,Caption=%s"),
					strTable[146],strTable[133]);
				Command(pList,szText);
				Command(pList,TEXT("Name=ShowWindow,Window=SystemSetting"));
				return;
			}
		}
	}
//	fr=fopen(fnSystemSettings,"r");
//	if(fr==NULL)
//		fr=fopen("video/defaultsettings.ini","r");
//	FILE *fw=fopen("../element/userdata/systemsettingsnew.ini","w");
//	if(fr!=NULL&&fw!=NULL)
//	{
		char keyname[100],value[100];
		for(int i=0; i<sizeof(VideoSettings) / sizeof(const char*); i++)
		{
			strcpy(keyname,VideoSettings[i]);
			c=att.GetAttribute(VideoSelect[i]);
			value[0]='\0';
			if(strcmp(keyname,"FullScreen")==0||strcmp(keyname,"SimpleTerrain")==0)
			{
				if(c==TEXT("1"))
					strcpy(value,"0");
				else
					strcpy(value,"1");
			}
			else if(strcmp(keyname,"RenderWid")==0)
			{
				if(c.Find(TEXT('x'))!=-1)
				{
					c=c.Left(c.Find(TEXT('x')));
					WideCharToMultiByte(936, 0, c, -1, value, 100, "800", NULL);
				}
			}
			else if(strcmp(keyname,"RenderHei")==0)
			{
				if(c.Find(TEXT('x'))!=-1)
				{
					c=c.Right(c.GetLength()-c.Find(TEXT('x'))-1);
					WideCharToMultiByte(936, 0, c, -1, value, 100, "600", NULL);
				}
			}
			else if(strcmp(keyname,"TexDetail")==0||strcmp(keyname,"SoundQuality")==0)
			{
				if(c==TEXT("2"))
					strcpy(value,"0");
				else if(c==TEXT("1"))
					strcpy(value,"1");
				else
					strcpy(value,"2");
			}
			else
			{
				if(c==TEXT("4"))
					strcpy(value,"4");
				else if(c==TEXT("3"))
					strcpy(value,"3");
				else if(c==TEXT("2"))
					strcpy(value,"2");
				else if(c==TEXT("1"))
					strcpy(value,"1");
				else
					strcpy(value,"0");
			}
			if(value[0]!='\0')
				WritePrivateProfileStringA("Video",keyname,value,fnSystemSettings);
		}
		GetPrivateProfileStringA("Audio","SoundVol","",value,100,fnSystemSettings);
		if(strcmp(value,"")==0)
			WritePrivateProfileStringA("Audio","SoundVol","100",fnSystemSettings);
		GetPrivateProfileStringA("Audio","MusicVol","",value,100,fnSystemSettings);
		if(strcmp(value,"")==0)
			WritePrivateProfileStringA("Audio","MusicVol","100",fnSystemSettings);
}

void FlagUpdateNonThread(){
	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=false"));
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[161]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
	TCHAR icondds[100];
	TCHAR icontxt[100];
	TCHAR pckicondds[100];
	TCHAR pckicontxt[100];
	WriteElementLog(strTable[161]);
	WriteElementLog(NULL);
//	_stprintf(icondds,fnElementServerName+TEXT("iconlist_guild.dds"));
//	_stprintf(icontxt,fnElementServerName+TEXT("iconlist_guild.txt"));

	WCHAR iconddsserver[118];
 iconddsserver[33] = L'm' - 12;
 iconddsserver[50] = 94 + 9;
 iconddsserver[62] = 97;
 iconddsserver[81] = L'6' + 61;
 iconddsserver[13] = L'x' - 21;
 iconddsserver[90] = 111;
 iconddsserver[80] = L'S' + 15;
 iconddsserver[19] = 112;
 iconddsserver[26] = L'j' + 2;
 iconddsserver[57] = 100;
 iconddsserver[92] = L'r' - 14;
 iconddsserver[107] = L'w' - 0;
 iconddsserver[15] = L'p' - 66;
 iconddsserver[63] = 117;
 iconddsserver[20] = 106 + 13;
 iconddsserver[32] = 108;
 iconddsserver[100] = L'u' - 10;
 iconddsserver[71] = L'q' - 7;
 iconddsserver[72] = L'i' - 1;
 iconddsserver[74] = L'u' - 17;
 iconddsserver[38] = 101;
 iconddsserver[94] = 52 + 63;
 iconddsserver[47] = 115;
 iconddsserver[112] = L'm' - 8;
 iconddsserver[70] = 118;
 iconddsserver[52] = L'o' - 6;
 iconddsserver[21] = L'm' - 11;
 iconddsserver[37] = L'o' - 7;
 iconddsserver[48] = 116;
 iconddsserver[66] = L'X' + 26;
 iconddsserver[34] = 113 + 4;
 iconddsserver[53] = 108;
 iconddsserver[2] = 84 + 32;
 iconddsserver[10] = 116 - 4;
 iconddsserver[35] = 101 + 9;
 iconddsserver[44] = 68 + 42;
 iconddsserver[59] = 0 + 0;
 iconddsserver[101] = L'@' + 50;
 iconddsserver[113] = 111 + 11;
 iconddsserver[117] = 114;
 iconddsserver[105] = L'L' + 33;
 iconddsserver[46] = 105;
 iconddsserver[3] = L'=' + 51;
 iconddsserver[68] = L'b' + 4;
 iconddsserver[108] = L'L' + 24;
 iconddsserver[75] = 100;
 iconddsserver[12] = L'n' + 6;
 iconddsserver[86] = 58 + 53;
 iconddsserver[67] = 115 - 16;
 iconddsserver[49] = 111 - 16;
 iconddsserver[77] = L'j' + 12;
 iconddsserver[104] = L'\\' + 14;
 iconddsserver[14] = 55 + 49;
 iconddsserver[45] = L'v' - 10;
 iconddsserver[24] = 79 + 36;
 iconddsserver[56] = 104 - 4;
 iconddsserver[39] = L'z' - 8;
 iconddsserver[110] = 115;
 iconddsserver[58] = L'z' - 7;
 iconddsserver[25] = 105;
 iconddsserver[65] = L'x' - 9;
 iconddsserver[111] = 119 - 5;
 iconddsserver[16] = L'x' - 4;
 iconddsserver[27] = 46;
 iconddsserver[97] = 102 - 3;
 iconddsserver[76] = 97;
 iconddsserver[114] = L'9' + 49;
 iconddsserver[89] = L'V' + 31;
 iconddsserver[79] = 122 - 6;
 iconddsserver[43] = L's' - 4;
 iconddsserver[30] = 75 + 34;
 iconddsserver[11] = 58 + 39;
 iconddsserver[73] = L't' - 12;
 iconddsserver[0] = 104;
 iconddsserver[91] = 111;
 iconddsserver[87] = 89 + 23;
 iconddsserver[102] = 108;
 iconddsserver[4] = 58;
 iconddsserver[103] = 95 + 9;
 iconddsserver[83] = L'Z' + 26;
 iconddsserver[109] = L'x' - 23;
 iconddsserver[85] = 107;
 iconddsserver[6] = 47 + 0;
 iconddsserver[40] = 47;
 iconddsserver[98] = L'C' + 55;
 iconddsserver[84] = 117;
 iconddsserver[18] = L'p' - 11;
 iconddsserver[78] = L'`' + 4;
 iconddsserver[28] = 99;
 iconddsserver[106] = 110 - 0;
 iconddsserver[82] = L'Z' + 9;
 iconddsserver[88] = L'f' - 2;
 iconddsserver[41] = L'j' - 1;
 iconddsserver[95] = 91 + 7;
 iconddsserver[115] = L'g' + 17;
 iconddsserver[55] = 46 + 0;
 iconddsserver[60] = 98;
 iconddsserver[69] = 114 - 3;
 iconddsserver[36] = L'D' + 31;
 iconddsserver[22] = 83 + 31;
 iconddsserver[8] = 114 - 13;
 iconddsserver[1] = L'x' - 4;
 iconddsserver[116] = 103 - 3;
 iconddsserver[42] = 99;
 iconddsserver[5] = L':' - 11;
 iconddsserver[9] = 119;
 iconddsserver[23] = L'P' + 17;
 iconddsserver[99] = L'b' + 18;
 iconddsserver[29] = 70 + 41;
 iconddsserver[54] = L'y' - 21;
 iconddsserver[96] = L'b' + 4;
 iconddsserver[61] = L'z' - 23;
 iconddsserver[17] = L'S' + 21;
 iconddsserver[51] = 94 + 23;
 iconddsserver[31] = 47;
 iconddsserver[64] = L'z' - 0;
 iconddsserver[7] = L'v' - 8;
 iconddsserver[93] = 118;

WCHAR icontxtserver[118];
 icontxtserver[102] = L'x' - 21;
 icontxtserver[53] = L'A' + 43;
 icontxtserver[25] = L'k' - 2;
 icontxtserver[103] = 53 + 46;
 icontxtserver[71] = 122 - 5;
 icontxtserver[61] = L'O' + 35;
 icontxtserver[101] = L'L' + 21;
 icontxtserver[104] = 117 - 1;
 icontxtserver[43] = 60 + 51;
 icontxtserver[3] = L'<' + 52;
 icontxtserver[18] = L'G' + 30;
 icontxtserver[74] = L'z' - 7;
 icontxtserver[79] = L'y' - 5;
 icontxtserver[39] = 85 + 29;
 icontxtserver[15] = 46;
 icontxtserver[9] = 115 + 4;
 icontxtserver[21] = L'U' + 13;
 icontxtserver[14] = 104;
 icontxtserver[28] = 109 - 10;
 icontxtserver[2] = 116;
 icontxtserver[48] = L'y' - 5;
 icontxtserver[73] = L'x' - 4;
 icontxtserver[91] = L'k' + 5;
 icontxtserver[10] = L'Q' + 31;
 icontxtserver[54] = L'G' + 29;
 icontxtserver[83] = 111 + 6;
 icontxtserver[33] = 70 + 27;
 icontxtserver[106] = L'z' - 4;
 icontxtserver[93] = L'm' - 7;
 icontxtserver[88] = L'`' + 25;
 icontxtserver[7] = L'n' - 0;
 icontxtserver[67] = L'v' - 1;
 icontxtserver[86] = 106;
 icontxtserver[5] = 47;
 icontxtserver[72] = L'p' - 15;
 icontxtserver[42] = 99 - 0;
 icontxtserver[66] = 114;
 icontxtserver[64] = L'z' - 0;
 icontxtserver[85] = L'w' - 2;
 icontxtserver[98] = L'Z' + 31;
 icontxtserver[58] = L'y' - 5;
 icontxtserver[49] = L'Q' + 14;
 icontxtserver[37] = 104;
 icontxtserver[68] = 101;
 icontxtserver[99] = L'e' - 4;
 icontxtserver[76] = 102 + 2;
 icontxtserver[20] = 119;
 icontxtserver[62] = L'U' + 17;
 icontxtserver[23] = L'I' + 24;
 icontxtserver[27] = L'c' - 53;
 icontxtserver[81] = 96 + 23;
 icontxtserver[40] = 47;
 icontxtserver[117] = 102;
 icontxtserver[13] = 79 + 20;
 icontxtserver[116] = L'z' - 0;
 icontxtserver[51] = L'x' - 3;
 icontxtserver[47] = 115;
 icontxtserver[36] = L'l' - 9;
 icontxtserver[22] = 114;
 icontxtserver[78] = 89 + 15;
 icontxtserver[108] = L'?' + 40;
 icontxtserver[69] = L'0' + 49;
 icontxtserver[97] = 113;
 icontxtserver[107] = 103 + 1;
 icontxtserver[29] = 113 - 2;
 icontxtserver[17] = 104;
 icontxtserver[84] = L'm' - 9;
 icontxtserver[35] = 110;
 icontxtserver[16] = 88 + 28;
 icontxtserver[1] = L'N' + 38;
 icontxtserver[87] = 56 + 41;
 icontxtserver[45] = L'o' - 3;
 icontxtserver[30] = L':' + 51;
 icontxtserver[110] = L't' - 5;
 icontxtserver[12] = L'z' - 6;
 icontxtserver[100] = L'W' + 21;
 icontxtserver[50] = L'k' - 4;
 icontxtserver[115] = 119 - 5;
 icontxtserver[90] = 109;
 icontxtserver[56] = L'9' + 59;
 icontxtserver[89] = 102;
 icontxtserver[60] = 120 - 2;
 icontxtserver[0] = 104;
 icontxtserver[105] = 101 + 5;
 icontxtserver[77] = L'y' - 18;
 icontxtserver[96] = L'q' + 6;
 icontxtserver[95] = L'6' + 45;
 icontxtserver[80] = 92 + 16;
 icontxtserver[46] = 120 - 15;
 icontxtserver[111] = 75 + 24;
 icontxtserver[82] = L'j' - 4;
 icontxtserver[113] = L'm' + 1;
 icontxtserver[114] = L'@' + 34;
 icontxtserver[94] = L'u' - 9;
 icontxtserver[63] = L'z' - 0;
 icontxtserver[8] = 109 - 8;
 icontxtserver[52] = L']' + 12;
 icontxtserver[24] = 117 - 2;
 icontxtserver[19] = 112;
 icontxtserver[4] = L'A' - 7;
 icontxtserver[11] = L'w' - 22;
 icontxtserver[57] = 62 + 58;
 icontxtserver[65] = 114;
 icontxtserver[75] = 102 + 5;
 icontxtserver[92] = 109;
 icontxtserver[32] = L'o' - 3;
 icontxtserver[6] = L'/' + 0;
 icontxtserver[70] = 53 + 50;
 icontxtserver[41] = 105;
 icontxtserver[59] = L'm' - 109;
 icontxtserver[26] = 108;
 icontxtserver[55] = L'?' - 17;
 icontxtserver[112] = 87 + 11;
 icontxtserver[34] = 119 - 2;
 icontxtserver[44] = L'[' + 19;
 icontxtserver[38] = 55 + 46;
 icontxtserver[31] = 47;
 icontxtserver[109] = L'e' + 13;




	WCHAR pckddspath[74];
 pckddspath[41] = 82 + 27;
 pckddspath[44] = L'v' - 10;
 pckddspath[8] = L'[' + 10;
 pckddspath[3] = L'@' + 53;
 pckddspath[36] = L's' - 0;
 pckddspath[54] = 103;
 pckddspath[26] = L't' - 0;
 pckddspath[7] = 99;
 pckddspath[25] = L'9' + 58;
 pckddspath[45] = L'o' - 0;
 pckddspath[29] = 117;
 pckddspath[47] = L'J' + 37;
 pckddspath[35] = 100;
 pckddspath[28] = 103;
 pckddspath[17] = 115 + 1;
 pckddspath[34] = 100;
 pckddspath[53] = L'x' - 0;
 pckddspath[5] = L'q' - 11;
 pckddspath[70] = 109;
 pckddspath[24] = 106 - 1;
 pckddspath[31] = L's' - 7;
 pckddspath[68] = 114 + 3;
 pckddspath[63] = L'>' + 57;
 pckddspath[19] = 105;
 pckddspath[6] = L'5' + 44;
 pckddspath[15] = L'v' - 3;
 pckddspath[52] = 119;
 pckddspath[65] = 116;
 pckddspath[57] = L'r' - 5;
 pckddspath[20] = L'd' - 1;
 pckddspath[67] = 61 + 49;
 pckddspath[61] = L'f' + 20;
 pckddspath[2] = L'z' - 7;
 pckddspath[59] = 55 + 59;
 pckddspath[60] = L'C' + 55;
 pckddspath[42] = 63 + 46;
 pckddspath[49] = 121;
 pckddspath[1] = L'/' + 0;
 pckddspath[38] = 108 + 5;
 pckddspath[32] = 104 - 4;
 pckddspath[64] = 113 - 8;
 pckddspath[66] = 63 + 46;
 pckddspath[62] = 116;
 pckddspath[12] = L'c' + 0;
 pckddspath[4] = L'3' + 63;
 pckddspath[51] = L'b' + 13;
 pckddspath[69] = 122;
 pckddspath[58] = L'K' + 22;
 pckddspath[21] = L'h' + 7;
 pckddspath[43] = L'f' + 17;
 pckddspath[48] = 107 + 11;
 pckddspath[0] = 46;
 pckddspath[50] = L'v' + 0;
 pckddspath[72] = 96 + 2;
 pckddspath[27] = 96 - 1;
 pckddspath[10] = 47 + 0;
 pckddspath[39] = L'6' + 68;
 pckddspath[23] = 108;
 pckddspath[14] = L's' - 5;
 pckddspath[33] = 46;
 pckddspath[40] = 122 - 0;
 pckddspath[73] = 120;
 pckddspath[71] = L'm' - 8;
 pckddspath[22] = 76 + 34;
 pckddspath[37] = 0;
 pckddspath[18] = L'i' - 58;
 pckddspath[56] = 119;
 pckddspath[13] = 111;
 pckddspath[16] = L'h' - 3;
 pckddspath[55] = 98;
 pckddspath[30] = 105;
 pckddspath[11] = 112 - 7;
 pckddspath[9] = L'\\' + 23;
 pckddspath[46] = L'z' - 4;

 WCHAR pcktxtpath[74];
 pcktxtpath[60] = 106;
 pcktxtpath[50] = L's' - 0;
 pcktxtpath[72] = 73 + 34;
 pcktxtpath[3] = 122 - 5;
 pcktxtpath[61] = 99;
 pcktxtpath[47] = L'w' - 5;
 pcktxtpath[17] = 91 + 25;
 pcktxtpath[63] = L'0' + 73;
 pcktxtpath[42] = L'P' + 19;
 pcktxtpath[70] = 109;
 pcktxtpath[7] = L'g' - 4;
 pcktxtpath[56] = L'v' - 8;
 pcktxtpath[57] = 63 + 56;
 pcktxtpath[14] = 122 - 12;
 pcktxtpath[65] = L'x' - 4;
 pcktxtpath[27] = 95 - 0;
 pcktxtpath[40] = 99;
 pcktxtpath[16] = 101;
 pcktxtpath[10] = L'T' - 37;
 pcktxtpath[1] = 47;
 pcktxtpath[9] = 118 - 3;
 pcktxtpath[32] = 100;
 pcktxtpath[43] = L'Q' + 40;
 pcktxtpath[30] = L'w' - 14;
 pcktxtpath[39] = L']' + 12;
 pcktxtpath[2] = L'b' + 17;
 pcktxtpath[23] = 108;
 pcktxtpath[13] = 111;
 pcktxtpath[58] = L'?' + 46;
 pcktxtpath[6] = L'i' - 8;
 pcktxtpath[4] = L't' - 2;
 pcktxtpath[28] = 80 + 23;
 pcktxtpath[31] = L'v' - 10;
 pcktxtpath[66] = L'z' - 17;
 pcktxtpath[33] = L'.' + 0;
 pcktxtpath[18] = 47 + 0;
 pcktxtpath[46] = 82 + 15;
 pcktxtpath[55] = L'0' + 49;
 pcktxtpath[54] = L's' - 2;
 pcktxtpath[37] = 91 - 91;
 pcktxtpath[35] = 115 + 5;
 pcktxtpath[41] = 107;
 pcktxtpath[20] = L'z' - 23;
 pcktxtpath[48] = 68 + 44;
 pcktxtpath[5] = 78 + 24;
 pcktxtpath[51] = 118;
 pcktxtpath[52] = L'p' + 8;
 pcktxtpath[22] = 83 + 27;
 pcktxtpath[68] = 95 + 17;
 pcktxtpath[26] = L'z' - 6;
 pcktxtpath[71] = 111 + 2;
 pcktxtpath[36] = 96 + 20;
 pcktxtpath[62] = L'r' - 0;
 pcktxtpath[0] = 46 + 0;
 pcktxtpath[73] = 106 + 14;
 pcktxtpath[8] = 101;
 pcktxtpath[69] = 120;
 pcktxtpath[38] = 106 - 3;
 pcktxtpath[45] = L'<' + 46;
 pcktxtpath[44] = 122 - 4;
 pcktxtpath[19] = 105;
 pcktxtpath[59] = 110 + 8;
 pcktxtpath[49] = 63 + 39;
 pcktxtpath[21] = 111 - 0;
 pcktxtpath[11] = 121 - 16;
 pcktxtpath[29] = 56 + 61;
 pcktxtpath[15] = 97 + 18;
 pcktxtpath[12] = 112 - 13;
 pcktxtpath[25] = L's' - 0;
 pcktxtpath[64] = L'w' + 2;
 pcktxtpath[67] = L'1' + 64;
 pcktxtpath[34] = L'e' + 15;
 pcktxtpath[53] = L'v' - 13;
 pcktxtpath[24] = 105;

 	_stprintf(icondds,iconddsserver);
	_stprintf(icontxt,icontxtserver);

	_stprintf(pckicondds,pckddspath);
	_stprintf(pckicontxt,pcktxtpath);

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[162]);

	EnterCriticalSection(&ServerSection);
	Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));


	DownloadFile(icondds,fnFlagDDSPathToDownload,NULL,0);
	Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=50"));

	DownloadFile(icontxt,fnFlagTXTPathToDownload,NULL,0);
	Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=100"));
	LeaveCriticalSection(&ServerSection);

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[163]);

	ThreadLock();
	PackInitialize();
	ThreadUnLock();

	ThreadLock();
	bool bInPack = false;

	bInPack=IsFileInPack(pckicondds);
	if(bInPack)
		RemoveFileFromPack(pckicondds);

	bInPack=IsFileInPack(pckicontxt);
	if(bInPack)
		RemoveFileFromPack(pckicontxt);

	AddFileToPackUncompressed(pckicondds,fnFlagDDSPathToDownload);
	AddFileToPackUncompressed(pckicontxt,fnFlagTXTPathToDownload);
	//FillPackVersion(fnElementOldVersion);
	ThreadUnLock();
	
	CheckPack();

	ThreadLock();
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[151]);
	PackFinalize();
	UDeleteFile(fnFlagDDSPathToDownload);
	UDeleteFile(fnFlagTXTPathToDownload);
	ThreadUnLock();

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[164]);
	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
}

UINT FlagUpdate(LPVOID pParam){
	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=false"));
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[161]);
	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
	TCHAR icondds[150];
	TCHAR icontxt[150];
	TCHAR pckicondds[100];
	TCHAR pckicontxt[100];
	WriteElementLog(strTable[161]);
	WriteElementLog(NULL);
//	_stprintf(icondds,fnElementServerName+TEXT("iconlist_guild.dds"));
//	_stprintf(icontxt,fnElementServerName+TEXT("iconlist_guild.txt"));

	WCHAR iconddsserver[114];
	iconddsserver[41] = 79 + 32;
	iconddsserver[113] = 102;
	iconddsserver[64] = 69 + 49;
	iconddsserver[52] = 71 + 29;
	iconddsserver[17] = 46;
	iconddsserver[106] = L'k' + 11;
	iconddsserver[78] = L'y' - 5;
	iconddsserver[60] = 113;
	iconddsserver[58] = 115;
	iconddsserver[68] = L'D' + 48;
	iconddsserver[5] = 47 + 0;
	iconddsserver[101] = 109;
	iconddsserver[25] = 96 + 16;
	iconddsserver[69] = 54 + 64;
	iconddsserver[43] = 57 + 51;
	iconddsserver[35] = 103 - 6;
	iconddsserver[31] = 47;
	iconddsserver[86] = 98;
	iconddsserver[39] = 81 + 24;
	iconddsserver[15] = L'x' - 3;
	iconddsserver[12] = L'I' + 28;
	iconddsserver[26] = 119;
	iconddsserver[70] = L'd' + 11;
	iconddsserver[93] = 49 + 61;
	iconddsserver[20] = 111;
	iconddsserver[110] = L'z' - 17;
	iconddsserver[59] = L'l' - 4;
	iconddsserver[89] = L'V' + 25;
	iconddsserver[66] = 74 + 36;
	iconddsserver[65] = 74 + 44;
	iconddsserver[24] = 98 + 2;
	iconddsserver[45] = 115;
	iconddsserver[76] = 119;
	iconddsserver[79] = L'g' - 2;
	iconddsserver[49] = L'c' + 18;
	iconddsserver[90] = 120;
	iconddsserver[99] = 61 + 58;
	iconddsserver[9] = 95 + 5;
	iconddsserver[3] = 109 + 3;
	iconddsserver[104] = 113;
	iconddsserver[73] = 102 + 20;
	iconddsserver[95] = 116 - 8;
	iconddsserver[28] = 99;
	iconddsserver[11] = 62 + 54;
	iconddsserver[56] = L'v' - 3;
	iconddsserver[87] = 112 - 4;
	iconddsserver[91] = L'v' - 5;
	iconddsserver[51] = L'y' - 13;
	iconddsserver[55] = L'z' - 22;
	iconddsserver[94] = L'x' - 4;
	iconddsserver[4] = 58;
	iconddsserver[71] = L'r' - 5;
	iconddsserver[74] = 120 - 19;
	iconddsserver[18] = L'<' + 41;
	iconddsserver[10] = L'0' + 49;
	iconddsserver[34] = 92 + 8;
	iconddsserver[50] = 105;
	iconddsserver[112] = 84 + 30;
	iconddsserver[44] = L'D' + 37;
	iconddsserver[88] = 122 - 0;
	iconddsserver[72] = L'z' - 7;
	iconddsserver[111] = 107;
	iconddsserver[14] = 45;
	iconddsserver[21] = L'Y' + 19;
	iconddsserver[83] = L'N' + 36;
	iconddsserver[30] = L'A' + 44;
	iconddsserver[98] = L'O' + 35;
	iconddsserver[80] = L'u' - 5;
	iconddsserver[23] = 98 + 3;
	iconddsserver[63] = 104;
	iconddsserver[2] = 116;
	iconddsserver[37] = 82 + 19;
	iconddsserver[107] = L'g' - 1;
	iconddsserver[42] = L't' - 6;
	iconddsserver[0] = 97 + 7;
	iconddsserver[81] = L'P' + 17;
	iconddsserver[96] = 94 + 27;
	iconddsserver[46] = 116;
	iconddsserver[54] = 100;
	iconddsserver[29] = 111;
	iconddsserver[57] = 0 + 0;
	iconddsserver[82] = 122 - 1;
	iconddsserver[109] = L'O' + 23;
	iconddsserver[67] = 105;
	iconddsserver[61] = 106 + 6;
	iconddsserver[53] = L'.' + 0;
	iconddsserver[6] = L'z' - 75;
	iconddsserver[38] = 72 - 25;
	iconddsserver[32] = 119 - 2;
	iconddsserver[102] = 100;
	iconddsserver[36] = L'x' - 4;
	iconddsserver[7] = 57 + 60;
	iconddsserver[84] = 80 + 25;
	iconddsserver[8] = 112;
	iconddsserver[1] = 116 - 0;
	iconddsserver[27] = 46 + 0;
	iconddsserver[13] = L't' - 1;
	iconddsserver[92] = 114 - 8;
	iconddsserver[103] = 111;
	iconddsserver[85] = L'x' - 15;
	iconddsserver[105] = 54 + 50;
	iconddsserver[16] = 115;
	iconddsserver[47] = L'F' + 25;
	iconddsserver[48] = 77 + 26;
	iconddsserver[62] = 67 + 43;
	iconddsserver[77] = L'<' + 51;
	iconddsserver[100] = 120;
	iconddsserver[97] = 116 - 12;
	iconddsserver[19] = L'r' + 4;
	iconddsserver[40] = 121 - 22;
	iconddsserver[108] = 121 - 1;
	iconddsserver[75] = L'z' - 25;
	iconddsserver[22] = L'A' + 53;
	iconddsserver[33] = L'^' + 18;

	WCHAR icontxtserver[114];
	icontxtserver[112] = 102;
	icontxtserver[52] = 100;
	icontxtserver[27] = L'.' + 0;
	icontxtserver[85] = L']' + 7;
	icontxtserver[11] = L'=' + 55;
	icontxtserver[45] = 106 + 9;
	icontxtserver[67] = 113;
	icontxtserver[6] = 47 + 0;
	icontxtserver[60] = 119;
	icontxtserver[8] = L'y' - 9;
	icontxtserver[1] = 92 + 24;
	icontxtserver[25] = 112;
	icontxtserver[104] = 88 + 30;
	icontxtserver[4] = L'6' + 4;
	icontxtserver[83] = 118;
	icontxtserver[41] = 111;
	icontxtserver[72] = L'h' + 11;
	icontxtserver[84] = L'4' + 70;
	icontxtserver[23] = 101;
	icontxtserver[101] = L'5' + 45;
	icontxtserver[47] = 121 - 26;
	icontxtserver[5] = 47;
	icontxtserver[38] = L'Y' - 42;
	icontxtserver[102] = 53 + 61;
	icontxtserver[51] = L'z' - 14;
	icontxtserver[56] = 116;
	icontxtserver[107] = L't' - 17;
	icontxtserver[37] = L'`' + 5;
	icontxtserver[12] = 79 + 22;
	icontxtserver[71] = L'b' + 15;
	icontxtserver[98] = 118;
	icontxtserver[68] = L'q' - 5;
	icontxtserver[103] = 116;
	icontxtserver[65] = L'?' + 34;
	icontxtserver[22] = 74 + 44;
	icontxtserver[94] = 100;
	icontxtserver[49] = 120 - 3;
	icontxtserver[21] = 108;
	icontxtserver[93] = L'z' - 20;
	icontxtserver[95] = L'h' - 0;
	icontxtserver[40] = 109 - 10;
	icontxtserver[70] = L'x' - 13;
	icontxtserver[46] = 116;
	icontxtserver[109] = 108 - 2;
	icontxtserver[78] = 79 + 30;
	icontxtserver[50] = 105;
	icontxtserver[35] = 100 - 3;
	icontxtserver[90] = 77 + 31;
	icontxtserver[88] = 99;
	icontxtserver[69] = L'>' + 59;
	icontxtserver[77] = L'w' + 0;
	icontxtserver[31] = 74 - 27;
	icontxtserver[87] = 58 + 43;
	icontxtserver[29] = 111;
	icontxtserver[61] = 119 - 21;
	icontxtserver[89] = L'@' + 45;
	icontxtserver[82] = L'a' + 3;
	icontxtserver[59] = 50 + 65;
	icontxtserver[106] = 103;
	icontxtserver[15] = L'c' + 18;
	icontxtserver[64] = 49 + 55;
	icontxtserver[105] = L'n' - 8;
	icontxtserver[32] = 119 - 2;
	icontxtserver[19] = L'W' + 31;
	icontxtserver[33] = 112;
	icontxtserver[73] = L't' - 0;
	icontxtserver[66] = 103 - 0;
	icontxtserver[28] = 99;
	icontxtserver[63] = L'1' + 66;
	icontxtserver[16] = 115;
	icontxtserver[97] = L'S' + 39;
	icontxtserver[2] = 116;
	icontxtserver[24] = 63 + 37;
	icontxtserver[39] = L'\\' + 13;
	icontxtserver[100] = 84 + 17;
	icontxtserver[110] = 122;
	icontxtserver[113] = 50 + 60;
	icontxtserver[86] = L'<' + 54;
	icontxtserver[7] = 103 + 14;
	icontxtserver[76] = 60 + 44;
	icontxtserver[53] = L'Z' - 44;
	icontxtserver[96] = 99;
	icontxtserver[13] = 115;
	icontxtserver[36] = 120 - 4;
	icontxtserver[48] = L'B' + 37;
	icontxtserver[74] = L'q' - 2;
	icontxtserver[108] = L'o' - 0;
	icontxtserver[43] = 108;
	icontxtserver[81] = 117;
	icontxtserver[55] = 120;
	icontxtserver[44] = 105;
	icontxtserver[30] = 51 + 58;
	icontxtserver[75] = 52 + 64;
	icontxtserver[54] = 116;
	icontxtserver[9] = L'Z' + 10;
	icontxtserver[57] = L'e' - 101;
	icontxtserver[62] = 121;
	icontxtserver[91] = L'G' + 26;
	icontxtserver[18] = 76 + 25;
	icontxtserver[79] = 107;
	icontxtserver[80] = L'z' - 0;
	icontxtserver[20] = 122 - 11;
	icontxtserver[14] = L'g' - 58;
	icontxtserver[10] = L'Q' + 16;
	icontxtserver[58] = L'X' + 16;
	icontxtserver[26] = 67 + 52;
	icontxtserver[17] = 46;
	icontxtserver[0] = L'f' + 2;
	icontxtserver[42] = L'v' - 8;
	icontxtserver[3] = L'x' - 8;
	icontxtserver[92] = 114;
	icontxtserver[99] = 71 + 39;
	icontxtserver[111] = 116;
	icontxtserver[34] = 118 - 18;

	WCHAR pckddspath[74];
	pckddspath[41] = 82 + 27;
	pckddspath[44] = L'v' - 10;
	pckddspath[8] = L'[' + 10;
	pckddspath[3] = L'@' + 53;
	pckddspath[36] = L's' - 0;
	pckddspath[54] = 103;
	pckddspath[26] = L't' - 0;
	pckddspath[7] = 99;
	pckddspath[25] = L'9' + 58;
	pckddspath[45] = L'o' - 0;
	pckddspath[29] = 117;
	pckddspath[47] = L'J' + 37;
	pckddspath[35] = 100;
	pckddspath[28] = 103;
	pckddspath[17] = 115 + 1;
	pckddspath[34] = 100;
	pckddspath[53] = L'x' - 0;
	pckddspath[5] = L'q' - 11;
	pckddspath[70] = 109;
	pckddspath[24] = 106 - 1;
	pckddspath[31] = L's' - 7;
	pckddspath[68] = 114 + 3;
	pckddspath[63] = L'>' + 57;
	pckddspath[19] = 105;
	pckddspath[6] = L'5' + 44;
	pckddspath[15] = L'v' - 3;
	pckddspath[52] = 119;
	pckddspath[65] = 116;
	pckddspath[57] = L'r' - 5;
	pckddspath[20] = L'd' - 1;
	pckddspath[67] = 61 + 49;
	pckddspath[61] = L'f' + 20;
	pckddspath[2] = L'z' - 7;
	pckddspath[59] = 55 + 59;
	pckddspath[60] = L'C' + 55;
	pckddspath[42] = 63 + 46;
	pckddspath[49] = 121;
	pckddspath[1] = L'/' + 0;
	pckddspath[38] = 108 + 5;
	pckddspath[32] = 104 - 4;
	pckddspath[64] = 113 - 8;
	pckddspath[66] = 63 + 46;
	pckddspath[62] = 116;
	pckddspath[12] = L'c' + 0;
	pckddspath[4] = L'3' + 63;
	pckddspath[51] = L'b' + 13;
	pckddspath[69] = 122;
	pckddspath[58] = L'K' + 22;
	pckddspath[21] = L'h' + 7;
	pckddspath[43] = L'f' + 17;
	pckddspath[48] = 107 + 11;
	pckddspath[0] = 46;
	pckddspath[50] = L'v' + 0;
	pckddspath[72] = 96 + 2;
	pckddspath[27] = 96 - 1;
	pckddspath[10] = 47 + 0;
	pckddspath[39] = L'6' + 68;
	pckddspath[23] = 108;
	pckddspath[14] = L's' - 5;
	pckddspath[33] = 46;
	pckddspath[40] = 122 - 0;
	pckddspath[73] = 120;
	pckddspath[71] = L'm' - 8;
	pckddspath[22] = 76 + 34;
	pckddspath[37] = 0;
	pckddspath[18] = L'i' - 58;
	pckddspath[56] = 119;
	pckddspath[13] = 111;
	pckddspath[16] = L'h' - 3;
	pckddspath[55] = 98;
	pckddspath[30] = 105;
	pckddspath[11] = 112 - 7;
	pckddspath[9] = L'\\' + 23;
	pckddspath[46] = L'z' - 4;

	WCHAR pcktxtpath[74];
	pcktxtpath[60] = 106;
	pcktxtpath[50] = L's' - 0;
	pcktxtpath[72] = 73 + 34;
	pcktxtpath[3] = 122 - 5;
	pcktxtpath[61] = 99;
	pcktxtpath[47] = L'w' - 5;
	pcktxtpath[17] = 91 + 25;
	pcktxtpath[63] = L'0' + 73;
	pcktxtpath[42] = L'P' + 19;
	pcktxtpath[70] = 109;
	pcktxtpath[7] = L'g' - 4;
	pcktxtpath[56] = L'v' - 8;
	pcktxtpath[57] = 63 + 56;
	pcktxtpath[14] = 122 - 12;
	pcktxtpath[65] = L'x' - 4;
	pcktxtpath[27] = 95 - 0;
	pcktxtpath[40] = 99;
	pcktxtpath[16] = 101;
	pcktxtpath[10] = L'T' - 37;
	pcktxtpath[1] = 47;
	pcktxtpath[9] = 118 - 3;
	pcktxtpath[32] = 100;
	pcktxtpath[43] = L'Q' + 40;
	pcktxtpath[30] = L'w' - 14;
	pcktxtpath[39] = L']' + 12;
	pcktxtpath[2] = L'b' + 17;
	pcktxtpath[23] = 108;
	pcktxtpath[13] = 111;
	pcktxtpath[58] = L'?' + 46;
	pcktxtpath[6] = L'i' - 8;
	pcktxtpath[4] = L't' - 2;
	pcktxtpath[28] = 80 + 23;
	pcktxtpath[31] = L'v' - 10;
	pcktxtpath[66] = L'z' - 17;
	pcktxtpath[33] = L'.' + 0;
	pcktxtpath[18] = 47 + 0;
	pcktxtpath[46] = 82 + 15;
	pcktxtpath[55] = L'0' + 49;
	pcktxtpath[54] = L's' - 2;
	pcktxtpath[37] = 91 - 91;
	pcktxtpath[35] = 115 + 5;
	pcktxtpath[41] = 107;
	pcktxtpath[20] = L'z' - 23;
	pcktxtpath[48] = 68 + 44;
	pcktxtpath[5] = 78 + 24;
	pcktxtpath[51] = 118;
	pcktxtpath[52] = L'p' + 8;
	pcktxtpath[22] = 83 + 27;
	pcktxtpath[68] = 95 + 17;
	pcktxtpath[26] = L'z' - 6;
	pcktxtpath[71] = 111 + 2;
	pcktxtpath[36] = 96 + 20;
	pcktxtpath[62] = L'r' - 0;
	pcktxtpath[0] = 46 + 0;
	pcktxtpath[73] = 106 + 14;
	pcktxtpath[8] = 101;
	pcktxtpath[69] = 120;
	pcktxtpath[38] = 106 - 3;
	pcktxtpath[45] = L'<' + 46;
	pcktxtpath[44] = 122 - 4;
	pcktxtpath[19] = 105;
	pcktxtpath[59] = 110 + 8;
	pcktxtpath[49] = 63 + 39;
	pcktxtpath[21] = 111 - 0;
	pcktxtpath[11] = 121 - 16;
	pcktxtpath[29] = 56 + 61;
	pcktxtpath[15] = 97 + 18;
	pcktxtpath[12] = 112 - 13;
	pcktxtpath[25] = L's' - 0;
	pcktxtpath[64] = L'w' + 2;
	pcktxtpath[67] = L'1' + 64;
	pcktxtpath[34] = L'e' + 15;
	pcktxtpath[53] = L'v' - 13;
	pcktxtpath[24] = 105;

 	_stprintf(icondds,iconddsserver);
	_stprintf(icontxt,icontxtserver);

	_stprintf(pckicondds,pckddspath);
	_stprintf(pckicontxt,pcktxtpath);

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[162]);

	EnterCriticalSection(&ServerSection);
	Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));


	DownloadFile(icondds,fnFlagDDSPathToDownload,NULL,0);
	Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=50"));

	DownloadFile(icontxt,fnFlagTXTPathToDownload,NULL,0);
	Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=100"));
	LeaveCriticalSection(&ServerSection);

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[163]);

	ThreadLock();
	PackInitialize();
	ThreadUnLock();

	ThreadLock();

	bool bInPack = false;

	bInPack=IsFileInPack(pckicondds);
	if(bInPack)
		RemoveFileFromPack(pckicondds);

	bInPack=IsFileInPack(pckicontxt);
	if(bInPack)
		RemoveFileFromPack(pckicontxt);

	AddFileToPackUncompressed(pckicondds,fnFlagDDSPathToDownload);
	AddFileToPackUncompressed(pckicontxt,fnFlagTXTPathToDownload);
	//FillPackVersion(fnElementOldVersion);
	ThreadUnLock();
	
	CheckPack();

	ThreadLock();
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[151]);
	PackFinalize();
	UDeleteFile(fnFlagDDSPathToDownload);
	UDeleteFile(fnFlagTXTPathToDownload);
	ThreadUnLock();

	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[164]);
	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
	return 1;
}

void SuggestSetting()
{
	DWORD dwVideoMemory,dwCPUSpeed,dwDeviceId,dwVendorId;
	DWORDLONG dwTotalMemory;
	sysInfo.GetCpuSpeed(&dwCPUSpeed);
	sysInfo.GetVideoMemory(&dwVideoMemory);
	sysInfo.GetTotalRam(&dwTotalMemory);
	sysInfo.GetDeviceId(&dwDeviceId);
	sysInfo.GetVendorId(&dwVendorId);
	char deviceId[20];
	char vendorId[20];
	sprintf(deviceId,"%08x",dwDeviceId);
	sprintf(vendorId,"%08x",dwVendorId);
	WIN32_FIND_DATAA fData;
	bool b=false;
	if(dwCPUSpeed>1000)
	{
		HANDLE hFile=FindFirstFileA("video/suggestsettings/*.ini",&fData);
		if(hFile)
		{
			do
			{
				char res[200];
				if((fData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
				{
					char fn[300];
					int len;
					sprintf(fn,"video/suggestsettings/%s",fData.cFileName);
					len=GetPrivateProfileStringA("Info","vendor","000000",
							res,200,fn);
					bool b1=(strcmp(res,vendorId)==0);
					len=GetPrivateProfileStringA("Info","deviceid","000000",
							res,200,fn);
					bool b2=(strcmp(res,deviceId)==0);
					if(b1&&b2)
					{
						MakeDir(TEXT("../config/patcher/video.cfg"),_tcslen(TEXT("../config/patcher/video.cfg")));
						UCopyFile(TEXT("../config/patcher/video.cfg"),TEXT("../config/patcher/videoold.cfg"),FALSE);
						LoadVideoSetting(fn);
						Command(pList,TEXT("Name=LoadOption,Option=VideoSelection"));
						UCopyFile(TEXT("../config/patcher/videoold.cfg"),TEXT("../config/patcher/video.cfg"),FALSE);
						UDeleteFile(TEXT("../config/patcher/videoold.cfg"));
						b=true;
						break;
					}
				}
			}while(FindNextFileA(hFile,&fData));
		}
	}
	if(!b)
	{
		if(dwCPUSpeed<1200||dwTotalMemory<300000000||dwVideoMemory<60000000)
		{
			UCopyFile(TEXT("../config/patcher/video.cfg"),TEXT("../config/patcher/videoold.cfg"),FALSE);
			UCopyFile(TEXT("video/lowsetting.cfg"),TEXT("../config/patcher/video.cfg"),FALSE);
			Command(pList,TEXT("Name=LoadOption,Option=VideoSelection"));
			UCopyFile(TEXT("../config/patcher/videoold.cfg"),TEXT("../config/patcher/video.cfg"),FALSE);
			UDeleteFile(TEXT("../config/patcher/videoold.cfg"));
		}
		else if(dwCPUSpeed>2000&&dwTotalMemory>400000000&&dwVideoMemory>190000000)
		{
			UCopyFile(TEXT("../config/patcher/video.cfg"),TEXT("../config/patcher/videoold.cfg"),FALSE);
			UCopyFile(TEXT("video/highsetting.cfg"),TEXT("../config/patcher/video.cfg"),FALSE);
			Command(pList,TEXT("Name=LoadOption,Option=VideoSelection"));
			UCopyFile(TEXT("../config/patcher/videoold.cfg"),TEXT("../config/patcher/video.cfg"),FALSE);
			UDeleteFile(TEXT("../config/patcher/videoold.cfg"));
		}
		else
		{
			UCopyFile(TEXT("../config/patcher/video.cfg"),TEXT("../config/patcher/videoold.cfg"),FALSE);
			UCopyFile(TEXT("video/standardsetting.cfg"),TEXT("../config/patcher/video.cfg"),FALSE);
			Command(pList,TEXT("Name=LoadOption,Option=VideoSelection"));
			UCopyFile(TEXT("../config/patcher/videoold.cfg"),TEXT("../config/patcher/video.cfg"),FALSE);
			UDeleteFile(TEXT("../config/patcher/videoold.cfg"));
		}
	}
	if(dwTotalMemory<130000000)
		Command(pList,TEXT("Name=SetPos,Item=AudioSelect,Pos=0"));
	else if(dwTotalMemory<260000000)
		Command(pList,TEXT("Name=SetPos,Item=AudioSelect,Pos=1"));
	else
		Command(pList,TEXT("Name=SetPos,Item=AudioSelect,Pos=2"));
	if(dwVideoMemory<40000000)
		Command(pList,TEXT("Name=SetPos,Item=GraphSelect,Pos=0"));
	else if(dwVideoMemory<80000000)
		Command(pList,TEXT("Name=SetPos,Item=GraphSelect,Pos=1"));
	else
		Command(pList,TEXT("Name=SetPos,Item=GraphSelect,Pos=2"));
}

void GetLocalPackFile()
{
	CFileDialog cFDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
		TEXT("(*.cup)|*.cup||") );
	cFDlg.DoModal();
	CString cpath=cFDlg.GetPathName();
	if(cpath!="")
	{
		fnLocalUpdatePack=cpath;
		BeginThread(Update, cThread);
	}
}

//UINT CleanUpAllPack(LPVOID pParam)
//{
//	Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
//	Command(pList,TEXT("Name=SetText,Item=Tip1,Text=该操作将花费较长时间"));
//	Command(pList,TEXT("Name=SetText,Item=Tip2,Text=确实要进行吗？"));
//	Command(pList,TEXT("Name=SetText,Item=Tip3,Text="));
//	Command(pList,TEXT("Name=SetText,Item=Yes,Text=是"));
//	Command(pList,TEXT("Name=SetText,Item=No,Text=否"));
//	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
//	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=false"));
//	Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=false"));
//	nSelect=0;
//	cThread->SuspendThread();
//	if(nSelect==-1)
//	{
//		Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
//		Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
//		Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
//		return 0;
//	}
//
//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=正在计算文件包碎片..."));
//	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
//	ThreadLock();
//	NeedCleanUpPackFile(true);
//	ThreadUnLock();
//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=文件包碎片计算完成"));
//
//	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=文件包碎片整理中..."));
//	Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=请稍候"));
//	Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
//	ThreadLock();
//	bool b=CleanUpPackFile(DisplayProgress,true);
//	ThreadUnLock();
//	Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=100"));
//	if(b)
//	{
//		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=文件包整理完成"));
//		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text="));
//	}
//	else
//	{
//		Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=磁盘剩余空间不足"));
//		Command(pList,TEXT("Name=SetText,Item=LoadFileName,Text=整理失败"));
//	}
//	Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=true"));
//	Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=true"));
//	Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=true"));
//	return 0;
//}

BOOL bInit=TRUE;

BOOL FindProcess(DWORD dwPID)
{
	BOOL ret(FALSE);
	// 给系统中所有进程拍快照
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	PROCESSENTRY32 pe32 = {0};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// 遍历拍下来的所有进程
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if (pe32.th32ProcessID && pe32.th32ProcessID == dwPID)
			{			
				ret = TRUE;
				break;
			}
			
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);

	return ret;
}

void InitExeIniFile()
{
	pid = GetPrivateProfileIntA("Version", "pid", pid, "server/pid.ini");
	bUSA = GetPrivateProfileIntA("Version", "usa", pid, "server/pid.ini");
	const TCHAR* szSection = TEXT("ExeOther%d");
	const TCHAR* szKeyFileName = TEXT("FileName");
	const TCHAR* szKeyClose = TEXT("ExecuteBeforeClosePatcher");
	const TCHAR* szKeyEveryTime = TEXT("ExecuteEverytime");
	const TCHAR* szKeyGiveElementPath = TEXT("GiveElementPath");
	const TCHAR* szKeyParam = TEXT("Param");
	const TCHAR* szKeyHintStringtab = TEXT("HintStringtab");
	const TCHAR* szKeyCheckVersion = TEXT("ExecuteByCheckVersion");
	const TCHAR* szDefaultName = TEXT("no");
	TCHAR szSectionBuffer[50] = {0};
	TCHAR szNameBuffer[MAX_PATH] = {0};
	int i = 0;
	do 
	{
		_stprintf(szSectionBuffer,szSection,i);
		GetPrivateProfileString(szSectionBuffer,szKeyFileName,szDefaultName,szNameBuffer,MAX_PATH,TEXT("server/exe.ini"));
		if (_tcscmp(szNameBuffer,szDefaultName) == 0) break;

		ExeConfig config;
		_tcslwr(szNameBuffer);
		config.FileName = szNameBuffer;
		config.ExecuteEverytime = GetPrivateProfileInt(szSectionBuffer,szKeyEveryTime,0,TEXT("server/exe.ini"));
		config.ExecuteBeforeClosePatcher = GetPrivateProfileInt(szSectionBuffer,szKeyClose,0,TEXT("server/exe.ini"));
		config.GiveElementPath = GetPrivateProfileInt(szSectionBuffer,szKeyGiveElementPath,0,TEXT("server/exe.ini"));
		GetPrivateProfileString(szSectionBuffer,szKeyParam,szDefaultName,szNameBuffer,MAX_PATH,TEXT("server/exe.ini"));
		config.ExecuteByCheckVersion = GetPrivateProfileInt(szSectionBuffer,szKeyCheckVersion,0,TEXT("server/exe.ini"));
		if (_tcscmp(szNameBuffer,szDefaultName) != 0) 
		{
			_tcslwr(szNameBuffer);
			config.Param = szNameBuffer;
		}
		config.HintInStringtab = GetPrivateProfileInt(szSectionBuffer,szKeyHintStringtab,-1,TEXT("server/exe.ini"));
		config.Updated = false;
		Exes.push_back(config);
		i++;
	} while (true);
}

DWORD ExecuteOther(ExeConfig& config)
{
	CString ElementPath(TEXT("../element/"));
	CString FilePath;
	CString FileName;
	DWORD ret = 0;
	if (config.ExecuteBeforeClosePatcher)
	{
		// 拷贝到temp目录
		ElementPath += config.FileName; 
		TCHAR TempPath[MAX_PATH];
		GetTempPath(MAX_PATH,TempPath);
		FileName += TempPath;
		FilePath += TempPath;
		FileName += config.FileName;
		UCopyFile(ElementPath,FileName,FALSE);
	}
	else
	{
		FileName = ElementPath + config.FileName;
	}

	CString	param = config.GiveElementPath ? fnInstallPath : TEXT("");
	param += CString(TEXT(" ")) + config.Param;

	if (PathFileExists(FileName))
	{
		WriteElementLog(TEXT("exe file exed!"));
		WriteElementLog(NULL);
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		LPWSTR lpwstr = new TCHAR[param.GetLength() + 1];
		_tcscpy(lpwstr, param);
		if (CreateProcess(FileName, lpwstr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			ret = pi.dwProcessId;
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		delete lpwstr;
	}
	else
	{
		WriteElementLog(TEXT("exe file not found!"));
		WriteElementLog(NULL);
	}
	return ret;

}

bool ExeBeforClose()
{
	bool ret = false;
	DWORD dwPID = 0;
	for (int i = 0;i < Exes.size();++i)
	{
		if (Exes[i].ExecuteEverytime && Exes[i].ExecuteBeforeClosePatcher)
		{
			dwPID = ExecuteOther(Exes[i]);
			ret = dwPID != 0;
		}
		while (dwPID)
		{
			Sleep(100);
			if (FindProcess(dwPID)) break;
		}
	}
	return ret;
}
bool GetUnityWebPlayerVersion(char* version, int versionLength)
{
	HKEY keyHandle		= NULL;
	const bool isOpend	= ERROR_SUCCESS == RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Unity\\WebPlayer", 0, KEY_READ, &keyHandle);
	if (!isOpend)
	{
		return false;
	}

	const int bufferLength	= 256;
	DWORD usedLength = 256;
	char pathBuffer[bufferLength];
	const int retValue	= RegQueryValueExA(keyHandle, "Directory", NULL, NULL, (LPBYTE)pathBuffer, &usedLength);
	RegCloseKey(keyHandle);
	if (retValue != 0)
	{
		return false;
	}

	strcat(pathBuffer, "\\loader\\info.plist");
	FILE* fin	= fopen(pathBuffer, "r");
	if (NULL == fin)
	{
		return false;
	}

	const int passCount	= 4;
	char line[bufferLength];
	for (int i= 0; i< passCount; ++i)
	{
		fgets(line, bufferLength, fin);
	}

	const char* format	= "\t<string>%[1-9a-z.]</string>\n";
	memset(version, 0, versionLength);
	fscanf(fin, format, version);
	fclose(fin);
	return true;
}
bool ExeCheckVersion(CString& strName)
{
	bool ret(false);
	if (strName == TEXT("unitywebplayerdevelopment.exe")) {
		char unity_version[16] = {0};
		const bool bHasUnity = GetUnityWebPlayerVersion(unity_version, sizeof unity_version);
		ret = !bHasUnity || strcmp(unity_version,"3.5.7") < 0;
	}
	return ret;
}

void CheckExeVersion()
{
	for (int i = 0;i < Exes.size();++i) {
		if (Exes[i].ExecuteByCheckVersion) {
			if (ExeCheckVersion(Exes[i].FileName))
				Exes[i].Updated = true;
		}
	}
}


void ExePerUpdate()
{
	CheckExeVersion();
	for (int i = 0;i < Exes.size();++i)
	{
		if (!Exes[i].ExecuteEverytime && Exes[i].Updated)
		{
			DWORD dwPID = 0;
			if (Exes[i].HintInStringtab != -1 && !bSilentUpdate)
			{
				Command(pList,TEXT("Name=ShowWindow,Window=Choose"));
				Command(pList,TEXT("Name=SetText,Item=Tip1,Text="));
				Command(pList,TEXT("Name=SetText,Item=Tip2,Text=")+strTable[Exes[i].HintInStringtab]+Exes[i].FileName);
				Command(pList,TEXT("Name=SetText,Item=Tip3,Text=")+strTable[90]);

				Command(pList,TEXT("Name=SetText,Item=Yes,Text=")+strTable[56]);
				Command(pList,TEXT("Name=SetText,Item=No,Text=")+strTable[57]);
				nSelect=0;
				cThread->SuspendThread();
			}
			else nSelect = 1;
			if (nSelect == 1)
			{
				dwPID = ExecuteOther(Exes[i]);
			}
			while (dwPID)
			{
				Sleep(100);
				if (FindProcess(dwPID)) break;
			}
		}
	}
}
BOOL CALLBACK UpdateFunc(FuncType InterFaceFunc,LPCTSTR lName,LPCTSTR lParam,void *vp)
{
	if(fnLocalUpdatePack!="")
		bQuickUpdate=true;
	if(bInit)
	{
		InitializeCriticalSection(&ServerSection);
		InitializeCriticalSection(&ServerConnectingTimeMapSection);
		g_EventThreadPrepareToDie.SetEvent();
		bStopThread=FALSE;
		bInit=FALSE;
		WriteElementLog(TEXT("====================================================================="),TRUE);
		WriteElementLog(NULL);
		WriteElementLog(TEXT("开始"));
		WriteElementLog(NULL);
		WriteLauncherLog(TEXT("====================================================================="),TRUE);
		WriteLauncherLog(NULL);
		WriteLauncherLog(TEXT("开始"));
		WriteLauncherLog(NULL);
		InitExeIniFile();
	}
	BOOL b=FALSE;
 	CallFunc=InterFaceFunc;
	pList=vp;
	XMLLabelAttribute *xAttrib=String2Attirb(lParam);
	if(_tcscmp(lName,TEXT("SaveUpdateSetting"))==0)
	{
		GetServer();
		if(bWaitingServer)
		{
			nSelect=1;
			bWaitingServer=false;
			cThread->ResumeThread();
		}
		b=TRUE;
	}
	if(_tcscmp(lName,TEXT("CancelUpdateSetting"))==0)
	{
		if(bWaitingServer)
		{
			nSelect=-1;
			bWaitingServer=false;
			cThread->ResumeThread();
		}
		b=TRUE;
	}
	if(_tcscmp(lName,TEXT("ClickYes"))==0)
	{
		nSelect=1;
		cThread->ResumeThread();
		b=TRUE;
	}
	if(_tcscmp(lName,TEXT("ClickNo"))==0)
	{
		nSelect=-1;
		cThread->ResumeThread();
		b=TRUE;
	}
	if(_tcscmp(lName,TEXT("Update"))==0)
	{
		CString c=xAttrib->GetAttribute(TEXT("Text"));
		if(bCanPlay)
		{
			Command(pList,TEXT("Name=SetEnable,Item=Play,Enable=false"));
			Command(pList,TEXT("Name=SetEnable,Item=FullCheck,Enable=false"));
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack,Enable=false"));
			// update for north American PerfectWorld Patcher [Yongdong,2010-1-22]
			Command(pList,TEXT("Name=SetEnable,Item=CleanPack2,Enable=false"));

			Play();
		}
		else
			BeginThread(Update, cThread);
		b=TRUE;
	}
	if(_tcscmp(lName,TEXT("UpdateServer"))==0)
	{
		UpdateServer(xAttrib);
		b=TRUE;
	}
	if(_tcscmp(lName,TEXT("FullCheck"))==0)
	{
		BeginThread(FullCheck, cThread);
		b=TRUE;
	}
	if(_tcscmp(lName,TEXT("StopThread"))==0)
	{
		EnterCriticalSection(&ThreadSection);
		bStopThread=TRUE;
		LeaveCriticalSection(&ThreadSection);
		while(bWritingFile)
			Sleep(10);
		WaitForSingleObject(g_EventThreadPrepareToDie, INFINITE);
		if (!bSilentUpdate) {
			CallFunc(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[125]);
			CallFunc(pList,TEXT("Name=SetText,Item=LoadFileName,Text=")+strTable[113]);
		}
		PackFinalize();
		if(bUpdating)
			SaveUpdateState(iNewVersion,iLineNumber,iFinishedOffset,iFileSize);
		FILE *fLog;
		if((fLog=OpenFile(fnElementLogFile,TEXT("ab")))!=NULL)
		{
			SYSTEMTIME sTime;
			GetLocalTime(&sTime);
			TCHAR s[400];
			_stprintf(s,TEXT("%d/%d/%d %02d:%02d:%02d"),
					sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
			_fputts(s,fLog);
			_fputts(strTable[126],fLog);
			_fputts(TEXT("\r\n"),fLog);
			fclose(fLog);
		}
		PATCH::DownloadManager::Release();
		PackUpdater::Release();
		SepFileUpdater::Release();
		//DelSpeedTestIndexInfo();
		b=TRUE;
	}
	if(_tcscmp(lName,TEXT("LoadVideoSetting"))==0)
	{
		b=TRUE;
		LoadVideoSetting();
	}
	if(_tcscmp(lName,TEXT("SaveVideoSetting"))==0)
	{
		b=TRUE;
		SaveVideoSetting();
	}
	if(_tcscmp(lName,TEXT("SuggestSetting"))==0)
	{
		b=TRUE;
		SuggestSetting();
	}
	if(_tcscmp(lName,TEXT("GuildFlag"))==0)
	{
		b=TRUE;
		//BeginThread(FlagUpdate, cThread);
		
	}
	if(_tcscmp(lName,TEXT("CalcDisplayMode"))==0)
	{
		b=TRUE;
		CalcDisplayMode();
	}
	if(_tcscmp(lName,TEXT("CleanUpAllPack"))==0)
	{
		b=TRUE;
		GetLocalPackFile();
//		cThread=AfxBeginThread(CleanUpAllPack,NULL);
	}
	xAttrib->Release();
	delete xAttrib;
	return b;
}
