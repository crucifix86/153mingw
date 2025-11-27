#include "StdAfx.h"
#include "PackUpdater.h"
#include "Update.h"
#include "packdll.h"
#include <afxmt.h>
extern CString fnElementPatchName;
extern CString fnServer;
extern CString fnPackServer;
extern CString fnLocalUpdatePack;
const extern CString fnElementOldVersion;
const extern CString DownloadTempFile2;

extern int iOldVersion;
extern int iOldVersion1;
extern int iNewVersion;
extern CWinThread *cThread;
extern bool bFileDown;
extern volatile float fFileProgress;
extern bool bP2SPDownload;
extern int iP2SPDownSpeed;
extern int iP2SPUpSpeed;
extern int pid;
extern void *pList;
extern CEvent g_EventThreadPrepareToDie;
extern BOOL bStopThread;

int iStopVersion = 0;

int GAME_ID = 3;

extern CRITICAL_SECTION ServerSection;
#define PACK_VERSION_FILE L"../config/element/version.txt"
#define PACK_FILE_NAME_PATTERN "ec_patch_%d-%d.cup"
#define PACK_FILE_PATH_PATTERN "../config/element/ec_patch_%d-%d.cup"
#define PACK_FILE_MD5_PATTERN "patches/ec_patch_%d-%d.cup.MD5"
#define PACK_FILE_URL_PATTERN "patches/ec_patch_%d-%d.cup"
#define PACK_SAVE_PATH L"../config/element"
#define HASH_STRING_LENGTH 33

template<> PackUpdater* Singleton<PackUpdater>::ms_Singleton = 0;

int Updater::m_iCurrentDownload = 0;
int Updater::m_iTotalDownload = 0;
int Updater::m_iUpdateInterval = 0;
int Updater::m_iLastTick = 0;
UINT64 Updater::m_u64LastFileSize = 0;
void Updater::SetDownloadParam(int iCurrent, int iTotal, int iUpdateInterval)
{
	m_iCurrentDownload = iCurrent; 
	m_iTotalDownload = iTotal;
	m_iUpdateInterval = iUpdateInterval;
	m_iLastTick = GetTickCount();
	m_u64LastFileSize = 0;
}
void Updater::ShowDownloadSpeed(UINT64 ComplateSize)
{
	if (m_iCurrentDownload == 0 || 
		m_iTotalDownload == 0 ||
		m_iUpdateInterval == 0 ||
		m_iLastTick == 0)
		return;
	int iCurrentTick = GetTickCount();
	static bool bShowSpeed = true;
	if (iCurrentTick - m_iLastTick > m_iUpdateInterval)
	{
		TCHAR szText[200];
		m_iLastTick = iCurrentTick;
		if (bShowSpeed)
		{
			bShowSpeed = false;
			UINT64 KBPerSecond(0);
			if (ComplateSize > m_u64LastFileSize) 
				KBPerSecond = (ComplateSize - m_u64LastFileSize) * 1000 / 1024 / m_iUpdateInterval;
			_stprintf(szText, TEXT("Name=SetText,Item=LoadFileName,Text=") + strTable[173], KBPerSecond);
			Command(pList,szText);		
		}
		else
		{
			_stprintf(szText, TEXT("Name=SetText,Item=LoadFileName,Text=%s%d/%d"),
				strTable[166], m_iCurrentDownload, m_iTotalDownload);
			Command(pList,szText);
			bShowSpeed = true;
		}
		m_u64LastFileSize = ComplateSize;
	}
}

PackUpdater::PackUpdater()
{
	m_hHandle = ::LoadLibrary(CONST_STRING_DLL_PATH);
	m_bP2SPStartSucc = false;
	m_bDownloadError = false;
	InitializeCriticalSection(&m_csMD5List);
}

PackUpdater::~PackUpdater()
{
	if (m_bP2SPStartSucc)
	{
		dll_p2spStop();
		FreeLibrary(m_hHandle);
		m_hHandle = NULL;
	}
}
void ResumeUpdateThread()
{
	while (!cThread->ResumeThread())
	{
		Sleep(100);
	}
}
UINT P2SPDownloadThread(LPVOID pParam)
{
	PackUpdater* pUpdater = (PackUpdater*)pParam;
	int iSize = pUpdater->m_MD5List.size();
	int iCurrentDownload = 1;
	g_EventThreadPrepareToDie.ResetEvent();
	TCHAR szText[200];
	for (PackUpdater::StringList::iterator iterMD5 = pUpdater->m_MD5List.begin();
		iterMD5 != pUpdater->m_MD5List.end();++iterMD5,++iCurrentDownload)
	{
		_stprintf(szText,TEXT("Name=SetText,Item=LoadFileName,Text=%s%d/%d"),
			strTable[166],iCurrentDownload,iSize);
		Command(pList,szText);
		Updater::SetDownloadParam(iCurrentDownload, iSize, 5000);
		if (iterMD5->second == PackUpdater::PACK_ALREADY_EXISTS)
		{
			continue;
		}
		if (pUpdater->dll_p2spAddDownload((iterMD5->first).c_str()))
		{
			WriteElementLog(TEXT("dll_p2spAddDownload succeed!"));
			WriteElementLog(NULL);
			if (pUpdater->dll_p2spStartDownload((iterMD5->first).c_str()))
			{
				WriteElementLog(TEXT("dll_p2spStartDownload succeed!"));
				WriteElementLog(NULL);
				strcpy(pUpdater->m_CurrentDownloadInfo.md5,(iterMD5->first).c_str());
				while (true)
				{
					Sleep(1000);
					if (bStopThread)
					{
						pUpdater->dll_p2spStopDownload((iterMD5->first).c_str());
						g_EventThreadPrepareToDie.SetEvent();
						return 0;
					}
					bool bRet = pUpdater->dll_p2spGetTaskInfomation(&(pUpdater->m_CurrentDownloadInfo));
					if (bRet)
					{
						Updater::ShowDownloadSpeed(pUpdater->m_CurrentDownloadInfo.complateSize);
						if (pUpdater->m_CurrentDownloadInfo.size != 0)
						{
							fFileProgress = (float) pUpdater->m_CurrentDownloadInfo.complateSize /
								pUpdater->m_CurrentDownloadInfo.size;
						}
						
						if (P2P_TASK_STATE_COMPLETE == pUpdater->m_CurrentDownloadInfo.state)
						{
							//pUpdater->DownloadSucced();
							EnterCriticalSection(&pUpdater->m_csMD5List);
							iterMD5->second = PackUpdater::PACK_ALREADY_EXISTS;
							LeaveCriticalSection(&pUpdater->m_csMD5List);
							break;
						}
						else if (P2P_TASK_STATE_ERROR == pUpdater->m_CurrentDownloadInfo.state)
						{
							WriteElementLog(TEXT("P2P_TASK_STATE_ERROR!"));
							WriteElementLog(NULL);
							AfxBeginThread(HTTPDownloadThread,pParam);
							g_EventThreadPrepareToDie.SetEvent();
							return -1;
						}
					} 
				}
			}
			else // 开始p2sp下载任务失败
			{
				WriteElementLog(TEXT("dll_p2spStartDownload failed!"));
				WriteElementLog(NULL);
				AfxBeginThread(HTTPDownloadThread,pParam);
				g_EventThreadPrepareToDie.SetEvent();
				return -1;
			}
		}
		else // 添加p2sp下载任务失败
		{
			WriteElementLog(TEXT("dll_p2spAddDownload failed!"));
			WriteElementLog(NULL);
			AfxBeginThread(HTTPDownloadThread,pParam);
			g_EventThreadPrepareToDie.SetEvent();
			return -1;
		}
		if (!pUpdater->IsUpdateFailed())
		{
			cThread->ResumeThread();
		}
	}
	g_EventThreadPrepareToDie.SetEvent();
	return 0;
}
UINT HTTPDownloadThread(LPVOID pParam)
{
	PackUpdater* pUpdater = (PackUpdater*)pParam;
	char strPackName[MAX_PATH] = {0};
	char strSaveName[MAX_PATH] = {0};
	char md5[HASH_STRING_LENGTH] = {0};
	TCHAR szText[200];
	int iCurrentDownload = 1;
	int iSize = pUpdater->m_MD5List.size();
	PackUpdater::StringList::iterator iterMD5 = pUpdater->m_MD5List.begin();
	for (PackUpdater::PairsList::iterator iter = pUpdater->m_DownloadList.begin();
		iter != pUpdater->m_DownloadList.end();++iter,++iterMD5,++iCurrentDownload)
	{
		_stprintf(szText,TEXT("Name=SetText,Item=LoadFileName,Text=%s%d/%d"),
			strTable[166],iCurrentDownload,iSize);
		Command(pList,szText);
		Updater::SetDownloadParam(iCurrentDownload, iSize, 5000);
		if (iterMD5->second == PackUpdater::PACK_ALREADY_EXISTS)
		{
			continue;
		}
		sprintf(strPackName,PACK_FILE_URL_PATTERN,iter->first,iter->second);
		sprintf(strSaveName,PACK_FILE_PATH_PATTERN,iter->first,iter->second);
		// 注意多线程下的这个变量，使用更新包更新时这个变量专门给下载线程使用，其他线程不能使用
		bFileDown = false;
		// 尝试两次下载，第一次从CDN下载链接(如果有的话)；第二次从更新服务器
		int i = fnPackServer.IsEmpty() ? 1 : 0;
		for (; i < 2 ; ++i)
		{
			bool bDownSucced(false);
			CString urlServer = 0 == i ? fnPackServer : fnServer;
		//	PATCH::DownloadManager::GetSingletonPtr()->GetByUrl(urlServer + strPackName,CString(strSaveName),
		//		(callback_t)UpdateCallback,0);
			DownloadFileByHttp(urlServer + strPackName,CString(strSaveName),(callback_t)UpdateCallback,0);
			if (bFileDown)
			{
				CalFileMd5(CString(strSaveName),md5);
				if (0 == stricmp(md5,(iterMD5->first).c_str()))
				{
					EnterCriticalSection(&pUpdater->m_csMD5List);
					iterMD5->second = PackUpdater::PACK_ALREADY_EXISTS;
					LeaveCriticalSection(&pUpdater->m_csMD5List);
					bDownSucced = true;
				}
				else if (0 == i) continue;
			}
			else if (0 == i) continue;
			if (bDownSucced) 
			{
				cThread->ResumeThread();
				break;
			}
			else
			{
				pUpdater->DownloadFailed(strPackName);
				if (iter == pUpdater->m_DownloadList.begin() || !pUpdater->IsUpdateFailed())
					ResumeUpdateThread();
				return -1;
			}
		}
	}
	return 0;
}
void PackUpdater::DownloadFailed(const char* name)
{
	m_bDownloadError = true;
	TCHAR strlog[200];
	_stprintf(strlog,TEXT("%s: %s"),strTable[62],CString(name));
	WriteElementLog(strlog);
	WriteElementLog(NULL);
}
void PackUpdater::SetUpdateFailed()
{
	StringList::iterator iterMD5 = m_MD5List.begin();
	EnterCriticalSection(&m_csMD5List);
	iterMD5->second = PACK_UPDATE_FAILED;
	LeaveCriticalSection(&m_csMD5List);
}
bool PackUpdater::IsUpdateFailed()
{
	bool ret = false;
	StringList::iterator iterMD5 = m_MD5List.begin();
	EnterCriticalSection(&m_csMD5List);
	ret = iterMD5->second;
	LeaveCriticalSection(&m_csMD5List);
	return ret == PACK_UPDATE_FAILED;
}
bool PackUpdater::DownloadMD5List()
{
	char strPackMD5Name[MAX_PATH] = {0};
	char md5[HASH_STRING_LENGTH] = {0};
	m_MD5List.clear();
	for (PairsList::iterator iter = m_DownloadList.begin();iter != m_DownloadList.end();++iter)
	{
		sprintf(strPackMD5Name,PACK_FILE_MD5_PATTERN,iter->first,iter->second);
		UDeleteFile(DownloadTempFile2);
		DownloadFileByHttp(fnServer + strPackMD5Name,DownloadTempFile2,NULL,0);
		FILE *fStream = OpenFile(DownloadTempFile2,TEXT("r"));
		if (fStream == NULL)
		{
			return false;
		}
		fscanf(fStream,"%s",md5);
		fclose(fStream);
		if (strlen(md5) != 0)
		{
			m_MD5List.push_back(make_pair(std::string(md5),PACK_NOT_EXISTS));
		}
		else
		{
			return false;
		}
	}
	UDeleteFile(DownloadTempFile2);
	return true;
}
// 如果第一个更新包已经下载完毕，返回true表示可以马上安装(暂时没有用到)
bool PackUpdater::CheckDownloadedFile()
{
	bool bCanInstallPack = false;
	char strPackName[MAX_PATH] = {0};
	char md5[HASH_STRING_LENGTH] = {0};
	PairsList::iterator iterPair = m_DownloadList.begin();
	StringList::iterator iterMD5 = m_MD5List.begin();
	for (; m_DownloadList.end() != iterPair; ++iterPair,++iterMD5)
	{
		sprintf(strPackName,PACK_FILE_PATH_PATTERN,iterPair->first,iterPair->second);
		FILE* fStream = OpenFile(CString(strPackName),TEXT("r"));
		if (NULL != fStream)
		{
			fclose(fStream);
			CalFileMd5(CString(strPackName),md5);
			if (0 == stricmp(md5,(iterMD5->first).c_str()))
			{
				iterMD5->second = PACK_ALREADY_EXISTS;
				if (m_DownloadList.begin() == iterPair)
				{
					bCanInstallPack = true;
				}
			}
		}
	}
	return bCanInstallPack;
}
bool PackUpdater::StartP2SPEngine()
{
	GAME_ID = _tstoi(strTable[172]);
	if (0 == GAME_ID)
	{
		WriteElementLog(TEXT("can't get valid game id for p2sp!"));
		WriteElementLog(NULL);
		return false;
	}
	bool ret = dll_p2spStart(GAME_ID);
	if (ret)
	{
		TCHAR path[MAX_PATH] = {0};
		GetCurrentDirectory(MAX_PATH, path);
		CString PackPath(path);
		int iPos = PackPath.ReverseFind(TEXT('\\'));
		PackPath = PackPath.Left(iPos);
		PackPath += TEXT("\\config\\element");

		SettingItemT setting;
		setting.mask = P2P_SETTING_ITEM_FILE_SAVE_PATH | P2P_SETTING_ITEM_MAX_DOWNLOAD_SPEED | P2P_SETTING_ITEM_MAX_UPLOAD_SPEED;
		lstrcpyW(setting.savePath, PackPath);
		setting.maxDownloadSpeed = static_cast<UINT>(iP2SPDownSpeed);
		setting.maxUploadSpeed = static_cast<UINT>(iP2SPUpSpeed);
		dll_p2spSetting(&setting);
		Sleep(5000);

	}

	return ret;
}
bool PackUpdater::BeginDownloadThread()
{
	// 下载更新包的MD5文件
	Command(pList,TEXT("Name=SetProgressPos,Item=FileProgress,Pos=0"));
	if (!bSilentUpdate)
		Command(pList,TEXT("Name=SetProgressPos,Item=TotalProgress,Pos=0"));
	if (!DownloadMD5List())
	{
		return false;
	}
	// 查看下载目录里面是否有已经下载的安装包
	CheckDownloadedFile();
	if (bP2SPDownload)
	{
		if (StartP2SPEngine())
		{
			WriteElementLog(TEXT("StartP2SPEngine succeed!"));
			WriteElementLog(NULL);
			m_bP2SPStartSucc = true;
			AfxBeginThread(P2SPDownloadThread,this);
		}
		else
		{
			WriteElementLog(TEXT("StartP2SPEngine failed!"));
			WriteElementLog(NULL);
			m_bP2SPStartSucc = false;
			AfxBeginThread(HTTPDownloadThread,this);
		}
	}
	else
	{
		m_bP2SPStartSucc = false;
		AfxBeginThread(HTTPDownloadThread,this);
	}

	return true;
}
// 启动一个线程用于下载，起到下载的同时安装前一个下载好的更新包的效果
bool PackUpdater::Update()
{
	// 需要下载哪些更新包
	if (!MakeDownloadList())
	{
		PackListError();
		return false;
	}
	if (!BeginDownloadThread())
	{
		MD5ListError();
		return false;
	}
	SetUpdateState(0);
	Command(pList,TEXT("Name=SetText,Item=UpdateState,Text=")+strTable[167]);
	char strPackName[100];
	int iSize = m_DownloadList.size();
	m_iCurrentPack = 0;
	bool bUpdateSucceed = true;
	StringList::iterator iterMD5 = m_MD5List.begin();
	for (PairsList::iterator iter = m_DownloadList.begin();iter != m_DownloadList.end();++iter,++iterMD5)
	{
		EnterCriticalSection(&m_csMD5List);
		bool bPackNotExists = (iterMD5->second == PACK_NOT_EXISTS); 
		LeaveCriticalSection(&m_csMD5List);
		if (bPackNotExists)
		{
			// 没有下载完毕则等待下载完毕后被唤醒
			cThread->SuspendThread();
		}
		// 被下载线程唤醒后发现下载出错
		if (m_bDownloadError)
		{
			PackDownloadError();
			fnLocalUpdatePack = "";
			return false;
		}
		int iError = 0;
		sprintf(strPackName,PACK_FILE_PATH_PATTERN,iter->first,iter->second);
		iError = UpdateFromPack(strPackName,iSize,m_iCurrentPack);
		if (iError)
		{
			DealWithUpdateError(iError);
			SetUpdateFailed();
			bUpdateSucceed = false;
			break;
		}
		else
			++m_iCurrentPack;
	}
	if (bUpdateSucceed)
	{
		TCHAR st[100];
		_stprintf(st,TEXT("Name=SetText,Item=ElementVersion,Text=")+strTable[77],iNewVersion);
		Command(pList,st);
		SetUpdateState(1);
	}
	fnLocalUpdatePack = "";
	CheckPack();
	return bUpdateSucceed;
}
/*
客户端首先取自己的本地版本，然后利用本地版本与上述版本对中的起始版本进行比较，产生如下处理分支：
1.	如果找到了对应的版本对，那么根据此版本对下载对应的更新包进行更新，更新后将此版本对的目标版本
写入自己的本地版本，再用新的本地版本作为起始版本进行更新，直到本地版本为最新版本。例如：本地版本
为822,那么首先找到822-831，利用此版本对下载ec_patch_822-831.cup更新包，安装后本地版本为831，还是
低于最新版本867，接着进一步找到831 -841，通过下载安装ec_patch_831-841.cup来更新到最新版本。
2.	如果找不到对应的版本对，那么利用本地版本所属的版本区间来查找，然后进行1中所述的更新操作。例如
本地版本是827，那么找不到相应的版本对，但能找到区间822-831包含827，查找到822-831，然后利用1中所述
方式一步步更新到最新版本。

867 822
822 831
831 841
841 863
842 863
843 863
... 
862 863
863 867
864 867
865 867
866 867
*/
// 返回false表示version.txt出错了
bool PackUpdater::MakeDownloadList()
{
	if (iOldVersion >= iNewVersion)
	{
		return false;
	}

	m_DownloadList.clear();
	PairsList::iterator iter = m_VersionPairs.find(iOldVersion);
	// 刚好找得到版本号
	if (iter != m_VersionPairs.end())
	{
		do
		{
			m_DownloadList.insert(make_pair(iter->first,iter->second));
			iter = m_VersionPairs.find(iter->second);

		}while (iter != m_VersionPairs.end());
	}
	// 找对应版本区间
	else
	{
		for (iter = m_VersionPairs.begin();iter != m_VersionPairs.end();++iter)
		{
			if (iter->first < iOldVersion && iter->second > iOldVersion)
			{
				do
				{
					m_DownloadList.insert(make_pair(iter->first,iter->second));
					iter = m_VersionPairs.find(iter->second);

				}while (iter != m_VersionPairs.end());
				break;
			}
		}
	}

	PairsList::reverse_iterator iter_reverse = m_DownloadList.rbegin();
	return !m_DownloadList.empty() && iter_reverse->second == iNewVersion;
}
static bool ReadLine(FILE* fp, char* szLine)
{
	if (fgets(szLine, 260, fp) == NULL) return false;
	szLine[strcspn(szLine, "\r\n")] = '\0';
	return true;
}
// 跟SepFileUpdater的Getversion代码有太多重复的地方
int PackUpdater::GetVersion(bool bFirstGet)
{
	if (!CheckLocal())
	{	
		return Updater::LOCAL_VERSION_ERROR;
	}
	if (!CheckPackVersion(bFirstGet))
	{
		return Updater::PACKVERSION_GETTING_FAILED;
	}
	FILE *fStream;

	m_VersionPairs.clear();
	UDeleteFile(PACK_VERSION_FILE);
	EnterCriticalSection(&ServerSection);
	DownloadFileByHttp(fnElementPatchName+TEXT("version.txt"),PACK_VERSION_FILE,NULL,0);
	LeaveCriticalSection(&ServerSection);
	if((fStream=OpenFile(PACK_VERSION_FILE,TEXT("r")))!=NULL)
	{
		char s[400];
		if(!ReadLine(fStream,s))
		{
			return FAILED_DOWNLOADING_VERSION_FILE;
		}

		sscanf(s,"%d %d",&iNewVersion,&iStopVersion);
		int versionFrom,versionTo;
		while(ReadLine(fStream,s))
		{
			sscanf(s,"%d %d",&versionFrom,&versionTo);
			m_VersionPairs.insert(make_pair(versionFrom,versionTo));
		}

		fclose(fStream);
		UDeleteFile(PACK_VERSION_FILE);
	}
	else
	{
		PackFinalize();
		return Updater::FAILED_DOWNLOADING_VERSION_FILE;
	}

	if (iOldVersion < iStopVersion)
	{
		PackFinalize();
		return Updater::VERSION_STOP_UPDATE;
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


bool PackUpdater::dll_p2spStart( int gameID )
{
	if (m_hHandle)
	{
		DLL_P2SPSTART h_dll_p2spStart = NULL;
		h_dll_p2spStart = (DLL_P2SPSTART)::GetProcAddress(m_hHandle,"p2spStart");
		if (h_dll_p2spStart)
		{
			return h_dll_p2spStart(gameID);
		}
	}
	return false;
}

bool PackUpdater::dll_p2spSetting( const SettingItemT* setting )
{
	if (m_hHandle)
	{
		DLL_P2SPSETTING h_dll_p2spSetting = NULL;
		h_dll_p2spSetting = (DLL_P2SPSETTING)::GetProcAddress(m_hHandle,"p2spSetting");
		if (h_dll_p2spSetting)
		{
			return h_dll_p2spSetting(setting);
		}
	}
	return false;
}

bool PackUpdater::dll_p2spGetSetting( SettingItemT* setting )
{
	if (m_hHandle)
	{
		DLL_P2SPGETSETTING h_dll_p2spGetSetting = NULL;
		h_dll_p2spGetSetting = (DLL_P2SPGETSETTING)::GetProcAddress(m_hHandle,"p2spGetSetting");
		if (h_dll_p2spGetSetting)
		{
			return h_dll_p2spGetSetting(setting);
		}
	}
	return false;
}

bool PackUpdater::dll_p2spAddDownload( const char* filemd5 )
{
	if (m_hHandle)
	{
		DLL_P2SPADDDOWNLOAD h_dll_p2spAddDownload = NULL;
		h_dll_p2spAddDownload = (DLL_P2SPADDDOWNLOAD)::GetProcAddress(m_hHandle,"p2spAddDownload");
		if (h_dll_p2spAddDownload)
		{
			return h_dll_p2spAddDownload(filemd5);
		}
	}
	return false;
}

bool PackUpdater::dll_p2spStartDownload( const char* filemd5 )
{
	if (m_hHandle)
	{
		DLL_P2SPSTARTDOWNLOAD h_dll_p2spStartDownload = NULL;
		h_dll_p2spStartDownload = (DLL_P2SPSTARTDOWNLOAD)::GetProcAddress(m_hHandle,"p2spStartDownload");
		if (h_dll_p2spStartDownload)
		{
			return h_dll_p2spStartDownload(filemd5);
		}
	}
	return false;
}

bool PackUpdater::dll_p2spStopDownload( const char* filemd5 )
{
	if (m_hHandle)
	{
		DLL_P2SPSTOPDOWNLOAD h_dll_p2spStopDownload = NULL;
		h_dll_p2spStopDownload = (DLL_P2SPSTOPDOWNLOAD)::GetProcAddress(m_hHandle,"p2spStopDownload");
		if (h_dll_p2spStopDownload)
		{
			return h_dll_p2spStopDownload(filemd5);
		}
	}
	return false;
}

bool PackUpdater::dll_p2spDeleteDownload( const char* filemd5 )
{
	if (m_hHandle)
	{
		DLL_P2SPDELETEDOWNLOAD h_dll_p2spDeleteDownload = NULL;
		h_dll_p2spDeleteDownload = (DLL_P2SPDELETEDOWNLOAD)::GetProcAddress(m_hHandle,"p2spDeleteDownload");
		if (h_dll_p2spDeleteDownload)
		{
			return h_dll_p2spDeleteDownload(filemd5);
		}
	}
	return false;
}

bool PackUpdater::dll_p2spGetTaskInfomation( TaskInfomationT* info )
{
	if (m_hHandle)
	{
		DLL_P2SPGETTASKINFOMATION h_dll_p2spGetTaskInfomation = NULL;
		h_dll_p2spGetTaskInfomation = (DLL_P2SPGETTASKINFOMATION)::GetProcAddress(m_hHandle,"p2spGetTaskInfomation");
		if (h_dll_p2spGetTaskInfomation)
		{
			return h_dll_p2spGetTaskInfomation(info);
		}
	}
	return false;
}

bool PackUpdater::dll_p2spStop()
{
	if (m_hHandle)
	{
		DLL_P2SPSTOP h_dll_p2spStop = NULL;
		h_dll_p2spStop = (DLL_P2SPSTOP)::GetProcAddress(m_hHandle,"p2spStop");
		if (h_dll_p2spStop)
		{
			return h_dll_p2spStop();
		}
	}
	return false;
}