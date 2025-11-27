// CupReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
#include "tchar.h"

#include "AFI.h"
#include "AFilePackage.h"


#include "..\Base64.h"

char ElementDir[MAX_PATH];
AFilePackage LocalUpdateFilePack;
Base64Code base64;
const int CodePage=936;

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

void Init(const char* strPath)
{	
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   _splitpath(strPath, drive, dir, fname, ext);

	GetModuleFileNameA(NULL, ElementDir, MAX_PATH);
	int i=strlen(ElementDir)-1;
	while(ElementDir[i]!='\\')
		i--;
	strcat(fname,"\\element\\");
	strcpy(ElementDir+i+1,fname);
	MakeDir(ElementDir,strlen(ElementDir));
	af_Initialize();
	af_SetBaseDir(ElementDir);
	SetCurrentDirectory(ElementDir);
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
		desName[k]=TEXT('\0');
		return TRUE;
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
void UDeleteFile(const char* src)
{
	SetFileAttributes(src, FILE_ATTRIBUTE_NORMAL);
	DeleteFile(src);
}
int main(int argc, char* argv[])
{
	if (argv[1] == NULL)
	{
	   printf("no cup file path input.\n");
	   return -1;
	}
	// a_init
	Init(argv[1]);

	if (!LocalUpdateFilePack.Open(argv[1],AFilePackage::OPENEXIST))
	{
		printf("cup文件打开失败\n");
		return -1;
	}
	
	// 取得cup文件内的文件列表 cup可能包含多个版本的更新，取版本差最大的更新列表文件作为文件列表
	char SepFileName[MAX_PATH];
	char CodedFileName[MAX_PATH];
	char IncNumber[MAX_PATH];
	char IncFile[MAX_PATH];

	int FileNum = LocalUpdateFilePack.GetFileNumber();
	int inc = 1;
	for (int i = 0;i < FileNum;++i)
	{
		AFilePackage::FILEENTRY entry = *LocalUpdateFilePack.GetFileEntryByIndex(i);
		char *pdest = strstr(entry.szFileName,".inc");
		if (pdest)
		{
			strcpy(IncNumber,entry.szFileName + 2);
			int len = strlen(IncNumber);
			IncNumber[len - 4] = 0;
			int num = atoi(IncNumber);
			if (num > inc)
			{
				inc = num;
			}
		}
	}
	sprintf(IncFile,"v-%d.inc",inc);
	if (!GetLocalUpdateFile(IncFile,IncFile))
	{
		printf("inc 文件读取失败!\n");
		return -1;	
	}
	
	FILE* FileInc = OpenFile(IncFile,"r");

	char s[400];
	fgets(s,400,FileInc);
	int versionFrom,versionTo;
	char o[30];
	sscanf(s,"%s%d%d",o,&versionFrom,&versionTo);
	printf("起始版本: %d , 结束版本: %d \n",versionFrom,versionTo);

	int iTotalLine = 0;
	// 计算共有多少个文件需要解压
	fseek(FileInc,0,SEEK_SET);
	while(fgets(s,400,FileInc)!=NULL)
	{
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		iTotalLine++;
	}
	if (iTotalLine <= 0)
	{
		printf("cup文件里没有文件!\n");
		return -1;
	}
	int iLineNumber = 0;
	fseek(FileInc,0,SEEK_SET);
	char md5[100];
	FILE* FileVersion = NULL;
	FILE* FileError = NULL;
	FILE* FilePid = NULL;
	int iPid = 0;
	// 生成标记文件，内含最新版本号、pid以及本次更新中要删除的文件
	FileVersion = OpenFile(AString(ElementDir) + "version.sw","wb");
	if (FileVersion == NULL)
	{
		printf("version.sw 创建失败!\n");
	}
	if (!GetLocalUpdateFile("pid","pid"))
	{
		printf("pid 文件读取失败!\n");
		return -1;	
	}
	FilePid = OpenFile("pid","r");
	if (FilePid == NULL)
	{
		printf("pid 文件打开失败!\n");
		return -1;
	}
	fscanf(FilePid,"%d",&iPid);
	fclose(FilePid);
	UDeleteFile("pid");
	fprintf(FileVersion, "%d %d %d", iPid, versionFrom, versionTo);
	do
	{
		if(fgets(s,400,FileInc)==NULL)
			break;
		if(s[0]=='#')
			continue;
		if(strcmp(s,"-----BEGIN ELEMENT SIGNATURE-----\n")==0)
			break;
		sscanf(s,"%s %s",md5,CodedFileName);
		TransferName(CodedFileName,SepFileName);
		AString NameWithOutDot("element/");
		NameWithOutDot += CodedFileName;
		int i = 0;
		while (i < NameWithOutDot.GetLength() - 2)
		{
			if (NameWithOutDot[i] == '.' && NameWithOutDot[i + 1] == '/')
				NameWithOutDot = NameWithOutDot.Left(i) + NameWithOutDot.Right(NameWithOutDot.GetLength() - i - 2);
			else
				i++;
		}

		if(md5[0]=='-')
		{

			fprintf(FileVersion,"%s\n",SepFileName);
		}
		else
		{
			AString strDest(AString(ElementDir) + SepFileName);
			MakeDir(strDest,strDest.GetLength());
			if (!GetLocalUpdateFile(NameWithOutDot,strDest))
			{
				if (FileError == NULL)
				{
					FileError = OpenFile("FileError.txt","wb");
				}
				fprintf(FileError,"%s\n",SepFileName);	
			}
			else
			{
				iLineNumber++;
				printf("\r%d/%d",iLineNumber,iTotalLine);
			}
		}
	}while (true);

	if (iLineNumber != iTotalLine)
	{
		printf("cup文件损坏，某些文件没有解压成功!");
	}
	else
	{
		printf("\nsuccess!");
	}

	if (FileVersion)
	{
		fclose(FileVersion);
	}
	if (FileError)
	{
		fclose(FileError);
	}
	fclose(FileInc);
	UDeleteFile(IncFile);

	return 0;
}

