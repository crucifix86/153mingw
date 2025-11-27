#pragma once

#include <Afxinet.h>
#pragma warning(disable:4786)

#include <vector>
using namespace std;
#include "Singleton.h"
namespace PATCH
{

#define BLOCK_SIZE      8192
#define MAXFILESIZE     1073741824
#define SESSION_FLAG   INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT \
						| INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE

class Downloader;
class DownloadManager;

typedef int(* callback_t)(int status, Downloader* worker);

void __cdecl WorkerThread(void* p);

enum Status{
	DOWNLOAD_START  = 1,    // 开始下载, GetSize() 获得文件大小
	BLOCK_DONE      = 2,    // 下载了一块, GetOffset() 获得偏移
	FILE_DONE       = 3,    // 下载完成
	RET_ERROR       = 4,    // 返回错误，GetError() 错误代码
	NET_ERROR       = 5,    // CInternetException
};

enum Error_code{
	ERR_URL_PARSE   = 1,    // URL 解析错误
	ERR_GET_ADDR    = 2,    // 无法解析服务器地址 
	ERR_OVERFLOW    = 3,    // 写文件越界
	ERR_CREATE_FILE = 4,    // 无法创建文件
	ERR_REQ_DENIED  = 5,
	ERR_CONNECTION  = 6,    // 网络连接错误
	ERR_UNKNOWN     = 7,    // 俺也不知道
};

enum Mode{
	BLOCKING, 
	NONBLOCK,
};

typedef unsigned int DID;   // Downloader ID
class Downloader
{
protected:
	DID m_id;
	int m_offset;
	int m_size; 
	int m_verb;
	int m_status;
	int m_error;
	
	CString	m_url;
	
	CHttpConnection  *m_pServer;
	CInternetSession *m_pSession;
	CHttpFile        *m_pFile;
	
	CString   m_filename;
	
	DownloadManager*   m_mgr;
	callback_t         m_callback;
	
	DWORD Redirect();
	DWORD Connect(CString url);
	
	void Downloader::Read();
	void Downloader::ReadHeader();
public:
	Downloader(DID id, DownloadManager* mgr, callback_t callback,int iConnectTimeout = 60000,int iReceiveTimeout = 90000);
	~Downloader();
	
	void Run();
	
	void Download(LPCTSTR url, LPCTSTR fn, int mode,  int start=-1);
	
	DID GetID() { return m_id; }
	int GetError() { return m_error; }
	unsigned int GetSize() { return m_size; }
	unsigned int GetOffset() { return m_offset; }
	const CString& GetFileName() { return m_filename; }
	const CString& GetUrl() { return m_url; }
};


class DownloadManager:public Singleton<DownloadManager>
{
	int m_maxthreads;
	int m_mode;

	CString  m_server;
	CString  m_object;

	INTERNET_PORT m_port;

	std::vector<Downloader*> m_Workers;
	int m_iConnectTimeout;
	int m_iReceiveTimeout;

	//std::vector<std::string>  m_ip;
	Downloader* RecruitWorker(DID* id, callback_t callback);

public:
	DownloadManager() : m_maxthreads(0), m_mode(BLOCKING),m_iConnectTimeout(60000),m_iReceiveTimeout(90000){}
	DownloadManager(int mode) : m_maxthreads(0), m_mode(mode){}
	~DownloadManager(){
		size_t WorkerNum = m_Workers.size();
		for (int i = 0;i < WorkerNum;++i)
		{
			if (m_Workers[i])
			{
				delete m_Workers[i];
				m_Workers[i] = NULL;
			}
	}
	}

	void SetServer(LPCTSTR address, int maxthreads = 0);
	DID GetByUrl(LPCTSTR url, LPCTSTR filename, callback_t callback, unsigned int start);
	DID GetByPath(LPCTSTR object, LPCTSTR filename, callback_t callback, unsigned int start);
	void SetTimeOut(int iConnectTimeout,int iReceiveTimeout){m_iConnectTimeout = iConnectTimeout;m_iReceiveTimeout = iReceiveTimeout;}

	int Cancel(DID id);

	LPCTSTR GetServerName() { return m_server; }
};



};
