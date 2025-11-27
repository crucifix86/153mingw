   
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

// 回调函数的输入参数为0~100之间的整数
typedef void (CALLBACK*callbackFunc)(int);

enum{
	PCK_INIT_SUCCESS,
	PCK_ERROR_INIT_UPDATE_VERSION_FILE,				// 用于散文件更新的version.sw打开失败
	PCK_ERROR_INIT_LOCAL_VERSION_FILE,				// config\element\version.sw打开失败			
	PCK_ERROR_INIT_VERSION_TOO_LOW,					// 本地版本小于更新包起始更新版本
	PCK_ERROR_INIT_VERSION_TOO_HIGH,				// 更新包版本小于本地版本
	PCK_ERROR_INIT_FILES_CLIENT_NOT_FIT,			// 客户端跟更新包不匹配
	PCK_ERROR_INIT_PCK_OPEN_FAILED,					// 客户端pck文件包打开失败
	PCK_ERROR_INIT_PCK_EMPTY,						// 客户端pck文件包为空
	PCK_ERROR_UPDATE_PCK_OPEN_FAILED,				// 更新包打开失败
	PCK_ERROR_UPDATE_PCK_NO_NEWVERSION,				// 更新包内version文件错误
	PCK_ERROR_UPDATE_PCK_VERSION_EQUAL,				// 版本相同不用更新
	PCK_ERROR_UPDATE_PCK_UPDATE_LIST,				// 更新包更新列表文件错误
	PCK_ERROR_UPDATE_FILE_MD5_FAILED,				// 更新中某个文件md5值与列表中不一致，一般是服务器打包错误
	PCK_ERROR_UPDATE_DECOMPRESS_FAILED,				// 更新中解压文件出错
};
PackExport int PackInitialize(const char* strInstallPath, const char* strVersionFilePath);
PackExport void PackFinalize();
PackExport bool CleanUpPackFile(callbackFunc DisplayProgress = NULL, bool bCleanUpAll = false);
PackExport void FillPackVersion();
PackExport bool UpdateFile(const char* strFile);
PackExport int PackUpdateInit(const char* strInstallPath);
PackExport int PackUpdate(const char* strPackPath, callbackFunc DisplayProgress = NULL);
PackExport void GetUpdateProgress(int& nTotalFiles, int& nCurrentFile);
