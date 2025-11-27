#ifndef NEWSKIN_PACKUPDATER_H_
#define NEWSKIN_PACKUPDATER_H_

#include <map>
#include <list>
#include <string>

#include "Singleton.h"
#include "Updater.h"
#include "..//p2sp_4th_lib/i_p2sp_download.h"
#ifdef _DEBUG
#define CONST_STRING_DLL_PATH L"p2sp_4th_libD.dll"
#else
#define CONST_STRING_DLL_PATH L"p2sp_4th_lib.dll"
#endif

class Downloader;

class PackUpdater : public Updater, public Singleton<PackUpdater>
{
public:
	PackUpdater();
	virtual ~PackUpdater();

	virtual bool Update();
	virtual int GetVersion(bool bFirstGet);

	void DownloadFailed(const char* name);
	void SetUpdateFailed();
	bool IsUpdateFailed();

	friend UINT P2SPDownloadThread(LPVOID pParam);
	friend UINT HTTPDownloadThread(LPVOID pParam);
	
	//DLL导出函数封装
	/// 更新、加载下载DLL，并加载该游戏的任务信息
	typedef bool (__cdecl * DLL_P2SPSTART)(int gameID);
	bool dll_p2spStart(int gameID);

	/// 设置、获取下载参数
	typedef bool (__cdecl * DLL_P2SPSETTING)(const SettingItemT* setting); 
	bool dll_p2spSetting(const SettingItemT* setting); 
	typedef bool (__cdecl * DLL_P2SPGETSETTING)(SettingItemT* setting); 
	bool dll_p2spGetSetting(SettingItemT* setting); 

	/// 添加单个文件下载任务
	typedef bool (__cdecl * DLL_P2SPADDDOWNLOAD)(const char* filemd5);
	bool dll_p2spAddDownload(const char* filemd5);

	/// 开始下载
	typedef bool (__cdecl * DLL_P2SPSTARTDOWNLOAD)(const char* filemd5);
	bool dll_p2spStartDownload(const char* filemd5);

	/// 停止下载任务
	typedef bool (__cdecl * DLL_P2SPSTOPDOWNLOAD)(const char* filemd5);
	bool dll_p2spStopDownload(const char* filemd5);

	/// 删除下载任务
	typedef bool (__cdecl * DLL_P2SPDELETEDOWNLOAD)(const char* filemd5);
	bool dll_p2spDeleteDownload(const char* filemd5);

	/// 获取任务的相关
	typedef bool (__cdecl * DLL_P2SPGETTASKINFOMATION)(TaskInfomationT* info);
	bool dll_p2spGetTaskInfomation(TaskInfomationT* info);

	/// 停止所有下载任务，并卸载DLL
	typedef bool (__cdecl * DLL_P2SPSTOP)();
	bool dll_p2spStop();

protected:
	// 根据当前版本和最新版本生成需要下载的更新包列表
	bool MakeDownloadList();
	// 下载每个更新包的MD5文件
	bool DownloadMD5List();
	// 检查下载目录里是否有上次下载完但未安装完的更新包
	bool CheckDownloadedFile();
	// 下载线程
	bool BeginDownloadThread();
	// 启动P2SP下载引擎
	bool StartP2SPEngine();

protected:
	Downloader* m_pDownloader;
	HMODULE m_hHandle;
	bool m_bP2SPStartSucc;
	volatile bool m_bDownloadError;
	// 当前正在安装的更新包序号，从0开始
	int m_iCurrentPack;
	// 保存需要更新的序号对
	typedef std::map<int,int> PairsList;
	PairsList m_DownloadList;
	PairsList m_VersionPairs;
	// 保存更新包的md5值，后面int标志更新包下载完成与否，用list是不希望排序
	typedef std::list<std::pair<std::string,int>> StringList;
	StringList m_MD5List;
	CRITICAL_SECTION m_csMD5List;
	TaskInfomationT m_CurrentDownloadInfo;

};

#endif //NEWSKIN_PACKUPDATER_H_
