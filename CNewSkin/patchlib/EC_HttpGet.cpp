#include <process.h>
#include "EC_HttpGet.h"

template<> PATCH::DownloadManager* Singleton<PATCH::DownloadManager>::ms_Singleton = 0;
namespace PATCH
{

Downloader::Downloader(DID id, DownloadManager* mgr, callback_t callback,int iConnectTimeout,int iReceiveTimeout) 
	: m_id(id), m_status(0), m_pServer(NULL), m_pFile(NULL), m_mgr(NULL)
{
	m_callback = callback;
	m_pSession = new CInternetSession(_T("LiveUpdate Application"));
	m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, iConnectTimeout); 
	m_pSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, iReceiveTimeout); 
}

Downloader::~Downloader()
{
	if(m_pFile != NULL)
	{
		m_pFile->Close();
		delete m_pFile;
	}
	if(m_pServer != NULL)
	{
		m_pServer->Close();
		delete m_pServer;
	}
	if(m_pServer)
	{
		m_pSession->Close();
		delete m_pSession;
	}
}

void __cdecl WorkerThread(void* p)
{
	Downloader* worker = (Downloader*)p;
	worker->Run();
}


Downloader* DownloadManager::RecruitWorker(DID* id, callback_t callback)
{
	static unsigned int counter = 0;
	*id = counter++;
	Downloader* worker = new Downloader(*id, this, callback,m_iConnectTimeout,m_iReceiveTimeout);
	return worker;
}

DID DownloadManager::GetByPath(LPCTSTR object,LPCTSTR filename, callback_t callback, unsigned int start)
{
	DID id;
	RecruitWorker(&id, callback)->Download(object, filename, m_mode, start);
	return id;
}

DID DownloadManager::GetByUrl(LPCTSTR url, LPCTSTR filename, callback_t callback, unsigned int start)
{
	DID id;
	RecruitWorker(&id, callback)->Download(url, filename, m_mode, start);
	return 0;
}

void DownloadManager::SetServer(LPCTSTR server, int maxthreads)
{
//	DWORD type;
//	CString url(server);
//
//	m_maxthreads = maxthreads;
//
//	if (!AfxParseURL(url, type, m_server, m_object, m_port))
//		throw ERR_URL_PARSE;
//	if(type!= AFX_INET_SERVICE_HTTP)
//		throw ERR_URL_PARSE;
//
//	struct hostent * addr = gethostbyname(m_server);
//
//	if(!addr)
//		throw ERR_GET_ADDR;
//	char ** p;
//	int n = 0;
//	for(p=addr->h_addr_list;*p!=NULL;p++) 
//	{
//		m_ip.push_back(inet_ntoa(*(struct in_addr *)(*p)));
//		n++;
//	}
}

void Downloader::Run()
{
	try
	{
		DWORD dwRet =  Connect(m_url);

		if (dwRet == HTTP_STATUS_MOVED ||
				dwRet == HTTP_STATUS_REDIRECT ||
				dwRet == HTTP_STATUS_REDIRECT_METHOD)
		{
			dwRet = Redirect();
		}

		if (dwRet > HTTP_STATUS_PARTIAL_CONTENT)
		{
			m_error = dwRet;
			m_status = NET_ERROR;
		}
		else
		{
			if(m_offset<=0)
				ReadHeader();
			Read();
			m_status = FILE_DONE;
		}
	}
	catch (CInternetException* pEx)
	{
		m_status = NET_ERROR;
		m_error = pEx->m_dwError;
		pEx->Delete();
	}
	catch (Error_code code){
		m_status = RET_ERROR;
		m_error = code;
	}
	catch (...)
	{
		m_status = RET_ERROR;
		m_error = ERR_UNKNOWN;
	}

	if(m_callback)
	{
		if(m_status == RET_ERROR)
			m_callback(m_status, this);
		else
			m_callback(FILE_DONE, this);
	}

	delete this;
}

DWORD Downloader::Redirect()
{
	CString strNewLocation;
	m_pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);

	int nPlace = strNewLocation.Find(_T("Location: "));
	if (nPlace == -1)
		throw ERR_GET_ADDR;

	strNewLocation = strNewLocation.Mid(nPlace + 10);
	nPlace = strNewLocation.Find('\n');
	if (nPlace > 0)
		strNewLocation = strNewLocation.Left(nPlace);

	m_pFile->Close();
	delete m_pFile;
	m_pServer->Close();
	delete m_pServer;

	return Connect(strNewLocation);
}

void Downloader::Download(LPCTSTR url, LPCTSTR fn,  int mode, int start)
{
	m_url = url;
	m_filename = fn;
	m_verb = CHttpConnection::HTTP_VERB_GET;
	m_offset = start;
	if(mode==BLOCKING)
		Run();
	else
		_beginthread(WorkerThread, 0, this);
}

DWORD Downloader::Connect(CString url)
{
	const TCHAR szHeaders[] =_T("Accept: */*\r\nUser-Agent: Mozilla/4.0\r\n");

	CString strServerName;
	CString strObject;
	INTERNET_PORT nPort;
	DWORD type;
	if (!AfxParseURL(url, type, strServerName, strObject, nPort))
		throw ERR_URL_PARSE;

	m_pServer = m_pSession->GetHttpConnection(strServerName, nPort);

	m_pFile = m_pServer->OpenRequest(m_verb,strObject, NULL, 1, NULL, NULL, SESSION_FLAG);
	m_pFile->AddRequestHeaders(szHeaders);
	if(m_offset>0)
	{
		CString range;
		range.Format(_T("RANGE: bytes= %d -"), m_offset);
		m_pFile->AddRequestHeaders(range);

	}else
		m_offset = 0;
	m_pFile->SendRequest();
	DWORD dwRet;
	m_pFile->QueryInfoStatusCode(dwRet);
	return dwRet;
}

void Downloader::ReadHeader()
{
	CString stmp;
	if(m_pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, stmp))
		m_size = _ttol(stmp);
	else
		m_size = 0;

	SetFileAttributes(m_filename, FILE_ATTRIBUTE_NORMAL);
	FILE* fp = _tfopen(m_filename, TEXT("wb"));
	if(!fp)
		throw ERR_CREATE_FILE;
	if(m_size>0){
		fseek(fp, m_size-1, SEEK_SET);
		fputc(0, fp);
	}
	if(m_callback)
		m_callback(DOWNLOAD_START, this);

	fclose(fp);
}

int DownloadManager::Cancel(DID id)
{
	// TODO 
	return 0;
}

void Downloader::Read()
{
	FILE* fp = _tfopen(m_filename, TEXT("rb+"));
	if(!fp)
		throw ERR_CREATE_FILE;
	unsigned int maxsize = fseek(fp,0, SEEK_END);
	maxsize = ftell(fp);
	if(maxsize==0)
		maxsize = MAXFILESIZE;

	fseek(fp, m_offset, SEEK_SET );

	char sz[4097];
	int nRead = 1, total = 0;

	while(nRead > 0) 
	{
		
		nRead = m_pFile->Read(sz, 4096);
		if((unsigned int)(m_offset+nRead)>maxsize)
		{
			fclose(fp);
			throw ERR_OVERFLOW;
		}
		if(nRead>0)
		{
			fwrite(sz, 1, nRead, fp);
			total += nRead;
			m_offset += nRead;
			if(total>BLOCK_SIZE && m_callback)
			{
				m_callback(BLOCK_DONE, this);
				total = 0;
			}
		}
	}
	fclose(fp);
}

}
