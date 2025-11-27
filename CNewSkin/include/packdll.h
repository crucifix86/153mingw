   
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PACKDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PACKDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef PACKDLL_EXPORTS
#define PackExport __declspec(dllexport)
#else
#define PackExport __declspec(dllimport)
#endif

typedef bool (CALLBACK*callbackFunc)(int);

PackExport void SetPackVersion(int version);
PackExport bool PackInitialize(bool b=false);
PackExport void PackFinalize();
PackExport bool IsFileInPack(const wchar_t *fn);
PackExport void AddFileToPack(const wchar_t *fn,const wchar_t *tempfn);
PackExport void AddFileToPackUncompressed(const wchar_t *fn,const wchar_t *tempfn);
PackExport void RemoveFileFromPack(const wchar_t *fn);
PackExport bool GetFileFromPack(const wchar_t *fn,DWORD &size,unsigned char *&fbuf);
PackExport bool NeedCleanUpPackFile(bool bCleanUpAll=false);
PackExport void ReleasePackBuf(unsigned char* buf);
PackExport bool CleanUpPackFile(callbackFunc DisplayProgress,bool bCleanUpAll=false);
PackExport bool UseLocalUpdatePack(const wchar_t *fn);
PackExport bool GetLocalUpdateFile(const wchar_t *fnSrc,const wchar_t *fnDes);
PackExport bool CalcCompressFile(const wchar_t *fnSrc,DWORD &size,unsigned char *&fbuf);
PackExport bool DecompressFile(const wchar_t *fnSrc,const wchar_t *fnDes);
PackExport void FillPackVersion(const wchar_t *fn);
PackExport int GetPackVersion();
PackExport bool GetStrValueIniFile(const wchar_t* fn,const char* sectName,const char* key,wchar_t* retValue);
