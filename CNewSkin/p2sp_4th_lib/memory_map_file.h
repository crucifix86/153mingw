
#pragma once

#include <Windows.h>
#include <tchar.h>
#include <limits.h>
#include <string>

namespace cdcsr
{
	class MemoryMapFile
	{
	public:
		MemoryMapFile() ;
		virtual ~MemoryMapFile();

		VOID close();
		BOOL remove();
		BOOL IsValid() const;
		BOOL create(const WCHAR* wszPath, LONGLONG length);
		BOOL load(const WCHAR* wszPath, BOOL only_read = TRUE);
		LONGLONG length() const;
		std::wstring path() const;
		DWORD read(LONGLONG offset, LPVOID lpBuffer, DWORD dwSizeOfBuffer);
		DWORD write(LONGLONG offset, const LPVOID lpBuffer, DWORD dwSizeOfBuffer);

		static BOOL ReadFileToString(const WCHAR* wszPath, std::string* out_string);
	protected:
		BOOL open_memory_map_file(const WCHAR* wszPath, LONGLONG length, BOOL bCreated);
		BOOL adjust_memory_map_range(LONGLONG offset, DWORD length);
		BOOL createFileMapping();

	private:
		HANDLE handle_;							//文件句柄
		HANDLE memory_map_handle_;				//映射文件句柄
		LPBYTE view_map_buffer_ptr_;			//映射视图开始指针
		LONGLONG view_map_begin_pos_;			//当前映射开始offset

		LONGLONG length_;						//文件大小
		std::wstring path_;						//文件路径

		DWORD dwAllocationGranularity_;			//当前映射的长度
		DWORD dwDefaultAllocationGranularity_;	//系统默认的映射的长度

		BOOL only_read_;						//是否只读	
	};
} //namespace base