    // PackDLL.cpp : Defines the entry point for the DLL application.
//


#include "stdafx.h"



#include "AFI.h"
#include "AFilePackage.h"
#include "PackDLL.h"
#include "elementpckdir.h"
#include <DIRECT.H>
#include "AIniFile.h"
#include <AFilePackMan.h>


const int CleanUpMinSize=15000000;
const double CleanUpMinPercent=0.7;
const unsigned int CleanUpMinFragmentSize = 200 * 1024 * 1024;

const wchar_t fnErrorLog[]=L"../config/element/fileerr.log";
char szWorkDir[400];
int PackVersion = 1;
double CleanupFileSize;
double MinFreeDiskSize;
AFilePackage LocalUpdateFilePack;

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


void SetPackVersion(int version)
{
	PackVersion = version;
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
	TCHAR t[400];
	strcpy(t,dir);
	t[r]='\0';
	_rmdir(t);
	DeleteDir(dir,r);
}

bool bInit=false;

PackExport bool CalcCompressFile(const wchar_t *fnSrc,DWORD &size,unsigned char *&fbuf)
{
	fbuf=NULL;
	char cfnSrc[400];
	char dc=' ';
	WideCharToMultiByte(936,0,fnSrc,-1,cfnSrc,400,&dc,false);
	int fsize=GetFileSize(cfnSrc);
	if(fsize==0xFFFFFFFF)
		return false;
	FILE *f=OpenFile(cfnSrc,"rb");
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

PackExport bool DecompressFile(const wchar_t *fnSrc,const wchar_t *fnDes)
{
	char cfnSrc[400];
	char dc=' ';
	WideCharToMultiByte(CP_ACP,0,fnSrc,-1,cfnSrc,400,&dc,false);
	SetFileAttributesW(fnDes, FILE_ATTRIBUTE_NORMAL);
	int fsize=GetFileSize(cfnSrc);
	if(fsize==0xFFFFFFFF||fsize<4)
		return false;
	FILE *f=OpenFile(cfnSrc,"rb");
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
		f=OpenWFile(fnDes,L"wb");
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
	f=OpenWFile(fnDes,L"wb");
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

PackExport bool UseLocalUpdatePack(const wchar_t *fn)
{
	LocalUpdateFilePack.Close();
	char cfn[400];
	char dc=' ';
	WideCharToMultiByte(CP_ACP,0,fn,-1,cfn,400,&dc,false);
	return LocalUpdateFilePack.Open(cfn,AFilePackage::OPENEXIST);
}

PackExport bool GetLocalUpdateFile(const wchar_t *fnSrc,const wchar_t *fnDes)
{
	char cfnSrc[400];
	char dc=' ';
	WideCharToMultiByte(936,0,fnSrc,-1,cfnSrc,400,&dc,false);
	AFilePackage::FILEENTRY entry;
	unsigned char *fbuf=NULL;
	if(LocalUpdateFilePack.GetFileEntry(cfnSrc,&entry))
	{
		ULONG size=entry.dwLength;
		fbuf=new unsigned char[size+2];
		LocalUpdateFilePack.ReadFile(cfnSrc,fbuf,&size);
		SetFileAttributesW(fnDes, FILE_ATTRIBUTE_NORMAL);
		FILE* f=OpenWFile(fnDes,L"wb");
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

PackExport bool PackInitialize(bool b)
{
	GetModuleFileNameA(NULL, szWorkDir, MAX_PATH);
	int i=strlen(szWorkDir)-1;
	while(szWorkDir[i]!='\\')
		i--;
	i--;
	while(szWorkDir[i]!='\\')
		i--;
	strcpy(szWorkDir+i+1,"element\\");
	MakeDir("..\\element",10);

	af_Initialize();
	af_SetBaseDir(szWorkDir);
	char pckFn[400];

	int PackFileNum = sizeof(g_szPckDir1) / sizeof(const char*);
	if( PackVersion == 2 )
		PackFileNum = sizeof(g_szPckDir2) / sizeof(const char*);
	for(i=0; i<PackFileNum; i++)
	{
		char szPckFile[MAX_PATH];
		if( PackVersion == 2 )
			sprintf(szPckFile, "%s.pck", g_szPckDir2[i]);
		else
			sprintf(szPckFile, "%s.pck", g_szPckDir1[i]);
		sprintf(pckFn,"%s%s","..\\element\\",szPckFile);
		MakeDir(pckFn,strlen(pckFn));
		SetFileAttributesA(pckFn, FILE_ATTRIBUTE_NORMAL);
		if(!g_AFilePackMan.OpenFilePackage(szPckFile))
		{
			if(b)
				g_AFilePackMan.OpenFilePackage(szPckFile,true);
			else
			{
				g_AFilePackMan.CloseAllPackages();
				af_Finalize();
				return false;
			}
		}
		else if(!b)
		{
			AFilePackage *pack=(AFilePackage*)g_AFilePackMan.GetFilePck(i);
			if(pack->GetFileNumber()==0)
			{
				g_AFilePackMan.CloseAllPackages();
				af_Finalize();
				return false;
			}
		}
	}
	bInit=true;
	return true;
}

PackExport void PackFinalize()
{
	if(bInit)
	{
		bInit=false;
		g_AFilePackMan.CloseAllPackages();
		af_Finalize();
	}
}

PackExport bool IsFileInPack(const wchar_t *fn)
{
	int k=0;
	while(fn[k]=='.'&&fn[k+1]=='/')
		k+=2;
	char cfn[400];
	char dc=' ';
	WideCharToMultiByte(936,0,fn+k,-1,cfn,400,&dc,false);
	AFilePackage *fPack=(AFilePackage*)g_AFilePackMan.GetFilePck(cfn);
	return (fPack!=NULL);
}

PackExport void AddFileToPackUncompressed(const wchar_t *fn,const wchar_t *tempfn)
{
	int k=0;
	while(fn[k]=='.'&&fn[k+1]=='/')
		k+=2;
	char cfn[400],ctempfn[400];
	char dc=' ';
	int fsize;
	WideCharToMultiByte(936,0,tempfn,-1,ctempfn,400,&dc,false);
	fsize=GetFileSize(ctempfn);
	if(fsize==0xFFFFFFFF||fsize<4)
		return;
	
	WideCharToMultiByte(936,0,fn+k,-1,cfn,400,&dc,false);
	int p=0;
	AFilePackage *fPack=(AFilePackage*)g_AFilePackMan.GetFilePck(cfn);
	if(fPack!=NULL)
	{
		unsigned char *fbuf=new unsigned char[fsize+1];
		FILE *f=OpenFile(ctempfn,"rb");
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

PackExport void AddFileToPack(const wchar_t *fn,const wchar_t *tempfn)
{
	int k=0;
	while(fn[k]=='.'&&fn[k+1]=='/')
		k+=2;
	char cfn[400],ctempfn[400];
	char dc=' ';
	int fsize;
	WideCharToMultiByte(936,0,tempfn,-1,ctempfn,400,&dc,false);
	fsize=GetFileSize(ctempfn);
	if(fsize==0xFFFFFFFF||fsize<4)
		return;
	
	WideCharToMultiByte(936,0,fn+k,-1,cfn,400,&dc,false);
	int p=0;
	AFilePackage *fPack=(AFilePackage*)g_AFilePackMan.GetFilePck(cfn);
	if(fPack!=NULL)
	{
		unsigned char *fbuf=new unsigned char[fsize+1];
		FILE *f=OpenFile(ctempfn,"rb");
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

//		if(!bQuick)
//		{
//			char pckFn[200];
//			strcpy(pckFn,fPack->GetPckFileName());
//			fPack->Close();
//			fPack->Open(pckFn,AFilePackage::OPENEXIST);
//		}
	}
//	else
//	{
//		char cpath[400];
//		strcpy(cpath,szWorkDir);
//		int ilen=strlen(cpath);
//		strcpy(cpath+ilen,cfn);
//		MakeDir(cpath,strlen(cpath));
//		SetFileAttributes(cpath, FILE_ATTRIBUTE_NORMAL);
//		if(!CopyFile(ctempfn,cpath,FALSE))
//		{
//			FILE *f=_wOpenFile(fnErrorLog,L"ab");
//			if(f!=NULL)
//			{
//				wchar_t path[400];
//				int len=MultiByteToWideChar(936,0,cpath,-1,path,400);
//				path[len]='\0';
//				SYSTEMTIME sTime;
//				GetLocalTime(&sTime);
//				wchar_t s[400];
//				swprintf(s,L"%d/%d/%d %02d:%02d:%02d",
//						sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
//				fputws(s,f);
//				swprintf(s,L"复制文件%s到%s的操作失败",tempfn,path);
//				fputws(s,f);
//				fputws(L"\r\n",f);
//				fclose(f);
//			}
//		}
//	}
}
PackExport void RemoveFileFromPack(const wchar_t *fn)
{
	int k=0;
	while(fn[k]=='.'&&fn[k+1]=='/')
		k+=2;
	char cfn[400];
	char dc=' ';
	WideCharToMultiByte(936,0,fn+k,-1,cfn,400,&dc,false);
	int p=0;

	AFilePackage *fPack=(AFilePackage*)g_AFilePackMan.GetFilePck(cfn);
	if(fPack!=NULL)
	{
		
		AFilePackage::FILEENTRY entry;
		if(fPack->GetFileEntry(cfn,&entry))
		{
			fPack->RemoveFile(cfn);
//			if(!bQuick)
//			{
//				char pckFn[200];
//				strcpy(pckFn,fPack->GetPckFileName());
//				fPack->Close();
//				fPack->Open(pckFn,AFilePackage::OPENEXIST);
//			}
		}
	}
//	else
//	{
//		char cpath[400];
//		strcpy(cpath,szWorkDir);
//		int ilen=strlen(cpath);
//		strcpy(cpath+ilen,cfn);
//		SetFileAttributes(cpath, FILE_ATTRIBUTE_NORMAL);
//		DeleteFile(cpath);
//		DeleteDir(cpath,strlen(cpath));
//	}
}

PackExport bool GetFileFromPack(const wchar_t *fn,DWORD &size,unsigned char *&fbuf)
{
	fbuf=NULL;
	int k=0;
	while(fn[k]=='.'&&fn[k+1]=='/')
		k+=2;
	char cfn[400];
	char dc=' ';
	WideCharToMultiByte(936,0,fn+k,-1,cfn,400,&dc,false);
	int p=0;
	AFilePackage *fPack=(AFilePackage*)g_AFilePackMan.GetFilePck(cfn);
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

PackExport bool NeedCleanUpPackFile(bool bCleanUpAll)
{
	PackFinalize();
	af_Initialize();
	CleanupFileSize=0;
	MinFreeDiskSize=0;
	int PackFileNum = sizeof(g_szPckDir1) / sizeof(const char*);
	if( PackVersion == 2 )
		PackFileNum = sizeof(g_szPckDir2) / sizeof(const char*);
	for(int i=0; i<PackFileNum; i++)
	{
		char szPckFile[MAX_PATH];
		if( PackVersion == 2 )
			sprintf(szPckFile, "%s.pck", g_szPckDir2[i]);
		else
			sprintf(szPckFile, "%s.pck", g_szPckDir1[i]);
		char pckFn[400];
		sprintf(pckFn,"%s%s","..\\element\\",szPckFile);

		MakeDir(pckFn,strlen(pckFn));
		SetFileAttributesA(pckFn, FILE_ATTRIBUTE_NORMAL);
		int pfilesize=GetFileSize(pckFn);
		if(!bCleanUpAll&&pfilesize<CleanUpMinSize)
			continue;
		if(pfilesize>MinFreeDiskSize)
			MinFreeDiskSize=pfilesize;
		AFilePackage *fPack=new AFilePackage;
		if(fPack->Open(pckFn,AFilePackage::OPENEXIST))
		{
			int sumfiles=fPack->GetFileNumber();
			int totfilesize=0;
			for(int j=0;j<sumfiles;j++)
			{
				const AFilePackage::FILEENTRY* fe = fPack->GetFileEntryByIndex(j);
				if (fe)				
					totfilesize+=fe->dwCompressedLength;								
			}
			if(bCleanUpAll||(double)totfilesize/pfilesize<CleanUpMinPercent)
				CleanupFileSize+=totfilesize;
			fPack->Close();
		}
		delete fPack;
	}
	af_Finalize();
	return (CleanupFileSize>0);
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
	const AFilePackage::FILEENTRY* entry;
	int FinishFileSize=0;
	int PackFileNum = sizeof(g_szPckDir1) / sizeof(const char*);
	if( PackVersion == 2 )
		PackFileNum = sizeof(g_szPckDir2) / sizeof(const char*);
	char szPckFile[MAX_PATH];
	char szPkxFile[MAX_PATH];
	for(int i=0; i<PackFileNum; i++)
	{
		if( PackVersion == 2 )
		{
			sprintf(szPckFile, "..\\element\\%s.pck", g_szPckDir2[i]);
			sprintf(szPkxFile, "..\\element\\%s.pkx", g_szPckDir2[i]);
		}
		else
		{
			sprintf(szPckFile, "..\\element\\%s.pck", g_szPckDir1[i]);
			sprintf(szPkxFile, "..\\element\\%s.pkx", g_szPckDir1[i]);
		}

		int pfilesize=GetFileSize(szPckFile);
		if (pfilesize == -1) continue;		
		int pkxfilesize = GetFileSize(szPkxFile);
		unsigned int PackSize = pfilesize;
		if (pkxfilesize != -1)
		{
			PackSize += pkxfilesize;
		}
		if(!bCleanUpAll&&PackSize<CleanUpMinSize)
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
		unsigned int totfilesize=0;
		for(int j=0;j<sumfiles;j++)
		{
			const AFilePackage::FILEENTRY* fe = fPack->GetFileEntryByIndex(j);
			if(fe)
				totfilesize+=fe->dwCompressedLength;
		}
		if(bCleanUpAll||(double)totfilesize/PackSize<CleanUpMinPercent || PackSize - totfilesize > CleanUpMinFragmentSize)
		{
			AFilePackage *newpack=new AFilePackage;
			if(newpack->Open("..\\config\\element\\temp.pck",AFilePackage::CREATENEW))
			{
				for(int j=0;j<sumfiles;j++)
				{
					entry=fPack->GetFileEntryByIndex(j);
					if(!entry) continue;

					DWORD dwCompressedL = entry->dwCompressedLength;

					unsigned char *fbuf=new unsigned char[entry->dwCompressedLength+1];
					fPack->ReadCompressedFile(entry->szFileName,fbuf,&dwCompressedL);
					newpack->AppendFileCompressed(entry->szFileName,fbuf,entry->dwLength,dwCompressedL);
					FinishFileSize+=dwCompressedL;
					delete []fbuf;
					if(!DisplayProgress(((double)FinishFileSize)/totfilesize*100))
					{
						newpack->Close();
						fPack->Close();
						delete fPack;
						delete newpack;
						af_Finalize();
						DeleteFile("..\\config\\element\\temp.pck");
						return true; 
					};
				}
				newpack->Close();
				fPack->Close();
				delete fPack;
				delete newpack;
				if (MoveFileEx("..\\config\\element\\temp.pck",szPckFile,MOVEFILE_REPLACE_EXISTING)
					&& pkxfilesize != -1) DeleteFile(szPkxFile);
				MoveFileEx("..\\config\\element\\temp.pkx",szPkxFile,MOVEFILE_REPLACE_EXISTING);
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

PackExport void ReleasePackBuf(unsigned char* buf)
{
	delete []buf;
}

PackExport void FillPackVersion(const wchar_t *fn)
{
	char cfn[400];
	char dc=' ';
	WideCharToMultiByte(936,0,fn,-1,cfn,400,&dc,false);
	FILE *f=OpenFile(cfn,"rb");
	if( !f ) 
		return;
	unsigned char *fbuf=new unsigned char[100];
	int fsize=fread(fbuf,sizeof(char),100,f);
	fclose(f);
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
		AFilePackage *fPack=(AFilePackage*)g_AFilePackMan.GetFilePck(fnDes);
		AFilePackage::FILEENTRY entry;
		if(fPack->GetFileEntry(fnDes,&entry))
			fPack->ReplaceFile(fnDes,fbuf,fsize,true);
		else
			fPack->AppendFile(fnDes,fbuf,fsize,true);
	}
	delete []fbuf;
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
		AFilePackage *fPack=(AFilePackage*)g_AFilePackMan.GetFilePck(fnDes);
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