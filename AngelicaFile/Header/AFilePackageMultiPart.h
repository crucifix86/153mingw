/*
 * FILE: AFilePackageMultiPart.h
 *
 * DESCRIPTION: Multi-part PCK file handler based on Python extractor logic
 *              Handles .pck, .pkx, .pkx1 files seamlessly
 *
 * CREATED BY: Direct implementation (no WinPCK dependency)
 *
 * HISTORY: 
 *
 * Copyright (c) 2024, All Rights Reserved.
 */

#ifndef _AFILEPACKAGE_MULTIPART_H_
#define _AFILEPACKAGE_MULTIPART_H_

#include "AFilePackBase.h"
#include "AFilePackage.h"
#include <windows.h>
#include <vector>
#include <map>
#include <string>

// PCK version constants
#define AFPCK_VERSION_202 0x00020002
#define AFPCK_VERSION_203 0x00020003

// PCK encryption keys (from Python script) - for litmodels.pck
#define PCK_INDEX_ENTRY_ADDR_KEY 0xA8937462
#define PCK_INDEX_COMPRESS_KEY1  0xA8937462
#define PCK_INDEX_COMPRESS_KEY2  0xF1A43653

// Structure sizes
#define PCK_TAIL_V202_SIZE 280
#define PCK_TAIL_V203_SIZE 288

#pragma pack(push, 1)

// PCK tail structure v2.0.2
struct PCKTAIL_V202 {
	DWORD dwHeadCheckHead;
	DWORD dwVersion0;
	DWORD dwEntryOffset;  // XOR encrypted
	DWORD dwFlags;
	char szAdditionalInfo[252];
	DWORD dwTailCheckTail;
	DWORD dwFileCount;
	DWORD dwVersion;
};

// PCK tail structure v2.0.3
struct PCKTAIL_V203 {
	DWORD dwHeadCheckHead;
	DWORD dwVersion0;
	ULONGLONG qwEntryOffset;  // 64-bit, XOR encrypted
	DWORD dwFlags;
	char szAdditionalInfo[252];
	ULONGLONG qwTailCheckTail;
	DWORD dwFileCount;
	DWORD dwVersion;
};

// File index entry v2.0.2
struct PCKFILEINDEX_V202 {
	char szFileName[256];
	DWORD dwUnknown1;
	DWORD dwOffset;
	DWORD dwFileClearSize;
	DWORD dwFileCipherSize;
	DWORD dwUnknown2;
};

// File index entry v2.0.3
struct PCKFILEINDEX_V203 {
	char szFileName[260];
	DWORD dwUnknown1;
	ULONGLONG qwOffset;  // 64-bit
	DWORD dwFileClearSize;
	DWORD dwFileCipherSize;
	DWORD dwUnknown2;
};

#pragma pack(pop)

class AFilePackageMultiPart : public AFilePackBase
{
public:
	AFilePackageMultiPart();
	virtual ~AFilePackageMultiPart();

	// Open/Close operations
	bool Open(const char* szPckPath, AFilePackage::OPENMODE mode, bool bEncrypt = false);
	bool Open(const char* szPckPath, const char* szFolder, AFilePackage::OPENMODE mode, bool bEncrypt = false);
	virtual bool Close();

	// File operations
	bool ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen);
	bool ReadFile(AFilePackage::FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD* pdwBufferLen);
	bool ReadCompressedFile(const char* szFileName, LPBYTE pCompressedBuffer, DWORD* pdwBufferLen);
	
	// File management (read-only implementation)
	bool AppendFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress) { return false; }
	bool RemoveFile(const char* szFileName) { return false; }
	bool ReplaceFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress) { return false; }
	
	// File entry operations
	bool GetFileEntry(const char* szFileName, AFilePackage::FILEENTRY* pFileEntry, int* pnIndex = NULL);
	const AFilePackage::FILEENTRY* GetFileEntryByIndex(int nIndex);
	int GetFileNumber() const { return m_dwFileCount; }
	
	// Shared file operations
	virtual DWORD OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem);
	virtual void CloseSharedFile(DWORD dwFileHandle);
	
	// Package information
	virtual const char* GetFolder() { return m_szFolder; }
	const char* GetPckFileName() { return m_szPckFileName; }
	virtual bool IsFileExist(const char* szFileName);
	DWORD GetPackageFileSize();
	
	// Cache operations
	void ClearFileCache() {}
	bool AddCacheFileName(const char* szFile) { return true; }
	
	// Static compression functions
	static int Compress(LPBYTE pFileBuffer, DWORD dwFileLength, LPBYTE pCompressedBuffer, DWORD* pdwCompressedLength);
	static int Uncompress(LPBYTE pCompressedBuffer, DWORD dwCompressedLength, LPBYTE pFileBuffer, DWORD* pdwFileLength);

private:
	// File handles for multi-part archive
	struct FilePartInfo {
		HANDLE hFile;
		ULONGLONG qwSize;
		ULONGLONG qwOffset;  // Offset in the combined virtual file
	};
	
	// Cached file entry
	struct FileEntryCache {
		AFilePackage::FILEENTRY entry;
		bool bCompressed;
	};
	
	// Shared file item
	struct SharedFileItem {
		DWORD dwFileID;
		bool bTempMem;
		int iRefCnt;
		BYTE* pFileData;
		DWORD dwFileLen;
		FileEntryCache* pEntry;
	};

	// Member variables
	bool m_bOpened;
	bool m_bReadOnly;
	DWORD m_dwVersion;  // 0x00020002 or 0x00020003
	DWORD m_dwFileCount;
	ULONGLONG m_qwIndexTableOffset;
	
	char m_szPckFileName[MAX_PATH];
	char m_szFolder[MAX_PATH];
	
	std::vector<FilePartInfo> m_fileParts;
	std::vector<FileEntryCache> m_fileEntries;
	std::map<std::string, int> m_fileIndexMap;
	
	std::map<DWORD, SharedFileItem> m_sharedFiles;
	DWORD m_dwNextFileID;
	CRITICAL_SECTION m_csSharedFiles;
	
	// Helper functions
	bool OpenMultiPartFiles(const char* szPckPath);
	bool DetectVersion();
	bool ReadTailData(void* pBuffer, size_t size);
	bool ParseV202Tail(const PCKTAIL_V202* pTail);
	bool ParseV203Tail(const PCKTAIL_V203* pTail);
	bool ReadFileIndexTable();
	bool ReadAtOffset(ULONGLONG qwOffset, void* pBuffer, size_t size);
	ULONGLONG GetTotalSize();
	void NormalizeFileName(const char* szIn, char* szOut);
	DWORD GenerateFileID();
};

#endif // _AFILEPACKAGE_MULTIPART_H_