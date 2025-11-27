    // PackDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include <vector>
using namespace std;

#include "AFI.h"
#include "AFilePackage.h"
#include "AWString.h"
#include "PackDLL.h"
#include "elementpckdir.h"
#include <DIRECT.H>
#include "AIniFile.h"
#include "EC_Md5hash.h"
#include "..\Base64.h"
#include "AAssist.h"
const int CleanUpMinSize=15000000;
const double CleanUpMinPercent=0.7;
const unsigned int CleanUpMinFragmentSize = 200 * 1024 * 1024;

const wchar_t fnErrorLog[]=L"/config/element/fileerr.log";
AString szWorkDir;
AString szInstallPath;
int PackVersion = 1;
double MinFreeDiskSize;
AFilePackage LocalUpdateFilePack;
int iPid = 1;
int iVersionFrom = 0;
int iVersionTo = 0;
int iOldVersion = 0;
int iOldVersion1 = 0;
int iNewVersion1;
int iTotalLine;
int iLineNumber;
int pid = 1;
Base64Code base64;
const int CodePage=936;
char md5[100];

std::vector<AString> FilesToDelete;

AString fnLocalUpdatePack;
const AString fnPIDFile="config\\element\\pid.sw";
const AString fnPackPIDFile="pid";
const AString fnElementLogName=("update.");
const AString fnElementLogFile=fnElementLogName+("log");
const AString fnUpdateList="config\\element\\inc.sw";
const AString DownloadTempFile="config\\element\\tempfile.dat";
const AString fnElementOldVersion="config\\element\\version.sw";
const AString fnElement="element/";
void WriteElementLog(const char* c,BOOL reWrite = FALSE);
bool IsFileInPack(const char *fn);
void RemoveFileFromPack(const char *fn);

CRITICAL_SECTION csProgress;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
    return TRUE;
}

void UDeleteFile(const char* src)
{
	SetFileAttributes(src, FILE_ATTRIBUTE_NORMAL);
	DeleteFile(src);
}
bool GetFileFromPack(const char *fn,DWORD &size,unsigned char *&fbuf)
{
	fbuf=NULL;
	int k=0;
	while(fn[k]=='.'&&fn[k+1]=='/')
		k+=2;
	const char* cfn = &fn[k];
	int p=0;
	AFilePackage *fPack=g_AFilePackMan.GetFilePck(cfn);
	if(fPack!=NULL)
	{
		AFilePackage::FILEENTRY entry;
		if(fPack->GetFileEntry(cfn,&entry))
		{
			size=entry.dwCompressedLength;
			fbuf=new unsigned char[size+6];
			DWORD len=entry.dwLength;
			for(int i=0;i<4;i++)
			{
				fbuf[i]=len&255;
				len=len>>8;
			}
			fPack->ReadCompressedFile(cfn,fbuf+4,&size);
			size+=4;
		}
		return true;
	}
	return false;
}
FILE* OpenFile(const char* name, const char* param)
{
	FILE* f=NULL;
	int i=0;
	while(i<=10&&(f=fopen(name, param))==NULL)
	{
		Sleep(30);
		i++;
	}
	return f;
}
DWORD GetFileSize(const char *fn)
{
	FILE *f;
	if((f=OpenFile(fn,"rb"))!=NULL)
	{
		unsigned int maxsize = fseek(f,0, SEEK_END);
		maxsize = ftell(f);
		fclose(f);
		return maxsize;
	}
	else
		return -1;
}
void AddFileToPack(const char *fn,const char *tempfn)
{
	int k=0;
	while(fn[k]=='.'&&fn[k+1]=='/')
		k+=2;
	const char* cfn = fn + k;
	int fsize=GetFileSize(tempfn);
	if(fsize==0xFFFFFFFF)
		return;
	int p=0;
	AFilePackage *fPack=g_AFilePackMan.GetFilePck(cfn);
	if(fPack!=NULL)
	{
		unsigned char *fbuf=new unsigned char[fsize+1];
		FILE *f=OpenFile(tempfn,"rb");
		fsize=fread(fbuf,sizeof(char),fsize,f);
		fclose(f);
		DWORD frealsize=0;
		for(int i=3;i>=0;i--)
			frealsize=frealsize*256+fbuf[i];
		AFilePackage::FILEENTRY entry;
		bool b;
		if(fPack->GetFileEntry(cfn,&entry))
			b=fPack->ReplaceFileCompressed(cfn,fbuf+4,frealsize,fsize-4);
		else
			b=fPack->AppendFileCompressed(cfn,fbuf+4,frealsize,fsize-4);
		delete []fbuf;
	}
	
}
PackExport void SetPackVersion(int version)
{
	PackVersion = version;
}
PackExport int GetPackVersion()
{
	int version=99999999;
	char fnDes[400];
	int PackFileNum = sizeof(g_szPckDir1) / sizeof(const char*);
	if( PackVersion == 2 )
		PackFileNum = sizeof(g_szPckDir2) / sizeof(const char*);
	for(int i=0; i< PackFileNum; i++)
	{
		if( PackVersion == 2 )
		{
			strcpy(fnDes, g_szPckDir2[i]);
			strcpy(fnDes + strlen(g_szPckDir2[i]), "\\version.sw");
		}
		else
		{
			strcpy(fnDes, g_szPckDir1[i]);
			strcpy(fnDes + strlen(g_szPckDir1[i]), "\\version.sw");
		}
		AFilePackage *fPack=g_AFilePackMan.GetFilePck(fnDes);
		unsigned char *fbuf = new unsigned char[100];
		DWORD fsize = 100;
		if(fPack && fPack->ReadFile(fnDes,fbuf,&fsize))
		{
			fbuf[fsize]='\0';
			int n;
			sscanf((char*)fbuf,"%d",&n);
			if(n<version)
				version=n;
			delete []fbuf;
		}
	}
	return version;
}

FILE* OpenWFile(LPCWSTR name, LPCWSTR param)
{
	FILE* f=NULL;
	int i=0;
	while(i<=10&&(f=_wfopen(name, param))==NULL)
	{
		Sleep(30);
		i++;
	}
	return f;
}


void MakeDir(const char *dir,int r)
{
	r--;
	while(r>0&&dir[r]!='/'&&dir[r]!='\\')
		r--;
	if(r==0)
		return;
	MakeDir(dir,r);
	char t[400];
	strcpy(t,dir);
	t[r]='\0';
	_mkdir(t);
}

void DeleteDir(const char *dir,int r)
{
	r--;
	while(r>0&&dir[r]!='/'&&dir[r]!='\\')
		r--;
	if(r==0)
		return;
	char t[400];
	strcpy(t,dir);
	t[r]='\0';
	_rmdir(t);
	DeleteDir(dir,r);
}

bool bInit=false;

bool CalcCompressFile(const char *fnSrc,DWORD &size,unsigned char *&fbuf)
{
	fbuf=NULL;
	int fsize=GetFileSize(fnSrc);
	if(fsize==0xFFFFFFFF)
		return false;
	FILE *f=OpenFile(fnSrc,"rb");
	if(f==NULL)
		return false;
	unsigned char *fbuf1=new unsigned char[fsize+1];
	fsize=fread(fbuf1,1,fsize,f);
	fclose(f);
	DWORD frealsize=fsize+1;
	fbuf=new unsigned char[fsize+5];
	if(AFilePackage::Compress(fbuf1,fsize,fbuf+4,&frealsize)!=0||frealsize>=fsize)
	{
		frealsize=fsize;
		memcpy(fbuf+4,fbuf1,fsize);
	}
	delete fbuf1;
	DWORD len=fsize;
	for(int i=0;i<4;i++)
	{
		fbuf[i]=len&255;
		len=len>>8;
	}
	size=frealsize+4;
	return true;
}

bool DecompressFile(const char *fnSrc,const char *fnDes)
{
	SetFileAttributes(fnDes, FILE_ATTRIBUTE_NORMAL);
	int fsize=GetFileSize(fnSrc);
	if(fsize==0xFFFFFFFF)
		return false;
	FILE *f=OpenFile(fnSrc,"rb");
	if(f==NULL)
		return false;
	unsigned char *fbuf1=new unsigned char[fsize+1];
	fsize=fread(fbuf1,1,fsize,f);
	fclose(f);
	DWORD frealsize=0;
	for(int i=3;i>=0;i--)
		frealsize=frealsize*256+fbuf1[i];
	if(frealsize+4==fsize)
	{
		f=OpenFile(fnDes,"wb");
		if(f==NULL)
		{
			delete fbuf1;
			return false;
		}
		fwrite(fbuf1+4,1,frealsize,f);
		fclose(f);
		delete fbuf1;
		return true;
	}
	unsigned char *fbuf2=new unsigned char[frealsize+1];
	int ret;
	if((ret = AFilePackage::Uncompress(fbuf1+4,fsize-4,fbuf2,&frealsize))!=0)
	{
		delete fbuf1;
		delete fbuf2;
		return false;
	}
	delete fbuf1;
	f=OpenFile(fnDes,"wb");
	if(f==NULL)
	{
		delete fbuf2;
		return false;
	}
	fwrite(fbuf2,1,frealsize,f);
	fclose(f);
	delete fbuf2;
	return true;
}

// 取version.sw和pck文件里的版本号中最小的那个，在SetDirPath和打开本地pck包文件之后调用
bool CheckPckVersion()
{
	int pverson = GetPackVersion();
	if(pverson < iOldVersion)
		iOldVersion = pverson;
	return true;
}
int SetDirPath(const char* strInstallPath)
{
	szInstallPath = strInstallPath;
	szWorkDir = szInstallPath + "element\\";
	int ret(0);
	FILE *fLocalVersion;
	if((fLocalVersion=OpenFile(szInstallPath + "config\\element\\version.sw",TEXT("r")))==NULL)
	{
		WriteElementLog(" Version file open failed!.");
		WriteElementLog(NULL);	
		return PCK_ERROR_INIT_LOCAL_VERSION_FILE;
	}
	else fscanf(fLocalVersion,"%d %d",&iOldVersion,&iOldVersion1);
	fclose(fLocalVersion);
	return ret;
}
int CheckPid()
{
	return 0;
}
int OpenClientPck()
{
	af_Initialize();
	af_SetBaseDir(szWorkDir);
	
	int PackFileNum = sizeof(g_szPckDir1) / sizeof(const char*);
	if( PackVersion == 2 )
		PackFileNum = sizeof(g_szPckDir2) / sizeof(const char*);
	int i;
	MakeDir(szWorkDir,szWorkDir.GetLength());
	SetFileAttributesA(szWorkDir, FILE_ATTRIBUTE_NORMAL);
	for(i=0; i<PackFileNum; i++)
	{
		char szPckFile[MAX_PATH];
		if( PackVersion == 2 )
			sprintf(szPckFile, "%s.pck", g_szPckDir2[i]);
		else
			sprintf(szPckFile, "%s.pck", g_szPckDir1[i]);
		
		if(!g_AFilePackMan.OpenFilePackage(szPckFile))
		{
			g_AFilePackMan.CloseAllPackages();
			af_Finalize();
			WriteElementLog(AString(" pck open failed: ") + szPckFile);
			WriteElementLog(NULL);
			return PCK_ERROR_INIT_PCK_OPEN_FAILED;
		}
		else
		{
			AFilePackage *pack=g_AFilePackMan.GetFilePck(i);
			if(pack->GetFileNumber()==0)
			{
				g_AFilePackMan.CloseAllPackages();
				af_Finalize();
				WriteElementLog(AString(" pck is empty: ") + szPckFile);
				WriteElementLog(NULL);
				return PCK_ERROR_INIT_PCK_EMPTY;
			}
		}
	}
	bInit=true;
	MakeDir(fnElementLogFile,strlen(fnElementLogFile));
	CheckPckVersion();
	return 0;
}
bool UseLocalUpdatePack(const char *fn)
{
	return LocalUpdateFilePack.Open(fn,AFilePackage::OPENEXIST);
}
bool GetLocalUpdateFile(const char *fnSrc,const char *fnDes)
{
	AFilePackage::FILEENTRY entry;
	unsigned char *fbuf=NULL;
	if(LocalUpdateFilePack.GetFileEntry(fnSrc,&entry))
	{
		ULONG size=entry.dwLength;
		fbuf=new unsigned char[size+2];
		LocalUpdateFilePack.ReadFile(fnSrc,fbuf,&size);
		SetFileAttributes(fnDes, FILE_ATTRIBUTE_NORMAL);
		FILE* f=OpenFile(fnDes,"wb");
		if(f!=NULL)
		{
			fwrite(fbuf,sizeof(char),size,f);
			fclose(f);
			delete []fbuf;
			return true;
		}
		delete []fbuf;
	}
	return false;
}
int CheckVersionUpdatable()
{
	int ret(0);
	AString strNewver = szInstallPath + "config\\element\\newver.sw";
	UDeleteFile(strNewver);
	while (true)
	{
		GetLocalUpdateFile("version", strNewver);
		FILE *fStream;
		if((fStream=OpenFile(strNewver,"r"))!=NULL)
		{
			fscanf(fStream,"%d %d",&iVersionTo,&iNewVersion1);
			fclose(fStream);
			UDeleteFile(strNewver);
		}
		else
		{
			WriteElementLog(" pck not include version");
			WriteElementLog(NULL);
			ret = PCK_ERROR_UPDATE_PCK_NO_NEWVERSION;
			break;
		}
		if(iVersionTo==0)
		{
			WriteElementLog(" pck not include version");
			WriteElementLog(NULL);
			ret = PCK_ERROR_UPDATE_PCK_NO_NEWVERSION;
			break;
		}
		int p=iVersionTo-iOldVersion;
		if(p<0)
		{
			WriteElementLog(" pck version lower than client");
			WriteElementLog(NULL);
			ret = PCK_ERROR_INIT_VERSION_TOO_HIGH;
			break;
		}
		
		if(p==0)
		{
			WriteElementLog(" pck version equal with client");
			WriteElementLog(NULL);
			ret = PCK_ERROR_UPDATE_PCK_VERSION_EQUAL;
			break;
		}
		
		break;
	}

	return ret;
}

int OpenUpdatePck()
{
	if(!UseLocalUpdatePack(fnLocalUpdatePack))
	{
		WriteElementLog(" update pck open failed!");
		WriteElementLog(NULL);
		return PCK_ERROR_UPDATE_PCK_OPEN_FAILED;
	}
	AString strPidFile = szInstallPath + fnPIDFile;
	if(GetLocalUpdateFile("pid",strPidFile))
	{
		FILE *fStream=OpenFile(strPidFile,TEXT("r"));
		int iNewPID=0;
		if(fStream!=NULL)
		{
			fscanf(fStream,"%d",&iNewPID);
			fclose(fStream);
			UDeleteFile(strPidFile);
			if(iNewPID!=pid)
			{
				WriteElementLog(" pid error");
				WriteElementLog(NULL);
				return PCK_ERROR_INIT_FILES_CLIENT_NOT_FIT;
			}
			else return 0;
		}
		else return PCK_ERROR_INIT_FILES_CLIENT_NOT_FIT;
	}
	else return PCK_ERROR_INIT_FILES_CLIENT_NOT_FIT;
}
void GetUpdateListFile()
{
	char st[100];
	int l=iVersionTo-iOldVersion;
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
	sprintf(st,"v-%d.inc",k);
	AString strUpdateList = szInstallPath + fnUpdateList;
	UDeleteFile(strUpdateList);
	GetLocalUpdateFile(st,strUpdateList);
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
					int o;
					o=WideCharToMultiByte(CodePage,0,ucode,-1,p,400,NULL,NULL);
					if(o>0)
					{
						o--;
						_tcscpy(desName+k,p);
						k+=o;
					}
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
			int o;
			o=WideCharToMultiByte(CodePage,0,ucode,-1,p,400,NULL,NULL);
			if(o>0)
			{
				o--;
				_tcscpy(desName+k,p);
				k+=o;
			}
		}
	}
	desName[k]=TEXT('\0');
	return TRUE;
}
const int FileSizeOnDraw=8192;
BOOL CalMemMd5(unsigned char *buf,int size,char *md5)
{
	PATCH::MD5Hash m;
	int pos=0;
	while(pos+FileSizeOnDraw<size)
	{
		m.Update((char*)buf+pos,FileSizeOnDraw);
		pos+=FileSizeOnDraw;
	}
	if(pos<size)
		m.Update((char*)buf+pos,size-pos);
	m.Final();
	UINT i=50;
	m.GetString(md5,i);
	return TRUE;
}
BOOL CalFileMd5(const char* lName,char *md5)
{
	FILE *f;
	PATCH::MD5Hash m;
	char buf[FileSizeOnDraw+1];
	int iRead;
	UINT iSize=GetFileSize(lName);
	if((f=OpenFile(lName,TEXT("rb")))!=NULL)
	{
		UINT iReadSize=0;
		do
		{
			iRead=fread(buf,sizeof(char),FileSizeOnDraw,f);
			m.Update(buf,iRead);
			iReadSize+=iRead;
		}while(iRead==FileSizeOnDraw);
		fclose(f);
	}
	UINT i=50;
	m.Final();
	m.GetString(md5,i);
	return TRUE;
}
int UpdatePckFile(callbackFunc DisplayProgress)
{
	EnterCriticalSection(&csProgress);
	iTotalLine=0;
	iLineNumber=0;
	LeaveCriticalSection(&csProgress);
	int iLastProgress = 0;
	FILE *fStream;
	char s[400];
	char fn[350];
	char path[400];
	char dc=' ';
	BOOL bEqual;
	AString strUpdateList = szInstallPath + fnUpdateList;
	AString strTempFile = szInstallPath + DownloadTempFile;

	GetUpdateListFile();
	if((fStream=OpenFile(strUpdateList,"r"))==NULL)
	{
		WriteElementLog("getting update list failed");
		WriteElementLog(NULL);
		return PCK_ERROR_UPDATE_PCK_UPDATE_LIST;
	}
	fgets(s,400,fStream);
	int versionFrom,versionTo;
	char o[30];
	sscanf(s,"%s%d%d",o,&versionFrom,&versionTo);
	if(versionTo!=iVersionTo)
	{
		fclose(fStream);
		WriteElementLog(" update list error");
		WriteElementLog(NULL);	
		return PCK_ERROR_UPDATE_PCK_UPDATE_LIST;
	}
	if(versionFrom>iOldVersion)
	{
		fclose(fStream);
		WriteElementLog(" pck update start version higher than client");
		WriteElementLog(NULL);
		return PCK_ERROR_INIT_VERSION_TOO_LOW;
	}
	fseek(fStream,0,SEEK_SET);
	EnterCriticalSection(&csProgress);
	while(fgets(s,400,fStream)!=NULL)
	{
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		iTotalLine++;
	}
	LeaveCriticalSection(&csProgress);
	if(iTotalLine>0)
	{
		fseek(fStream,0,SEEK_SET);
		char md[50];
		int iPos = 0;
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
			int k=0;
			while(path[k]=='.'&&path[k+1]=='/')
				k+=2;
			char* cpath = path + k;
			char* cfn = fn + k;

			bool bInPack=IsFileInPack(cpath);
			if(md5[0]=='-')
			{
				if(bInPack)
					RemoveFileFromPack(cpath);
				else
					UDeleteFile(szWorkDir+cpath);
			}
			else
			{
				bEqual=FALSE;
				unsigned char *fbuf=NULL;
				DWORD size;
				if(bInPack)
					GetFileFromPack(cpath,size,fbuf);
				else
				{
					AString strPath = (szWorkDir+cpath);
					MakeDir(strPath, strPath.GetLength());
					CalcCompressFile(szWorkDir+cpath,size,fbuf);
				}
				if(fbuf!=NULL)
				{
					CalMemMd5(fbuf,size,md);
					delete []fbuf;
					if(strcmp(md,md5+1)==0)
						bEqual=TRUE;
					else
						bEqual=FALSE;
				}
				else
					bEqual=FALSE;
				
				if (bEqual)
				{
					WriteElementLog("skip the same file: ");
					WriteElementLog(cpath);
					WriteElementLog(NULL);
				}
				else
				{
					WriteElementLog(cpath);
					WriteElementLog(NULL);

					GetLocalUpdateFile(fnElement + cfn, strTempFile);
					CalFileMd5(strTempFile,md);
					if(strcmp(md,md5+1)==0)
						bEqual=TRUE;
					else
						bEqual=FALSE;
					
					if(!bEqual)
					{
						WriteElementLog("file md5 not equal with the md5 list");
						WriteElementLog(NULL);
						return PCK_ERROR_UPDATE_FILE_MD5_FAILED;
					}
					else
					{
						if(bInPack)
							AddFileToPack(cpath,strTempFile);
						else
						{
							if(!DecompressFile(strTempFile,szWorkDir+cpath))
							{
								WriteElementLog("file decompress failed");
								WriteElementLog(NULL);
								return PCK_ERROR_UPDATE_DECOMPRESS_FAILED;
							}
							UDeleteFile(strTempFile);
						}
					}
				}
			}
			EnterCriticalSection(&csProgress);
			iLineNumber++;
			LeaveCriticalSection(&csProgress);
			if (DisplayProgress)
			{
				int iProgress = (float)iLineNumber / iTotalLine * 100;
				if (iProgress > iLastProgress)
				{
					DisplayProgress(iProgress);
					iLastProgress = iProgress;
				}
			}
		}while(TRUE);
	}
	fclose(fStream);
	FillPackVersion();
	UDeleteFile(strUpdateList);
	return 0;
}
PackExport void GetUpdateProgress(int& nTotalFiles, int& nCurrentFile)
{
	EnterCriticalSection(&csProgress);
	nTotalFiles = iTotalLine;
	nCurrentFile = iLineNumber;
	LeaveCriticalSection(&csProgress);
}
PackExport int PackUpdateInit(const char* strInstallPath)
{
	int ret(PCK_INIT_SUCCESS);
	while(true)
	{
		ret = SetDirPath(strInstallPath);
		if (ret) break;
		ret = OpenClientPck();
		if (ret) break;
		InitializeCriticalSection(&csProgress);
		WriteElementLog(" dll pck update begins.");
		WriteElementLog(NULL);
		break;
	}
	return ret;
}
PackExport int PackUpdate(const char* strPackPath, callbackFunc DisplayProgress)
{
	fnLocalUpdatePack = strPackPath;
	pid = GetPrivateProfileIntA("Version", "pid", pid, szInstallPath + "patcher\\server\\pid.ini");
	int ret(0);
	while (true)
	{
		ret = OpenUpdatePck();
		if (ret) break;
		ret = CheckVersionUpdatable();
		if (ret) break;
		ret = UpdatePckFile(DisplayProgress);
		if (ret) break;
		
		break;
	}
	return ret;
}
PackExport int PackInitialize(const char* strInstallPath, const char* strVersionFilePath)
{
	if (SetDirPath(strInstallPath) != 0)
	{
		return PCK_ERROR_INIT_LOCAL_VERSION_FILE;
	}
	FILE* fVersion = OpenFile(strVersionFilePath, "r");
	if (fVersion == NULL)
	{
		WriteElementLog(" Version file open failed!.");
		WriteElementLog(NULL);		
		return PCK_ERROR_INIT_UPDATE_VERSION_FILE;
	}
	char s[400];
	fgets(s, 400, fVersion);
	sscanf(s, "%d %d %d", &iPid, &iVersionFrom, &iVersionTo);
	while(fgets(s,400,fVersion)!=NULL)
	{
		FilesToDelete.push_back(AString(s));
	}

	if (iOldVersion < iVersionFrom)
	{
		WriteElementLog(" Local version below updating min version.");
		WriteElementLog(NULL);
		return PCK_ERROR_INIT_VERSION_TOO_LOW;
	}
	if (iOldVersion >= iVersionTo)
	{
		WriteElementLog(" Local version above updating max version.");
		WriteElementLog(NULL);
		return PCK_ERROR_INIT_VERSION_TOO_HIGH;
	}

	pid = GetPrivateProfileIntA("Version", "pid", pid, AString(strInstallPath) + "patcher\\server\\pid.ini");

	if (pid != iPid)
	{
		WriteElementLog(" the updating files is for other client, not this one!.");
		WriteElementLog(NULL);
		return PCK_ERROR_INIT_FILES_CLIENT_NOT_FIT;
	}

	int ret = OpenClientPck();
	if (ret) return ret;

	WriteElementLog(" Garena update begins.");
	WriteElementLog(NULL);

	int len = FilesToDelete.size();
	for (int i = 0; i < len; ++i)
	{
		if (IsFileInPack(FilesToDelete[i]))
		{
			RemoveFileFromPack(FilesToDelete[i]);
		}
		else
		{
			AString strPath = szWorkDir + FilesToDelete[i];
			int i=0;
			while(i < strPath.GetLength() - 2)
			{
				if(strPath[i] == ('.') && strPath[i+1] == ('/'))
					strPath = strPath.Left(i) + strPath.Right(strPath.GetLength() - i - 2);
				else
				{
					if (strPath[i] == ('/'))
					{
						strPath[i] = ('\\');
					}
					i++;
				}
			}
			DeleteFile(strPath);
		}
	}
	return PCK_INIT_SUCCESS;
}

PackExport void PackFinalize()
{
	if(bInit)
	{
		bInit=false;
		g_AFilePackMan.CloseAllPackages();
		af_Finalize();
		DeleteCriticalSection(&csProgress);
		WriteElementLog(" Garena update ends.");
		WriteElementLog(NULL);
	}
}

bool IsFileInPack(const char *fn)
{
	AFilePackage *fPack=g_AFilePackMan.GetFilePck(fn);
	return (fPack!=NULL);
}


void RemoveFileFromPack(const char *fn)
{
	int p=0;
	AFilePackage *fPack=g_AFilePackMan.GetFilePck(fn);
	if(fPack!=NULL)
	{
		AFilePackage::FILEENTRY entry;
		if(fPack->GetFileEntry(fn,&entry))
		{
			fPack->RemoveFile(fn);
		}
	}

}



// 1、若存在pkx文件，包所占磁盘空间大小应该为pck文件大小加上pkx大小。
// 2、所整理后的包存在pkx文件，应把pkx文件移动到element目录下
PackExport bool CleanUpPackFile(callbackFunc DisplayProgress,bool bCleanUpAll)
{
	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwNumberOfFreeClusters;

	PackFinalize();
	af_Initialize();
	AFilePackage::FILEENTRY entry;
	int FinishFileSize=0;
	int PackFileNum = sizeof(g_szPckDir1) / sizeof(const char*);
	if( PackVersion == 2 )
		PackFileNum = sizeof(g_szPckDir2) / sizeof(const char*);
	char szPathTempPkx[MAX_PATH];
	char szPathTemp[MAX_PATH];
	char szPckFile[MAX_PATH];
	char szPkxFile[MAX_PATH];
	strcpy(szPathTempPkx,szInstallPath);
	strcpy(szPathTemp,szInstallPath);
	strcat(szPathTempPkx,"config\\element\\temp.pkx");
	strcat(szPathTemp,"config\\element\\temp.pck");
	for(int i=0; i<PackFileNum; i++)
	{
		if( PackVersion == 2 )
		{
			sprintf(szPckFile, "%s%s.pck", szWorkDir, g_szPckDir2[i]);
			sprintf(szPkxFile, "%s%s.pkx", szWorkDir, g_szPckDir2[i]);
		}
		else
		{
			sprintf(szPckFile, "%s%s.pck", szWorkDir, g_szPckDir1[i]);
			sprintf(szPkxFile, "%s%s.pkx", szWorkDir, g_szPckDir1[i]);
		}

		int pfilesize=GetFileSize(szPckFile);
		if (pfilesize == -1) continue;
		int pkxfilesize = GetFileSize(szPkxFile);
		unsigned int PackSize = pfilesize;
		if (pkxfilesize != -1)
		{
			PackSize += pkxfilesize;
		}
		if(!bCleanUpAll&&pfilesize<CleanUpMinSize)
			continue;
		GetDiskFreeSpace(NULL,&dwSectorsPerCluster,&dwBytesPerSector,&dwNumberOfFreeClusters,NULL);
		if( ((double)dwSectorsPerCluster)*dwBytesPerSector*dwNumberOfFreeClusters<PackSize+10000000 )
		{
			af_Finalize();
			return false;
		}
		AFilePackage *fPack=new AFilePackage;
		if(!fPack->Open(szPckFile,AFilePackage::OPENEXIST))
		{
			delete fPack;
			continue;
		}
		int sumfiles=fPack->GetFileNumber();
		int totfilesize=0;
		for(int j=0;j<sumfiles;j++)
			totfilesize+=fPack->GetFileEntryByIndex(j)->dwCompressedLength;
		if(bCleanUpAll||(double)totfilesize/pfilesize<CleanUpMinPercent || PackSize - totfilesize > CleanUpMinFragmentSize)
		{
			AFilePackage *newpack=new AFilePackage;
			if(newpack->Open(szPathTemp,AFilePackage::CREATENEW))
			{
				for(int j=0;j<sumfiles;j++)
				{
					entry=*fPack->GetFileEntryByIndex(j);
					unsigned char *fbuf=new unsigned char[entry.dwCompressedLength+1];
					fPack->ReadCompressedFile(entry.szFileName,fbuf,&entry.dwCompressedLength);
					newpack->AppendFileCompressed(entry.szFileName,fbuf,entry.dwLength,entry.dwCompressedLength);
					FinishFileSize+=entry.dwCompressedLength;
					delete []fbuf;
				}
				newpack->Close();
				fPack->Close();
				delete fPack;
				delete newpack;
				if (MoveFileEx(szPathTemp,szPckFile,MOVEFILE_REPLACE_EXISTING)
					&& pkxfilesize != -1) DeleteFile(szPkxFile);
				MoveFileEx(szPathTempPkx,szPkxFile,MOVEFILE_REPLACE_EXISTING);
			}
			else
			{
				fPack->Close();
				delete fPack;
				delete newpack;
			}
		}
		else
		{
			fPack->Close();
			delete fPack;
		}
	}
	af_Finalize();
	return true;
}

void ReleasePackBuf(unsigned char* buf)
{
	delete []buf;
}

PackExport void FillPackVersion()
{
	FILE *f=OpenFile("version.sw","w+");
	if( !f ) 
		return;
	fprintf(f, "%d %d\n", iVersionTo, 0);
	fseek(f, 0, SEEK_SET);
	unsigned char *fbuf=new unsigned char[100];
	int fsize=fread(fbuf,sizeof(char),100,f);
	fclose(f);
	DeleteFile("version.sw");
	char fnDes[400];
	int PackFileNum = sizeof(g_szPckDir1) / sizeof(const char*);
	if( PackVersion == 2 )
		PackFileNum = sizeof(g_szPckDir2) / sizeof(const char*);
	for(int i=0; i< PackFileNum; i++)
	{
		if( PackVersion == 2 )
		{
			strcpy(fnDes, g_szPckDir2[i]);
			strcpy(fnDes + strlen(g_szPckDir2[i]), "\\version.sw");
		}
		else
		{
			strcpy(fnDes, g_szPckDir1[i]);
			strcpy(fnDes + strlen(g_szPckDir1[i]), "\\version.sw");
		}
		AFilePackage *fPack=g_AFilePackMan.GetFilePck(fnDes);
		AFilePackage::FILEENTRY entry;
		if(fPack->GetFileEntry(fnDes,&entry))
			fPack->ReplaceFile(fnDes,fbuf,fsize,true);
		else
			fPack->AppendFile(fnDes,fbuf,fsize,true);
	}
	delete []fbuf;

	FILE *fLocalVersion;
	if((fLocalVersion=OpenFile(szInstallPath + "config\\element\\version.sw",TEXT("w")))!=NULL)
	{
		fprintf(fLocalVersion,"%d %d",iVersionTo,0);
		fclose(fLocalVersion);	
	}
	else
	{
		WriteElementLog(" Version file write failed!.");
		WriteElementLog(NULL);
	}
	iOldVersion = iVersionTo;
}

bool GetStrValueIniFile(const wchar_t* fn,const char* sectName,const char* key,wchar_t* retValue)
{
	char cfn[400];
	char dc=' ';
	WideCharToMultiByte(936,0,fn,-1,cfn,400,&dc,false);

	AIniFile fp;
	if (!fp.Open(cfn))
	{
		return false;
	}

	//char ret[256] = {0};
	AString ret;

	ret = fp.GetValueAsString(sectName, key, ret);

	int nCount = MultiByteToWideChar(936,0,ret,-1,NULL,0);
	
	MultiByteToWideChar(936,0,ret,-1,retValue,nCount);
	
	fp.Close();

	return true;
}


AString cElog="";
const int MaxLogFileSize=300000;
const int NumbersOfLogFile=6;
void WriteElementLog(const char* c,BOOL reWrite)
{
	FILE *fLog;
	if(reWrite)
	{
		int size=GetFileSize(fnElementLogFile);
		if(size==-1||size>=MaxLogFileSize)
		{
			for(int i=NumbersOfLogFile;i>0;i--)
			{
				char t1[10];
				char t2[10];
				sprintf(t1,("%03d"),i);
				sprintf(t2,("%03d"),i-1);
				if(i!=1)
					CopyFile(fnElementLogName+t2,fnElementLogName+t1,FALSE);
				else
					CopyFile(fnElementLogFile,fnElementLogName+t1,FALSE);
			}
			UDeleteFile(fnElementLogFile);
			if((fLog=OpenFile(fnElementLogFile,("wb")))!=NULL)
			{
				fputc(0xfeff,fLog);
				fclose(fLog);
			}
		}
	}
	if(c==NULL)
	{
		if((fLog=OpenFile(fnElementLogFile,("rb")))==NULL)
		{
			if((fLog=OpenFile(fnElementLogFile,("wb")))!=NULL)
			{
				fputc(0xfeff,fLog);
				fclose(fLog);
			}
		}
		else
			fclose(fLog);
		if((fLog=OpenFile(fnElementLogFile,("ab")))!=NULL)
		{
			SYSTEMTIME sTime;
			GetLocalTime(&sTime);
			char s[400];
			sprintf(s,("%d/%d/%d %02d:%02d:%02d"),
				sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
			fputs(s,fLog);
			fputs(cElog,fLog);
			fputs(("\r\n"),fLog);
			fclose(fLog);
		}
		cElog="";
	}
	else
		cElog+=c;
}
bool DeleteElementFile(const char* strFile)
{
	return false;
}
enum
{
	UPDATE_ELEMENT,
	UPDATE_PATCHER,
	UPDATE_LAUNCHER,
	WRONG_PATH,
};
static char strBuffer[MAX_PATH];
static char strReverseBuffer[MAX_PATH];
void ReverseStr(const char* str)
{
	strcpy(strReverseBuffer,str);
	char* p = strReverseBuffer + strlen(str) - 1;
	char* tmp = strReverseBuffer;
	while(p > tmp)
	{
		*p ^= *tmp;
		*tmp ^= *p;
		*p ^= *tmp;
		--p;
		++tmp;
	}
}
int ParseFileName(const char* strFile)
{
	strcpy(strBuffer,strFile);
	strlwr(strBuffer);
	ReverseStr(strBuffer);
	char* pdest;
	if (strFile == NULL || strlen(strFile) == 0)
	{
		return WRONG_PATH;
	}
	if (pdest = strstr(strReverseBuffer, "\\tnemele\\"))
	{
		strcpy(strBuffer,strBuffer + strlen(strReverseBuffer) - (pdest - strReverseBuffer));
		return UPDATE_ELEMENT;
	}
	if (pdest = strstr(strReverseBuffer,"\\rehctap\\"))
	{
		strcpy(strBuffer,strBuffer + strlen(strReverseBuffer) - (pdest - strReverseBuffer));
		return UPDATE_PATCHER;
	}
	if (pdest = strstr(strReverseBuffer,"\\rehcnual\\"))
	{
		strcpy(strBuffer,strBuffer + strlen(strReverseBuffer) - (pdest - strReverseBuffer));
		return UPDATE_LAUNCHER;
	}
	return WRONG_PATH;
}
int UpdateElement(const char* strFile)
{
	if (IsFileInPack(strBuffer))
	{
		AddFileToPack(strBuffer, strFile);
		sprintf(strBuffer, " file in pack updated: %s", strFile);
		WriteElementLog(strBuffer);
		WriteElementLog(NULL);
	}
	else
	{
		AString strDir(szWorkDir);
		strDir += strBuffer;
		MakeDir(strDir, strDir.GetLength());
		if(!DecompressFile(strFile,strDir))
		{
			WriteElementLog(" Decompress File Failed!");
			WriteElementLog(NULL);
			return 0;
		}
		else
		{
			sprintf(strBuffer, " file out of pack updated: %s", strFile);
			WriteElementLog(strBuffer);
			WriteElementLog(NULL);
		}
	}

	return 1;

}
int UpdatePatcher(const char* strFile)
{
	AString strDir(szInstallPath);
	strDir += "patcher\\";
	strDir += strBuffer;
	MakeDir(strDir, strDir.GetLength());
	if(!DecompressFile(strFile,strDir))
	{
		WriteElementLog(" Decompress File Failed!");
		WriteElementLog(NULL);
		return 0;
	}
	else
	{
		sprintf(strBuffer, " file out of pack updated: %s", strFile);
		WriteElementLog(strBuffer);
		WriteElementLog(NULL);
	}
	return 1;
}
int UpdateLauncher(const char* strFile)
{
	AString strDir(szInstallPath);
	strDir += "launcher\\";
	strDir += strBuffer;
	MakeDir(strDir, strDir.GetLength());
	if(!DecompressFile(strFile,strDir))
	{
		WriteElementLog(" Decompress File Failed!");
		WriteElementLog(NULL);
		return 0;
	}
	else
	{
		sprintf(strBuffer, " file out of pack updated: %s", strFile);
		WriteElementLog(strBuffer);
		WriteElementLog(NULL);
	}
	return 1;
}
bool UpdateFile(const char* strFile)
{
	int iParseRet = ParseFileName(strFile);
	int ret;
	switch(iParseRet)
	{
	case UPDATE_ELEMENT:
		ret = UpdateElement(strFile);
		break;
	case UPDATE_PATCHER:
		ret = UpdatePatcher(strFile);
		break;
	case UPDATE_LAUNCHER:
		ret = UpdateLauncher(strFile);
		break;
	default:
		WriteElementLog(" the input string contains no element, patcher or launcher!");
		WriteElementLog(NULL);
		ret = 0;
	}
	return ret == 1;
}