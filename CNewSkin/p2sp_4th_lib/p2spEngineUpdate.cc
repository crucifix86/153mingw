

#include "p2spStableHeadres.h"
#include "p2spUtility.h"

#include "download.h"
#include "memory_map_file.h"

bool CALLBACK p2spUpdateEngineForRundll32( HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow );

namespace p2sp4
{
	//CFilePath::StripPath
	bool CallRundll32OnCreateProcess(const std::string& cmd_line)
	{
		PROCESS_INFORMATION pi = { 0 }; 
		STARTUPINFOA si = { sizeof(STARTUPINFOA), 0 }; 
		si.wShowWindow = SW_SHOW; 
		si.dwFlags = STARTF_USESHOWWINDOW; 

		LPCSTR lpApplicationName = NULL;
		std::string sCommandLine(cmd_line);
		
		CHAR szCommandLine[MAX_PATH] = { 0 };
		strcat_s(szCommandLine, sCommandLine.c_str());
		
		LPSECURITY_ATTRIBUTES lpProcessAttributes	= NULL;
		LPSECURITY_ATTRIBUTES lpThreadAttributes	= NULL;
		BOOL bInheritHandles						= FALSE;
		DWORD dwCreationFlags						= 0;
		LPVOID lpEnvironment						= NULL;
		LPCSTR lpCurrentDirectory = NULL;

		BOOL fRet = ::CreateProcessA(lpApplicationName, 
			szCommandLine, 
			lpProcessAttributes, 
			lpThreadAttributes, 
			bInheritHandles, 
			dwCreationFlags,
			lpEnvironment,
			lpCurrentDirectory, 
			&si,
			&pi);

		if(!fRet){
			std::stringstream sss;
			sss << "CallRundll32OnCreateProcess failed : err[" << ::GetLastError() << "].\n";
			::OutputDebugStringA(sss.str().c_str());
		} else {
			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);
		}
		return !!fRet;
	}

	//-------------------------------------------------------------------------------------------------------
	bool CreateProcessForUpdateEngine(tUpdateFileInfo* update_file_info)
	{
		/// copy self to ./p2sp_update/
		CFilePath currency_module_path, download_cache_path;
		PathProvider(DIR_MODULE, currency_module_path); //currency_module_path = c:\game\

		download_cache_path = currency_module_path;
		download_cache_path.Append(p2sp4::wszRelativelyPath);
		EnsureDirectory(download_cache_path.value(), NULL); //c:\game\p2sp_update\

		CFilePath src_path, dest_path(download_cache_path);
		PathProvider(FILE_MODULE, src_path);
		dest_path.Append(p2sp4::wszUpdateToolName); //dest = c:\game\p2sp_update\p2sp_4th_lib_o.dll
		//if(!dest_path.IsExists()) 
		{
			CopyFileW(src_path.value(), dest_path.value(), FALSE);
		}
		if(!dest_path.IsExists()) {
			return false;
		}

		// 获取当前目录
		CFilePath OldCurrentDirectory;
		LPWSTR buffer = NULL;
		DWORD buffer_len = 0;
		if( ( buffer_len = ::GetCurrentDirectoryW(buffer_len, NULL) ) > 0 ) {
			buffer = new WCHAR[buffer_len + 1];
			::GetCurrentDirectoryW(buffer_len+1, buffer);
			OldCurrentDirectory = CFilePath(buffer);
			delete [] buffer;
		}

		/// 转化目标目录相对于工作目录的相对路径
		WCHAR wszRelativelyPath[MAX_PATH] = L"";
		BOOL ok = ::PathRelativePathToW(wszRelativelyPath,
			OldCurrentDirectory.value(),
			FILE_ATTRIBUTE_DIRECTORY,
			dest_path.value(),
			FILE_ATTRIBUTE_NORMAL);
		if(!ok) return false;

		//rundll32.exe p2sp_engine.dll" "," "engineRundll32EntryPoint 
		std::wstring wsCommandLine(L"rundll32.exe " + std::wstring(wszRelativelyPath) + L"," L"p2spUpdateEngineForRundll32 ");
		std::wstring wurl = update_file_info->wsUpdateURL + update_file_info->wsZipFilename;
		wsCommandLine += wurl;
		wsCommandLine += L";";
		wsCommandLine += update_file_info->wsZipMD5;

		std::string sCommandLine(WideToUTF8(wsCommandLine));
		return CallRundll32OnCreateProcess(sCommandLine);

 		//std::string surl(WideToUTF8(update_file_info->wsUpdateURL + update_file_info->wsZipFilename + L";" + update_file_info->wsZipMD5));
 		//return p2spUpdateEngineForRundll32( NULL, NULL, (char*)surl.c_str(), SW_HIDE);
	}

} //namespace p2sp4

//-------------------------------------------------------------------------------------------------------
bool CALLBACK p2spUpdateEngineForRundll32( HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow )
{
	/// 60s 之后开始下载
	Sleep(60000);

	///// Whether the check is being downloaded, if it is immediately returned
	std::string read_buffer, url(lpszCmdLine);
	size_t pos = url.find_last_of(';');
	if(pos == std::string::npos) {
		return false;
	}
	std::string zipmd5 = url.substr(pos + 1);
	url.erase(pos);
	for(size_t i = 0; i != 4; ++i) {
		if(cdcsr::SimpleHttp::download(url, &read_buffer)) {
			break;
		}
	} ///如果出错了，那么重复下载几次
	if(read_buffer.empty()) {
		::OutputDebugStringA("download read_buffer is empty!\n");
		return false;
	}
	if(MD5String(read_buffer) != zipmd5) {
		::OutputDebugStringA("download read_buffer md5 != zipmd5!\n");
		return false;
	}

	std::string filename(url);
	pos = filename.find_last_of('/');
	if(pos != std::string::npos) {
		filename.erase(0, pos + 1);
	}
	CFilePath CurrentDirectory;
	PathProvider(DIR_MODULE, CurrentDirectory);
	CFilePath ZipFilenamePath(CurrentDirectory);
	ZipFilenamePath.Append(UTF8ToWide(filename).c_str());
	EnsureDirectoryForFile(ZipFilenamePath.value(), NULL);
	
	/// 文件存在比较md5，相等不需要更新了。	
	if(ZipFilenamePath.IsExists()) {
		if(p2sp4::Utility::FileMD5(ZipFilenamePath.value()) == MD5String(read_buffer)) {
			::OutputDebugStringA("ZipFilenamePath == MD5String(read_buffer)!\n");
			return false;
		}
		/// 不相等，先删除目前的
		if(!::DeleteFileW(ZipFilenamePath.value())) {
			::OutputDebugStringA("delete ZipFilenamePath is failed!\n");
			return false;
		}
	}

	cdcsr::MemoryMapFile mmf;
	BOOL ok = mmf.create(ZipFilenamePath.value(), read_buffer.size());
	if(!ok) {
		return false;
	}
	DWORD dw = mmf.write(0, (const LPVOID)read_buffer.c_str(), (DWORD)read_buffer.size());
	if(dw != read_buffer.size()) {
		mmf.close();
		DeleteFileW(ZipFilenamePath.value());
		::OutputDebugStringA("mmf.write is failed!\n");
		return false;
	}
	mmf.close();

	// 解压缩
	CFilePath pathFile(ZipFilenamePath);
	pathFile.RemoveFileSpec();
	pathFile.Append(p2sp4::wszEngineDllName);
	//pathFile.Append(L".bak");
	pc::myLZARI lzari;
	lzari.UnCompressW(ZipFilenamePath.value(), pathFile.value());

	// 拷贝
	CFilePath pathNormalFile(ZipFilenamePath);
	pathNormalFile.RemoveFileSpec();
	pathNormalFile.RemoveLastPart();
	pathNormalFile.Append(p2sp4::wszEngineDllName);
	if(pathNormalFile.IsExists()) {
		BOOL ok = ::DeleteFileW(pathNormalFile.value());
		if(!ok) {
			::OutputDebugStringA("copy step1 delete pathNormalFile is failed!\n");
			return false; //删除旧文件失败，不更新
		}
	}
	ok = ::CopyFileW(pathFile.value(), pathNormalFile.value(), FALSE);
	if(!ok) {
		::OutputDebugStringA("copy step2 is failed!\n");
	} else { 
		::OutputDebugStringA("update ok!\n");
	}
	return !!ok;
}