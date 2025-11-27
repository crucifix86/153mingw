
#include "p2spStableHeadres.h"
#include "memory_map_file.h"

namespace cdcsr
{
	MemoryMapFile::MemoryMapFile() 
		: handle_(INVALID_HANDLE_VALUE)
		, memory_map_handle_(INVALID_HANDLE_VALUE)
		, view_map_buffer_ptr_(NULL)
		, view_map_begin_pos_(0)
		, length_(0)
		, only_read_(FALSE)
		, dwAllocationGranularity_(0)
		, dwDefaultAllocationGranularity_(0)
	{
		SYSTEM_INFO sinf;
		::GetSystemInfo(&sinf);
		dwDefaultAllocationGranularity_ = sinf.dwAllocationGranularity; //内存分配粒度
	}

	//---------------------------------------------------------------------------------------------------
	MemoryMapFile::~MemoryMapFile()
	{
		close();
	}

	//---------------------------------------------------------------------------------------------------
	VOID MemoryMapFile::close() 
	{
		if(!IsValid()) 
		{
			//DCHECK(view_map_buffer_ptr_ == NULL);
			//DCHECK(memory_map_handle_ == INVALID_HANDLE_VALUE);
			return;
		}

		if(view_map_buffer_ptr_) 
		{
			::UnmapViewOfFile(view_map_buffer_ptr_);
			view_map_buffer_ptr_ = NULL;
		}

		if(memory_map_handle_ != INVALID_HANDLE_VALUE) 
		{
			::CloseHandle(memory_map_handle_);
			memory_map_handle_ = INVALID_HANDLE_VALUE;
		}

		if(handle_ != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(handle_);
			handle_ = INVALID_HANDLE_VALUE;
		}
	}

	//--------------------------------------------------------------------------------------------------------------------
	BOOL MemoryMapFile::remove() 
	{
		if(IsValid()) 
		{
			close();
		}

		return ::DeleteFileW(path_.c_str());
	}

	//--------------------------------------------------------------------------------------------------------------------
	BOOL MemoryMapFile::IsValid() const
	{
		return (handle_ != INVALID_HANDLE_VALUE);
	}

	//---------------------------------------------------------------------------------------------------
	BOOL MemoryMapFile::create(const WCHAR* wszPath, LONGLONG length)
	{
		int result = EnsureDirectoryForFile(wszPath, NULL);
		//if(result != ERROR_SUCCESS) return FALSE;
		//DCHECK(!IsValid());
		only_read_ = FALSE;
		return open_memory_map_file(wszPath, length, TRUE);
	}

	//--------------------------------------------------------------------------------------------------------------------
	BOOL MemoryMapFile::load(const WCHAR* wszPath, BOOL only_read/* = TRUE*/)
	{
		//DCHECK(!IsValid());
		only_read_ = only_read;
		return open_memory_map_file(wszPath, 0, FALSE);
	}

	//--------------------------------------------------------------------------------------------------------------------
	LONGLONG MemoryMapFile::length() const 
	{
		//DCHECK(IsValid());
		return length_; 
	}

	//--------------------------------------------------------------------------------------------------------------------
	std::wstring MemoryMapFile::path() const 
	{
		//DCHECK(IsValid());
		return std::wstring(path_);
	}

	//--------------------------------------------------------------------------------------------------------------------
	DWORD MemoryMapFile::read(LONGLONG offset, LPVOID lpBuffer, DWORD dwSizeOfBuffer)
	{
		//DCHECK(IsValid());
		//DCHECK(lpBuffer && dwSizeOfBuffer);

		if(dwSizeOfBuffer == 0) return 0;
		if(length_ <= offset) return 0;

		LONGLONG offset2end_length = length_ - offset;
		DWORD read_remain_length = dwSizeOfBuffer > offset2end_length ? (DWORD)offset2end_length : dwSizeOfBuffer;
		LONGLONG read_offset = offset;
		LPBYTE read_buffer_pos = (LPBYTE)lpBuffer;

		while(read_remain_length) {
			if(!adjust_memory_map_range(read_offset, read_remain_length)) return 0;

			DWORD pos = (DWORD)(read_offset - view_map_begin_pos_); 
			DWORD length = dwAllocationGranularity_ - pos;
			length = length > read_remain_length ? read_remain_length : length;

			errno_t err = memcpy_s(read_buffer_pos, length, &view_map_buffer_ptr_[pos], length * sizeof(char));
			if(err == 0) {
				read_buffer_pos += length;
				read_offset += length;
				read_remain_length -= length;
			}
			else {
				break;		
			}
		}
		return (DWORD)(read_offset - offset);
	}

	//--------------------------------------------------------------------------------------------------------------------
	// offset = -1时，在文件结尾处添加数据时
	DWORD MemoryMapFile::write(LONGLONG offset, const LPVOID lpBuffer, DWORD dwSizeOfBuffer)
	{
		//DCHECK(IsValid());
		//DCHECK(lpBuffer && dwSizeOfBuffer);

		if(offset == -1)
		{
			LARGE_INTEGER li_size = { 0 };
			li_size.QuadPart = dwSizeOfBuffer + length_;
			DWORD dwPtrLow = ::SetFilePointerEx(handle_, li_size, 0, FILE_BEGIN);
			if(INVALID_SET_FILE_POINTER == dwPtrLow && GetLastError() != NO_ERROR) 
			{
				return 0;
			}
			if(::SetEndOfFile(handle_) == 0) 
			{
				DWORD dw = ::GetLastError();
				return 0;
			}
			offset = length_;
			length_ = li_size.QuadPart;

			if(!createFileMapping()) return 0;
		}
		if(length_ <= offset) return 0;

		LONGLONG offset2end_length = length_ - offset;
		DWORD write_remain_length = dwSizeOfBuffer > offset2end_length ? (DWORD)offset2end_length : dwSizeOfBuffer;
		LONGLONG write_offset = offset;

		while(write_remain_length)
		{
			if(!adjust_memory_map_range(write_offset, write_remain_length)) return 0;

			DWORD pos = (DWORD)(write_offset - view_map_begin_pos_); 
			DWORD length = dwAllocationGranularity_ - pos;
			length = length > write_remain_length ? write_remain_length : length;

			const LPBYTE write_buffer_pos = (const LPBYTE)lpBuffer + (write_offset - offset);
			errno_t err = memcpy_s(&view_map_buffer_ptr_[pos], length, write_buffer_pos, length * sizeof(char));
			if(err == 0)
			{
				::FlushViewOfFile(&view_map_buffer_ptr_[pos], length);

				write_offset += length;
				write_remain_length -= length;
			}
			else
			{
				break;		
			}
		}
		return (DWORD)(write_offset - offset);
	}

	//--------------------------------------------------------------------------------------------------------------------
	BOOL MemoryMapFile::open_memory_map_file(const WCHAR* wszPath, LONGLONG length, BOOL bCreated)
	{
		// 文件必须是关闭状态的
		//DCHECK(!IsValid());

		// 不允许创建0字节的文件
		//if(length == 0 && bCreated) return FALSE;

		//打开硬盘文件
		DWORD dwCreationDisposition = bCreated ? CREATE_ALWAYS : OPEN_EXISTING;
		DWORD dwflags = (only_read_) ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
		handle_ = ::CreateFileW(wszPath, 
			dwflags,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL,
			NULL); 
		if(INVALID_HANDLE_VALUE == handle_) return FALSE;

		LARGE_INTEGER li_size = { 0 };
		::GetFileSizeEx(handle_, &li_size);
		length_ = li_size.QuadPart;

		if(bCreated && length_ != length)
		{
			li_size.QuadPart = length;
			DWORD dwPtrLow = ::SetFilePointerEx(handle_, li_size, 0, FILE_BEGIN);
			if(INVALID_SET_FILE_POINTER == dwPtrLow && GetLastError() != NO_ERROR) 
			{
				return FALSE;
			}
			if(::SetEndOfFile(handle_) == 0) 
			{
				DWORD dw = ::GetLastError();
				return FALSE;
			}
			length_ = length;
		}

		if(length_ > 0)
		{
			createFileMapping();
		}

		path_.assign(wszPath);

		return TRUE;
	}


	//--------------------------------------------------------------------------------------------------------------------
	BOOL MemoryMapFile::adjust_memory_map_range(LONGLONG offset, DWORD length)
	{
		//DCHECK(IsValid());
		//DCHECK(offset < length_);
		//DCHECK(offset + length <= length_);

		if(memory_map_handle_ == INVALID_HANDLE_VALUE || NULL == view_map_buffer_ptr_)
		{
			createFileMapping();
		}
		if(memory_map_handle_ == INVALID_HANDLE_VALUE || NULL == view_map_buffer_ptr_) 
			return FALSE;

		/// 如果offset在当前映射点首位之间
		if(view_map_begin_pos_ <= offset 
			&& offset < view_map_begin_pos_ + dwAllocationGranularity_ 
			) return TRUE;

		/// 计算开始映射位置、映射分配粒度
		view_map_begin_pos_ = offset - (offset % dwDefaultAllocationGranularity_);

		/// 防止溢出, 在系统粒度和映射点到文件尾的长度中取最短的。
		DWORD maxdwAllocationGranularity = (length_ - view_map_begin_pos_) < ULONG_MAX ? (DWORD)(length_ - view_map_begin_pos_) : ULONG_MAX;
		dwAllocationGranularity_ = dwDefaultAllocationGranularity_ < maxdwAllocationGranularity ? dwDefaultAllocationGranularity_ : maxdwAllocationGranularity;

		::UnmapViewOfFile(view_map_buffer_ptr_); view_map_buffer_ptr_ = NULL;
		DWORD dwAccess = only_read_? FILE_MAP_READ : FILE_MAP_ALL_ACCESS;

		DWORD dwFileOffsetHigh = (DWORD)(view_map_begin_pos_ >> 32);
		DWORD dwFileOffsetLow = (DWORD)(view_map_begin_pos_);
		view_map_buffer_ptr_ = (LPBYTE)::MapViewOfFile(memory_map_handle_, dwAccess, dwFileOffsetHigh, dwFileOffsetLow, dwAllocationGranularity_);

		return (view_map_buffer_ptr_ != NULL) && (view_map_begin_pos_ <= offset && offset < view_map_begin_pos_ + dwAllocationGranularity_);
	}

	//--------------------------------------------------------------------------------------------------------------------
	BOOL MemoryMapFile::createFileMapping()
	{
		// 重新确定文件映射粒度
		dwAllocationGranularity_  = dwDefaultAllocationGranularity_ > length_ ? (DWORD)length_ : dwDefaultAllocationGranularity_;
		if(dwDefaultAllocationGranularity_ != 0)
		{
			//创建映射文件
			DWORD dwflagsFileMapping = only_read_ ? PAGE_READONLY : PAGE_READWRITE;
			if(memory_map_handle_ != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(memory_map_handle_); memory_map_handle_ = INVALID_HANDLE_VALUE;
			}
			memory_map_handle_ = ::CreateFileMappingW(handle_, NULL, dwflagsFileMapping, 0, 0, 0);
			if(NULL == memory_map_handle_) 
			{
				::CloseHandle(handle_); handle_ = INVALID_HANDLE_VALUE;
				return FALSE;
			}

			//创建映射视图
			DWORD dwflagsView = only_read_ ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS;
			view_map_buffer_ptr_ = (LPBYTE)::MapViewOfFile(memory_map_handle_, dwflagsView, 0, 0, dwAllocationGranularity_);
			if(NULL == view_map_buffer_ptr_) 
			{
				close();
				return FALSE;
			}

			view_map_begin_pos_ = 0;
		}
		return TRUE;
	}

	//-------------------------------------------------------------------------------------------------------
	/*static*/
	BOOL MemoryMapFile::ReadFileToString( const WCHAR* wszPath, std::string* out_string )
	{
		out_string->clear();

		cdcsr::MemoryMapFile mmf;
		BOOL ok = mmf.load(wszPath, TRUE);
		if(!ok) {
			return FALSE;
		}
		if(mmf.length() == 0) return FALSE;

		DWORD dw = 0;
		size_t pos = 0;
		char buffer[1 << 16];
		while((dw = mmf.read(pos, &buffer[0], sizeof(buffer)))) {
			pos += dw;
			out_string->append(buffer, dw);
			if(pos + 1 == mmf.length()) break;
		}
		return TRUE;
	}

} //namespace cdcsr