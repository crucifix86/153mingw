// LauncherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Launcher.h"
#include "LauncherDlg.h"
#include "EC_HttpGet.h"
#include "EC_MD5Hash.h"
#include "EC_Signature.h"
#include "Base64.h"
#include "packdll.h"
#include "AWScriptFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD ReconnectTime=10000;
DWORD TotalTimeout=20000;
const int CodePage=936;
const int FileSizeOnDraw=8192;
const int NumbersOfLogFile=6;
const int MaxLogFileSize=300000;

const CString fnServerConfigFile=TEXT("../config/patcher/update.cfg");
CString fnServer=TEXT("http://patch.world2.cn/CPW/");
int pid=1;

CString fnPIDServer=fnServer+TEXT("info/pid");
const CString fnPIDFile=TEXT("../config/pid.sw");

const CString fnErrorLog=TEXT("../config/patcher/fileerr.log");
const CString fnErrorLog1=TEXT("../config/patcher/fileerr.001");
const CString DownloadTempFile=TEXT("../config/patcher/tempfile.dat");

CString fnPatcherServerName=fnServer+TEXT("patcher/");
CString fnPatcherServerPath=fnPatcherServerName+TEXT("patcher/");
const CString fnPatcherExe=TEXT("patcher.exe");
const char fnPatcherExeProcess[]="patcher.exe";
const CString fnConfig=TEXT("../config/patcher/patcher.cfg");
const CString fnOldVersion=TEXT("../config/patcher/version.sw");
const CString fnNewVersion=TEXT("../config/patcher/newver.sw");
const CString fnUpdateList=TEXT("../config/patcher/inc.sw");
const CString fnUpdateState=TEXT("../config/patcher/ustate.sw");
const CString fnFullCheckList=TEXT("../config/patcher/FullList.sw");
const CString fnFullListVersion=TEXT("../config/patcher/Listver.sw");
const CString fnTempPath=TEXT("../config/patcher/temp/");
const CString fnCheckPath=TEXT("../config/patcher/Check/");
const CString fnPath=TEXT("../patcher/");

const CString fnPatcherLogName=TEXT("../config/patcher/update.");
const CString fnPatcherLogFile=fnPatcherLogName+TEXT("log");

typedef int(* callback_t)(int status, PATCH::Downloader* worker);

bool bFileDown;
BOOL bError;
Base64Code base64;
CLauncherDlg *dlg;
int	iUpdatedLine;
int	iFinishedOffset;
int	iTotalLine;
int	iLineNumber;
int iFileSize;
int iFileOffset;
int iNewVersion;
int iOldVersion;
int iUpdatingVersion;
BOOL bEqual;
BOOL bResume;
BOOL bStopThread;
BOOL bWritingFile;
CWinThread *Thread;
PATCH::MD5Hash md5hash;
char md5[50];
CString strTable[200];
bool bAutoClose = false;

typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
extern MYFUNC MyUpdateLayeredWindow;

BOOL bSilentUpdate = FALSE;
enum
{
	OK,								//无错误
	PATCHER_HAS_BEEN_RUNNING,		//patcher已经运行
	PATCHER_NEED_UPDATED,			//patcher需要更新
	WRONG_VERSION_PATCH,			//补丁包版本错误
	PATCH_VALIDATION_FAILURE,		//补丁包校验失败
	UNKNOWN_ERROR,					//其他错误
};
enum
{
	UPDATE_ERROR,					// LPARAM = error code
	PROGRESS,						// LPARAM = 0 ~ 100
	COMPLETE,						// LPARAM = 0
	PATCHER_UPDATE_COMPLETE,		// LPARAM = 0
};
#define PW_PATCHER_MSG WM_USER + 2014
void NotifyOther(bool success, int progress = -1)
{
	if(bSilentUpdate && dwNotifyWindow) {
		HWND other = HWND(dwNotifyWindow);
		WORD event = progress == -1 ? (success ? PATCHER_UPDATE_COMPLETE : UPDATE_ERROR) : PROGRESS;
		DWORD wparam = MAKEWPARAM(event, wNotifyEventID);
		DWORD lparam = progress == -1 ? (success ? OK : UNKNOWN_ERROR) : progress;
		::PostMessage(other, PW_PATCHER_MSG, wparam, lparam);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CLauncherDlg dialog

CLauncherDlg::CLauncherDlg(BOOL bFullCheck,BOOL bUpdate,CWnd* pParent /*=NULL*/)
	: CDialog(CLauncherDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLauncherDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_bFullCheck=bFullCheck;
	m_bUpdate=bUpdate;
	dlg=this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bmp = Gdiplus::Bitmap::FromFile(TEXT("launcher.png"));
	m_Filled1 = Gdiplus::Bitmap::FromFile(TEXT("filled1.png"));
	m_Filled2 = Gdiplus::Bitmap::FromFile(TEXT("filled2.png"));
	m_Unfilled = Gdiplus::Bitmap::FromResource(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_UNFILLED));

	InitializeCriticalSection(&m_csGDIPlus);

	m_iTotalPos=0;
	m_iFilePos=0;
	m_cText="";
}

CLauncherDlg::~CLauncherDlg()
{
//	Gdiplus::DllExports::GdipFree(m_bmp);
//	Gdiplus::DllExports::GdipFree(m_Filled1);
//	Gdiplus::DllExports::GdipFree(m_Filled2);
//	Gdiplus::DllExports::GdipFree(m_Unfilled);
//	Gdiplus::DllExports::GdipFree(m_Font);
//	Gdiplus::DllExports::GdipFree(m_FileFont);

	DeleteCriticalSection(&m_csGDIPlus);
}

void CLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLauncherDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLauncherDlg, CDialog)
	//{{AFX_MSG_MAP(CLauncherDlg)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONUP()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLauncherDlg message handlers

void ThreadLock()
{
	if(bStopThread)
		ExitThread(0);
	bWritingFile=TRUE;
}

void ThreadUnLock()
{
	bWritingFile=FALSE;
	if(bStopThread)
		ExitThread(0);
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

void UMoveFile(LPCTSTR src,LPCTSTR des)
{
	SetFileAttributes(src, FILE_ATTRIBUTE_NORMAL);
	SetFileAttributes(des, FILE_ATTRIBUTE_NORMAL);
	if(!MoveFile(src,des))
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
			_stprintf(s,strTable[0],src,des);
			_fputts(s,f);
			_fputts(TEXT("\r\n"),f);
			fclose(f);
		}
		ThreadUnLock();
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

void GetServer()
{
	FILE *f;
	TCHAR s[400];
	fnPatcherServerName=fnServer+TEXT("patcher/");
	fnPIDServer=fnServer+TEXT("info/pid");
	if((f=OpenFile(fnServerConfigFile,TEXT("rb")))!=NULL)
	{
		CString servername="";
		TCHAR a[200],b[200];
		_fgettc(f);
		while(_fgetts(s,400,f)!=NULL)
		{
			_stscanf(s,TEXT("%s %s"),a,b);
			if(_tcscmp(a,TEXT("UpdateServerSelect"))==0)
				servername=b;
		}
		fclose(f);
		if(servername!=""&&(f=OpenFile(TEXT("../patcher/server/updateserver.txt"),TEXT("rb")))!=NULL)
		{
			TCHAR s[400];
			TCHAR c;
			_ftscanf(f,TEXT("%c"),&c);
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
				TCHAR cServer[300];
				_tcsncpy(cServer,s+j,i-j);
				cServer[i-j]=TEXT('\0');
				fnServer=cServer;
				fnPatcherServerName=fnServer+TEXT("patcher/");
				fnPIDServer=fnServer+TEXT("info/pid");
				if(servername==cName)
				{
					break;
				}
			}
			fclose(f);
		}
	}
	fnPatcherServerPath=fnPatcherServerName+TEXT("patcher/");
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
		UINT iReadSize=0;
		dlg->SetFileProgress(0);
		do
		{
			iRead=fread(buf,sizeof(char),FileSizeOnDraw,f);
			m.Update(buf,iRead);
			iReadSize+=iRead;
			dlg->SetFileProgress((UINT)((double)iReadSize/iSize*100));
		}while(iRead==FileSizeOnDraw);
		fclose(f);
		dlg->SetFileProgress(100);
	}
	UINT i=50;
	m.Final();
	m.GetString(md5,i);
	return TRUE;
}

BOOL CalMemMd5(unsigned char *buf,int size,char *md5)
{
	dlg->SetFileProgress(0);
	PATCH::MD5Hash m;
	int pos=0;
	while(pos+FileSizeOnDraw<size)
	{
		m.Update((char*)buf+pos,FileSizeOnDraw);
		pos+=FileSizeOnDraw;
		dlg->SetFileProgress((UINT)((double)pos/size*100));
	}
	if(pos<size)
		m.Update((char*)buf+pos,size-pos);
	m.Final();
	UINT i=50;
	m.GetString(md5,i);
	dlg->SetFileProgress(100);
	return TRUE;
}

CString cPlog="";

void WritePatcherLog(LPCTSTR c,BOOL reWrite=FALSE)
{
	FILE *fLog;
	if(reWrite)
	{
		int size=GetFileSize(fnPatcherLogFile);
		if(size==-1||size>=MaxLogFileSize)
		{
			for(int i=NumbersOfLogFile;i>0;i--)
			{
				TCHAR t1[10];
				TCHAR t2[10];
				_stprintf(t1,TEXT("%03d"),i);
				_stprintf(t2,TEXT("%03d"),i-1);
				if(i!=1)
					UCopyFile(fnPatcherLogName+t2,fnPatcherLogName+t1,FALSE);
				else
					UCopyFile(fnPatcherLogFile,fnPatcherLogName+t1,FALSE);
			}
			UDeleteFile(fnPatcherLogFile);
			ThreadLock();
			bWritingFile=TRUE;
			if((fLog=OpenFile(fnPatcherLogFile,TEXT("wb")))!=NULL)
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
		bWritingFile=TRUE;
		if((fLog=OpenFile(fnPatcherLogFile,TEXT("rb")))==NULL)
		{
			if((fLog=OpenFile(fnPatcherLogFile,TEXT("wb")))!=NULL)
			{
				_fputtc(0xfeff,fLog);
				fclose(fLog);
			}
		}
		else
			fclose(fLog);
		ThreadLock();
		bWritingFile=TRUE;
		if((fLog=OpenFile(fnPatcherLogFile,TEXT("ab")))!=NULL)
		{
			SYSTEMTIME sTime;
			GetLocalTime(&sTime);
			TCHAR s[400];
			_stprintf(s,TEXT("%d/%d/%d %02d:%02d:%02d"),
					sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
			_fputts(s,fLog);
			_fputts(cPlog,fLog);
			_fputts(TEXT("\r\n"),fLog);
			fclose(fLog);
		}
		ThreadUnLock();
		cPlog="";
	}
	else
		cPlog+=c;
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
			dlg->SetFileText(ucode);
#else
			int o;
			o=WideCharToMultiByte(CodePage,0,ucode,-1,p,400,NULL,NULL);
			if(o>0)
			{
				o--;
				_tcscpy(desName+k,p);
				k+=o;
				dlg->SetFileText(p);
			}
#endif
		}
	}
	desName[k]=TEXT('\0');
	return TRUE;
}

int FullCheckCallback(int status,PATCH::Downloader* worker)
{
	TCHAR st[200];
	int errcode;
	switch(status)
	{
	case PATCH::DOWNLOAD_START:
		iFileSize=worker->GetSize();
		dlg->SetFileProgress(0);
		bEqual=FALSE;
		break;
	case PATCH::BLOCK_DONE:
		iFinishedOffset=worker->GetOffset();
		dlg->SetFileProgress((int)((double)iFinishedOffset/iFileSize*100));
		break;
	case PATCH::FILE_DONE:
		dlg->SetFileProgress(100);
//		md5hash=worker->GetHash();
//		bEqual=md5hash.Equal(md5);
		bFileDown=true;
		break;
	case PATCH::NET_ERROR:
		bFileDown=false;
		errcode=worker->GetError();
		_stprintf(st,TEXT("%d"),errcode);
		WritePatcherLog(strTable[2]);
		WritePatcherLog(st);
		WritePatcherLog(NULL);
		break;
	case PATCH::RET_ERROR:
		bFileDown=false;
		int err=worker->GetError();
		switch(err)
		{
		case PATCH::ERR_URL_PARSE:
			WritePatcherLog(strTable[3]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_GET_ADDR:
			WritePatcherLog(strTable[4]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_OVERFLOW:
			WritePatcherLog(strTable[5]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_CREATE_FILE:
			WritePatcherLog(strTable[6]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_REQ_DENIED:
			WritePatcherLog(strTable[7]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_CONNECTION:
			WritePatcherLog(strTable[8]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_UNKNOWN:
			WritePatcherLog(strTable[9]);
			WritePatcherLog(NULL);
			break;
		default:
			WritePatcherLog(strTable[10]);
			WritePatcherLog(NULL);
			break;
		}
		break;
	}
	return 0;
}

BOOL FileFullCheck()
{
	int iTotalLine=0;
	int iLineNumber=0;
	int iTotalFile=0;
	FILE *fStream;
	char s[400];
	char fn[350];
	TCHAR path[400];

	UDeleteFile(fnUpdateState);
	if(!PATCH::Signature::GetInstance()->Verify(fnFullCheckList))
	{
		WritePatcherLog(strTable[11]);
		WritePatcherLog(NULL);
		dlg->SetText(strTable[12]);
		dlg->SetFileText(TEXT(""));
		Sleep(300);
		return FALSE;
	}
	if((fStream=OpenFile(fnFullCheckList,TEXT("r")))==NULL)
	{
		WritePatcherLog(strTable[11]);
		WritePatcherLog(NULL);
		dlg->SetText(strTable[12]);
		dlg->SetFileText(TEXT(""));
		Sleep(300);
		return FALSE;
	}
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
		return TRUE;
	}
	BOOL *b=new BOOL[iTotalLine];
	fseek(fStream,0,SEEK_SET);
	MakeDir(fnCheckPath,_tcsclen(fnCheckPath));
	dlg->SetTotalProgress(0);
	dlg->SetText(strTable[13]);
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
		WritePatcherLog(strTable[14]);
		WritePatcherLog(path);
		WritePatcherLog(NULL);
		dlg->SetText(strTable[13]);
		char md[50];
		DWORD size;
		unsigned char *fbuf=NULL;
		ThreadLock();
		CalcCompressFile(fnPath+path,size,fbuf);
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
		iFinishedOffset=0;
		b[iLineNumber]=FALSE;
		DWORD starttime=::GetTickCount();
		DWORD lasttime=starttime;
		DWORD nowtime=starttime;
		while(!bEqual)
		{
			WritePatcherLog(strTable[15]);
			WritePatcherLog(NULL);
			if(!bEqual&&i>=1)
			{
				do{
					Sleep(200);
					ThreadUnLock();
					nowtime=::GetTickCount();
				}while(DWORD(nowtime-lasttime)<=ReconnectTime);
				lasttime=nowtime;
				if(DWORD(nowtime-starttime)>=TotalTimeout)
					return FALSE;
			}
			i++;
			dlg->SetText(strTable[16]);
			MakeDir(fnCheckPath+path,_tcsclen(fnCheckPath+path));
			WritePatcherLog(strTable[17]);
			WritePatcherLog(path);
			WritePatcherLog(NULL);
			bFileDown=false;
			PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(fnPatcherServerPath+fn,DownloadTempFile,FullCheckCallback,iFinishedOffset);
			if(bFileDown)
			{
				CalFileMd5(DownloadTempFile,md);
				if(strcmp(md,md5)==0)
					bEqual=TRUE;
				else
					bEqual=FALSE;
				if(bEqual)
					if(!DecompressFile(DownloadTempFile,fnCheckPath+path))
					{
						dlg->SetText(strTable[18]);
						WritePatcherLog(strTable[18]);
						WritePatcherLog(NULL);
						bEqual=FALSE;
					}
				iFinishedOffset=0;
			}
			else
				bEqual=FALSE;
			b[iLineNumber]=TRUE;
		}
		if(b[iLineNumber])
			iTotalFile++;
		iLineNumber++;
		dlg->SetTotalProgress(iLineNumber*100/iTotalLine);
	}while(TRUE);
	if(iTotalFile>0)
	{
		fseek(fStream,0,SEEK_SET);
		WritePatcherLog(strTable[19]);
		WritePatcherLog(NULL);
		dlg->SetText(strTable[20]);
		dlg->SetTotalProgress(0);
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
				WritePatcherLog(strTable[21]);
				WritePatcherLog(path);
				WritePatcherLog(NULL);
				MakeDir(fnPath+path,_tcsclen(fnPath+path));
				ThreadLock();
				UDeleteFile(fnPath+path);
				UMoveFile(fnCheckPath+path,fnPath+path);
				ThreadUnLock();
				DeleteDir(fnCheckPath+path,_tcsclen(fnCheckPath+path));
				i++;
				dlg->SetTotalProgress(i*100/iTotalFile);
			}
			iLineNumber++;
		}while(TRUE);
	}
	fclose(fStream);
	dlg->SetTotalProgress(100);
	delete b;
	if((fStream=OpenFile(fnOldVersion,TEXT("w")))!=NULL)
	{
		_ftprintf(fStream,TEXT("%d\n"),iNewVersion);
		fclose(fStream);
	}
	UDeleteFile(DownloadTempFile);
	DeleteDir(fnCheckPath,_tcsclen(fnCheckPath));
	dlg->SetFileText(TEXT(""));
	return TRUE;
}

BOOL GetFullList()
{
	MakeDir(fnNewVersion,_tcsclen(fnNewVersion));
	FILE *fStream;
	PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(fnPatcherServerName+TEXT("version"),fnNewVersion,NULL,0);
	if((fStream=OpenFile(fnNewVersion,TEXT("r")))!=NULL)
	{
		fscanf(fStream,"%d",&iNewVersion);
		fclose(fStream);
		UDeleteFile(fnNewVersion);
	}
	if(iNewVersion==0)
		return FALSE;
//	int iListVersion=0;
//	if((fStream=OpenFile(fnFullListVersion,TEXT("r")))!=NULL)
//	{
//		fscanf(fStream,"%d",&iListVersion);
//		fclose(fStream);
//	}
//	BOOL b=FALSE;
//	if((fStream=OpenFile(fnFullCheckList,TEXT("r")))==NULL)
//		b=TRUE;
//	else
//		fclose(fStream);
//	if(b||iListVersion!=iNewVersion)
//	{
		WritePatcherLog(strTable[22]);
		WritePatcherLog(NULL);
		dlg->SetText(strTable[23]);
		UDeleteFile(fnFullCheckList);
		PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(fnPatcherServerName+TEXT("files.md5"),fnFullCheckList,NULL,0);
		if((fStream=OpenFile(fnFullListVersion,TEXT("w")))!=NULL)
		{
			fprintf(fStream,"%d\n",iNewVersion);
			fclose(fStream);
		}
//	}
	return TRUE;
}

UINT FullCheck(LPVOID pParam)
{
	bStopThread=FALSE;
	ThreadUnLock();
	dlg->SetText(strTable[24]);
	WritePatcherLog(strTable[24]);
	WritePatcherLog(NULL);
	dlg->SetTotalProgress(0);
	MakeDir(fnPIDFile,_tcslen(fnPIDFile));
	PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(fnPIDServer,fnPIDFile,NULL,0);
	FILE *fStream;
	int iNewPID=0;
	if((fStream=OpenFile(fnPIDFile,TEXT("r")))!=NULL)
	{
		fscanf(fStream,"%d",&iNewPID);
		fclose(fStream);
		UDeleteFile(fnPIDFile);
		if(iNewPID==0)
		{
			dlg->SetText(strTable[25]);
			WritePatcherLog(strTable[25]);
			WritePatcherLog(NULL);
			NotifyOther(false);
			return 0;
		}
		if(iNewPID!=pid)
		{
			dlg->SetText(strTable[27]);
			WritePatcherLog(strTable[27]);
			WritePatcherLog(NULL);
			NotifyOther(false);
			return 0;
		}
	}
	else
	{
		dlg->SetText(strTable[26]);
		WritePatcherLog(strTable[26]);
		WritePatcherLog(NULL);
		NotifyOther(false);
		return 0;
	}

	if(!GetFullList())
	{
		dlg->SetText(strTable[25]);
		WritePatcherLog(strTable[25]);
		WritePatcherLog(NULL);
		NotifyOther(false);
	}
	else
	{
		WritePatcherLog(strTable[28]);
		WritePatcherLog(NULL);
		if(!FileFullCheck())
		{
			dlg->SetText(strTable[29]);
			WritePatcherLog(strTable[29]);
			WritePatcherLog(NULL);
			NotifyOther(false);
		}
		else
		{
			dlg->SetText(strTable[30]);
			WritePatcherLog(strTable[30]);
			WritePatcherLog(NULL);
			NotifyOther(true);
		}
	}
	dlg->SetFileText(TEXT(""));
	return 0;
}

int UpdateCallback(int status,PATCH::Downloader* worker)
{
	TCHAR st[200];
	int errcode;
	FILE *f;
	switch(status)
	{
	case PATCH::DOWNLOAD_START:
		iFileSize=worker->GetSize();
		dlg->SetFileProgress(0);
		bEqual=FALSE;
		break;
	case PATCH::BLOCK_DONE:
		iFinishedOffset=worker->GetOffset();
		ThreadLock();
		if((f=OpenFile(fnUpdateState,TEXT("w")))!=NULL)
		{
			fprintf(f,"%d %d %d %d\r\n",iNewVersion,iLineNumber,0,iFileSize);
			fclose(f);
		}
		ThreadUnLock();
		dlg->SetFileProgress((int)((double)iFinishedOffset/iFileSize*100));
		break;
	case PATCH::FILE_DONE:
		dlg->SetFileProgress(100);
//		md5hash=worker->GetHash();
//		bEqual=md5hash.Equal(md5+1);
		bFileDown=true;
		break;
	case PATCH::NET_ERROR:
		bFileDown=false;
		errcode=worker->GetError();
		_stprintf(st,TEXT("%d"),errcode);
		WritePatcherLog(strTable[2]);
		WritePatcherLog(st);
		WritePatcherLog(NULL);
		break;
	case PATCH::RET_ERROR:
		bFileDown=false;
		int err=worker->GetError();
		switch(err)
		{
		case PATCH::ERR_URL_PARSE:
			WritePatcherLog(strTable[3]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_GET_ADDR:
			WritePatcherLog(strTable[4]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_OVERFLOW:
			WritePatcherLog(strTable[5]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_CREATE_FILE:
			WritePatcherLog(strTable[6]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_REQ_DENIED:
			WritePatcherLog(strTable[7]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_CONNECTION:
			WritePatcherLog(strTable[8]);
			WritePatcherLog(NULL);
			break;
		case PATCH::ERR_UNKNOWN:
			WritePatcherLog(strTable[9]);
			WritePatcherLog(NULL);
			break;
		default:
			WritePatcherLog(strTable[10]);
			WritePatcherLog(NULL);
			break;
		}
		break;
	}
	return 0;
}

UINT UpdateFile()
{
	iUpdatedLine=0;
	iFinishedOffset=0;
	iTotalLine=0;
	iLineNumber=0;
	FILE *fStream;
	char s[400];
	char fn[350];
	TCHAR path[400];
	BOOL b;
	FILE* f;

	f=OpenFile(fnUpdateState,TEXT("r"));
	if(f!=NULL)
	{
		if(fscanf(f,"%d",&iUpdatingVersion)==EOF)
			iUpdatingVersion=0;
		if(fscanf(f,"%d",&iUpdatedLine)==EOF)
			iUpdatedLine=0;
		if(fscanf(f,"%d",&iFinishedOffset)==EOF)
			iFinishedOffset=0;
		if(fscanf(f,"%d",&iFileSize)==EOF)
			iFileSize=0;
		fclose(f);
	}
	if((f=OpenFile(fnUpdateList,TEXT("r")))==NULL)
		b=TRUE;
	else
	{
		b=FALSE;
		fclose(f);
	}
	if(iUpdatingVersion!=iNewVersion||b)
	{
		iUpdatedLine=0;
		iFinishedOffset=0;
		TCHAR st[100];
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
		_stprintf(st,fnPatcherServerName+TEXT("v-%d.inc"),k);
		PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(st,fnUpdateList,NULL,0);
	}

	if(!PATCH::Signature::GetInstance()->Verify(fnUpdateList))
	{
		WritePatcherLog(strTable[11]);
		WritePatcherLog(NULL);
		dlg->SetText(strTable[12]);
		dlg->SetFileText(TEXT(""));
		Sleep(300);
		return 0;
	}
	if((fStream=OpenFile(fnUpdateList,TEXT("r")))==NULL)
	{
		WritePatcherLog(strTable[11]);
		WritePatcherLog(NULL);
		dlg->SetText(strTable[12]);
		dlg->SetFileText(TEXT(""));
		Sleep(300);
		return 0;
	}
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
		if((fStream=OpenFile(fnOldVersion,TEXT("w")))!=NULL)
		{
			_ftprintf(fStream,TEXT("%d\n"),iNewVersion);
			fclose(fStream);
		}
		return 1;
	}
	fseek(fStream,0,SEEK_SET);
	while(iLineNumber<iUpdatedLine)
	{
		if(fgets(s,400,fStream)==NULL)
			return 0;
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		iLineNumber++;
		sscanf(s,"%s %s",md5,fn);
		TransferPath(fn);
	}
	dlg->SetTotalProgress(iLineNumber*100/iTotalLine);
	dlg->SetText(strTable[16]);
	MakeDir(fnTempPath,_tcsclen(fnTempPath));
	char md[50];
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
		if(md5[0]!='-')
		{
			dlg->SetText(strTable[16]);
			DWORD size;
			unsigned char *fbuf=NULL;
			MakeDir(fnPath+path,_tcsclen(fnPath+path));
			ThreadLock();
			CalcCompressFile(fnPath+path,size,fbuf);
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
				WritePatcherLog(strTable[31]);
				WritePatcherLog(path);
				WritePatcherLog(NULL);
			}
			MakeDir(fnTempPath+path,_tcsclen(fnTempPath+path));
			if(iFinishedOffset!=0)
				bResume=TRUE;
			else
				bResume=FALSE;
			DWORD starttime=::GetTickCount();
			DWORD lasttime=starttime;
			DWORD nowtime=starttime;
			int i=0;
			while(!bEqual)
			{
				i++;
				if(bResume)
					WritePatcherLog(strTable[32]);
				else
					WritePatcherLog(strTable[33]);
				WritePatcherLog(path);
				WritePatcherLog(NULL);
				bFileDown=false;
				PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(fnPatcherServerPath+fn,DownloadTempFile,(callback_t)UpdateCallback,iFinishedOffset);
				if(bFileDown)
				{
					CalFileMd5(DownloadTempFile,md);
					if(strcmp(md,md5+1)==0)
						bEqual=TRUE;
					else
						bEqual=FALSE;
					if(bEqual)
						if(!DecompressFile(DownloadTempFile,fnTempPath+path))
						{
							dlg->SetText(strTable[18]);
							WritePatcherLog(strTable[18]);
							WritePatcherLog(NULL);
							bEqual=FALSE;
						}
					iFinishedOffset=0;
				}
				else
					bEqual=FALSE;
				if(!bEqual)
				{
					dlg->SetText(strTable[16]);
					WritePatcherLog(strTable[15]);
					WritePatcherLog(NULL);
				}
				if(!bEqual)
				{
					do{
						Sleep(200);
						ThreadUnLock();
						nowtime=::GetTickCount();
					}while(DWORD(nowtime-lasttime)<=ReconnectTime);
					lasttime=nowtime;
					if(DWORD(nowtime-starttime)>=TotalTimeout)
						return 0;
				}
			}
		}
		iLineNumber++;iFinishedOffset=0;
		ThreadLock();
		if((f=OpenFile(fnUpdateState,TEXT("w")))!=NULL)
		{
			fprintf(f,"%d %d %d %d\r\n",iNewVersion,iLineNumber,0,0);
			fclose(f);
		}
		ThreadUnLock();
		dlg->SetTotalProgress(iLineNumber*100/iTotalLine);
	}while(TRUE);
	fseek(fStream,0,SEEK_SET);
	dlg->SetText(strTable[34]);
	iLineNumber=0;
	dlg->SetTotalProgress(0);
	do
	{
		if(fgets(s,400,fStream)==NULL)
			break;
		sscanf(s,"%s %s",md5,fn);
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		TransferName(fn,path);
		if(md5[0]=='-')
		{
			dlg->SetText(strTable[35]);
			WritePatcherLog(strTable[36]);
			WritePatcherLog(path);
			WritePatcherLog(NULL);
			UDeleteFile(fnPath+path);
			DeleteDir(fnPath+path,_tcsclen(fnPath+path));
		}
		else
		{
			dlg->SetText(strTable[20]);
			MakeDir(fnPath+path,_tcsclen(fnPath+path));
			unsigned char *fbuf=NULL;
			DWORD size;
			ThreadLock();
			MakeDir(fnPath+path,_tcsclen(fnPath+path));
			CalcCompressFile(fnPath+path,size,fbuf);
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
			if(!bEqual)
			{
				WritePatcherLog(strTable[21]);
				WritePatcherLog(path);
				WritePatcherLog(NULL);
				MakeDir(fnPath+path,_tcsclen(fnPath+path));
				ThreadLock();
				UDeleteFile(fnPath+path);
				UMoveFile(fnTempPath+path,fnPath+path);
				ThreadUnLock();
				DeleteDir(fnTempPath+path,_tcsclen(fnTempPath+path));
			}
			else
			{
				UDeleteFile(fnTempPath+path);
				DeleteDir(fnTempPath+path,_tcsclen(fnTempPath+path));
			}
		}
		iLineNumber++;
		dlg->SetTotalProgress(iLineNumber*100/iTotalLine);
	}while(TRUE);
	fclose(fStream);
	dlg->SetTotalProgress(100);
	if((f=OpenFile(fnOldVersion,TEXT("w")))!=NULL)
	{
		_ftprintf(f,TEXT("%d\n"),iNewVersion);
		fclose(f);
	}
	UDeleteFile(fnUpdateState);
	UDeleteFile(fnUpdateList);
	dlg->SetFileText(TEXT(""));
	return 1;
}

UINT GetFileList()
{
	iNewVersion=0;
	iOldVersion=0;
	MakeDir(fnNewVersion,_tcsclen(fnNewVersion));

	FILE *fStream;
	PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(fnPatcherServerName+TEXT("version"),fnNewVersion,NULL,0);
	if((fStream=OpenFile(fnNewVersion,TEXT("r")))!=NULL)
	{
		fscanf(fStream,"%d",&iNewVersion);
		fclose(fStream);
		UDeleteFile(fnNewVersion);
	}
	if(iNewVersion==0)
		return 1;
	if((fStream=OpenFile(fnOldVersion,TEXT("r")))==NULL)
		return 2;
	else
	{
		fscanf(fStream,"%d",&iOldVersion);
		fclose(fStream);
		if(iOldVersion<1)
			return 2;
	}
	int p=iNewVersion-iOldVersion;
	if(p<0)
		return 2;
	if(p==0)
		return 0;
	return 3;
}

UINT Update(LPVOID pParam)
{
	bStopThread=FALSE;
	ThreadUnLock();
	dlg->SetText(strTable[37]);
	WritePatcherLog(strTable[37]);
	WritePatcherLog(NULL);
	PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(fnPIDServer,fnPIDFile,NULL,0);
	FILE *fStream;
	int iNewPID=0;
	if((fStream=OpenFile(fnPIDFile,TEXT("r")))!=NULL)
	{
		fscanf(fStream,"%d",&iNewPID);
		fclose(fStream);
		UDeleteFile(fnPIDFile);
		if(iNewPID==0)
		{
			dlg->SetText(strTable[25]);
			WritePatcherLog(strTable[25]);
			WritePatcherLog(NULL);
			NotifyOther(false);
			return 0;
		}
		if(iNewPID!=pid)
		{
			dlg->SetText(strTable[27]);
			WritePatcherLog(strTable[27]);
			WritePatcherLog(NULL);
			NotifyOther(false);
			return 0;
		}
	}
	else
	{
		dlg->SetText(strTable[26]);
		WritePatcherLog(strTable[26]);
		WritePatcherLog(NULL);
		NotifyOther(false);
		return 0;
	}

	int ret=GetFileList();
	switch(ret)
	{
	case 1:
		dlg->SetText(strTable[25]);
		WritePatcherLog(strTable[25]);
		WritePatcherLog(NULL);
		NotifyOther(false);
		Sleep(300);
		break;
	case 2:
		dlg->SetText(strTable[38]);
		dlg->SetFileText(strTable[39]);
		NotifyOther(false);
		Sleep(300);
		FullCheck(NULL);
		break;
	case 3:
		if(UpdateFile()==0)
		{
			dlg->SetText(strTable[40]);
			WritePatcherLog(strTable[40]);
			WritePatcherLog(NULL);
			NotifyOther(false);
			Sleep(300);
			break;
		} else NotifyOther(true);
	case 0:
		dlg->SetText(strTable[41]);
		WritePatcherLog(strTable[41]);
		WritePatcherLog(NULL);
		NotifyOther(true);
		Sleep(300);
		break;
	}
	dlg->SetFileText(TEXT(""));
	dlg->SetTotalProgress(100);
	dlg->SetFileProgress(100);
	return 0;
}

UINT Start(LPVOID pParam)
{
	pid = GetPrivateProfileIntA("Version", "pid", pid, "../patcher/server/pid.ini");
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
	WritePatcherLog(TEXT("====================================================================="),TRUE);
	WritePatcherLog(NULL);
	WritePatcherLog(strTable[42]);
	WritePatcherLog(NULL);
	dlg->SetText(TEXT(""));
	GetServer();
	HANDLE hMutex;
	if(dlg->m_bUpdate)
	{
		dlg->SetText(strTable[43]);
		Sleep(1000);
		int s=0;
		do
		{
			s++;
			if(s>20)
			{
				dlg->SetText(strTable[44]);
				dlg->SetFileText(strTable[45]);
				Sleep(500);
				dlg->SendMessage(WM_CLOSE);
				return 0;
			}
			hMutex = CreateMutex(NULL, FALSE, TEXT("PerfectWorldPatcherExist"));
			Sleep(300);
		} while(GetLastError()==ERROR_ALREADY_EXISTS);
		CloseHandle(hMutex);
	}
	else
	{
		Sleep(30);
		hMutex = CreateMutex(NULL, FALSE, TEXT("PerfectWorldPatcherExist"));
		if (GetLastError()==ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hMutex);
			WritePatcherLog(strTable[46],TRUE);
			WritePatcherLog(NULL);
			HWND hWnd=FindWindow(TEXT("PerfectWorldPatcherClass"),NULL);
			if(hWnd!=NULL)
			{
				SetForegroundWindow(hWnd);
				ShowWindow(hWnd,SW_SHOWNORMAL);
			}
			dlg->SendMessage(WM_CLOSE);
			return 0;
		}
		else
			CloseHandle(hMutex);
	}
	BOOL *b=(BOOL*)pParam;
	if(*b)
		FullCheck(NULL);
	else
		Update(NULL);
	Sleep(200);
	dlg->ShowWindow(SW_HIDE);
	TCHAR path[1000];
	GetCurrentDirectory(1000, path);
	_tcscat(path, TEXT("/"));
	_tcscat(path, fnPath+fnPatcherExe);
	CString cmdLine=::GetCommandLine();
	cmdLine += TEXT(" /update");
	if (!bSilentUpdate)
		ShellExecute(dlg->GetSafeHwnd(),TEXT("open"),path,cmdLine,fnPath.Left(fnPath.GetLength()-1),SW_SHOWNORMAL);
	Sleep(100);
	dlg->SendMessage(WM_CLOSE);
	return 0;
}

BOOL CLauncherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	ModifyStyle(WS_CAPTION, 0, SWP_FRAMECHANGED);

	CWnd *c=GetDesktopWindow();
	CRect cScreenRect;
	c->GetWindowRect(&cScreenRect);
	int x,y;
	x=(cScreenRect.Width()-m_bmp->GetWidth())/2;
	y=(cScreenRect.Height()-m_bmp->GetHeight())/2;
	SetWindowPos(this,x,y,m_bmp->GetWidth(),m_bmp->GetHeight(),0);

	int iHeight,iFontStyle;
	iHeight = 13;
	iFontStyle = FontStyleRegular;

	Font font(TEXT("宋体"),static_cast<float>(iHeight),iFontStyle,UnitPixel);
	if (font.IsAvailable())
	{
		m_FileFont = m_Font = font.Clone();
	}
	else
		m_FileFont = m_Font = NULL;

	FILE *f;
	if((f=OpenFile(fnConfig,TEXT("r")))!=NULL)
	{
		_ftscanf(f,TEXT("%s"),fnPatcherServerName);
		_ftscanf(f,TEXT("%s"),fnPatcherServerPath);
		fclose(f);
	}

	//设置窗口的EXSTYLE为0x80000,LayeredWindow
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	ImportStringTable(TEXT("stringtab.txt"));
	Thread=AfxBeginThread(Start, &m_bFullCheck);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLauncherDlg::UpdateView()
{
	if (bSilentUpdate) {
		ShowWindow(SW_HIDE);
		return;
	}
	m_pGraphics = NULL;
	//避免创建该窗口的线程和窗口线程同时调用GDI+的函数
	EnterCriticalSection(&m_csGDIPlus);
	SIZE sizeWindow = { m_bmp->GetWidth(), m_bmp->GetHeight()};
	
	HDC hDC = ::GetDC(m_hWnd);
	//先画到memoryDC上，使用这个memoryDC 更新hDC
	HDC hdcMemory = CreateCompatibleDC(hDC);
	
	
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
		m_pGraphics->DrawImage(m_bmp, 0, 0, sizeWindow.cx, sizeWindow.cy);
		
		m_pGraphics->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

		DrawText();
		DrawFileText();
		DrawTotalProgress();
		DrawFileProgress();

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

	LeaveCriticalSection(&m_csGDIPlus);
}
LRESULT CLauncherDlg::OnNcHitTest(CPoint point) 
{
	ScreenToClient(&point);
	if(point.x>=193&&point.x<=211&&point.y>=22&&point.y<40)
		return HTCLIENT;
	return HTCAPTION;
}

void CLauncherDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	SendMessage(WM_CLOSE);
}

BOOL CLauncherDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CLauncherDlg::SetText(LPCTSTR c)
{
	ThreadLock();
//	m_cText=TEXT("载入中...");
//	Sleep(100);
	m_cText=c;
	UpdateView();
	ThreadUnLock();
}

void CLauncherDlg::DrawText()
{
	if (!m_pGraphics)
		return;

	Gdiplus::StringFormat format;
	format.SetAlignment(StringAlignmentCenter);

	Color cText(254,120,30,0);
	SolidBrush brush(cText);

	RectF rect(54,153,184 - 54,167 - 153);

	m_pGraphics->DrawString(m_cText,-1,m_Font,rect,&format,&brush);
}

void CLauncherDlg::SetFileText(LPCTSTR c)
{
	ThreadLock();
	m_cFileText=c;
	UpdateView();
	ThreadUnLock();
}

void CLauncherDlg::DrawFileText()
{
	if (!m_pGraphics)
		return;
	
	Gdiplus::StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	
	Color cText(254,120,30,0);
	SolidBrush brush(cText);
	
	RectF rect(54,170,184 - 54,184 - 170);
	
	m_pGraphics->DrawString(m_cFileText,-1,m_Font,rect,&format,&brush);
}

void CLauncherDlg::SetTotalProgress(int i)
{
	ThreadLock();
	m_iTotalPos=i;
	UpdateView();
	NotifyOther(true, i);
	ThreadUnLock();
}

void CLauncherDlg::DrawTotalProgress()
{
	if (!m_pGraphics)
		return;
	
	int pos=m_iTotalPos*200/100;
	//m_pGraphics->DrawImage(m_Unfilled,pos + 54,125,130 - pos,10);
	m_pGraphics->DrawImage(m_Filled1, 55,130,pos,10);
}

void CLauncherDlg::SetFileProgress(int i)
{
	ThreadLock();
	m_iFilePos=i;
	UpdateView();
	ThreadUnLock();
}

void CLauncherDlg::DrawFileProgress()
{
	if (!m_pGraphics)
		return;
	
	int pos=m_iTotalPos*200/100;
	//m_pGraphics->DrawImage(m_Unfilled,pos + 54,138,130 - pos,10);
	m_pGraphics->DrawImage(m_Filled2,55,155,pos,10);
}

void CLauncherDlg::OnClose()
{
	bStopThread=TRUE;
	while(bWritingFile)
		Sleep(10);
	CDialog::OnClose();
}

bool CLauncherDlg::ImportStringTable(const TCHAR *pszFilename)
{
	FILE *fStream;
	if((fStream=OpenFile(pszFilename,TEXT("rb")))!=NULL)
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

	return true;
}

