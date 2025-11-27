/*
 * FILE: AFilePackageWinPCK.h
 *
 * DESCRIPTION: WinPCK wrapper for AFilePackage interface
 *              Provides compatibility layer between Client155 and WinPCK DLL
 *
 * CREATED BY: WinPCK Integration
 *
 * HISTORY: 
 *
 * Copyright (c) 2024, All Rights Reserved.
 */

#ifndef _AFILEPACKAGE_WINPCK_H_
#define _AFILEPACKAGE_WINPCK_H_

#include "AFilePackBase.h"
#include "AFilePackage.h"
#include "../WinPCK_Dependencies/include/pck_handle.h"
#include "../WinPCK_Dependencies/include/pck_default_vars.h"
#include "../WinPCK_Dependencies/include/pck_dependencies.h"
#include <windows.h>
#include <vector>
#include <map>

///////////////////////////////////////////////////////////////////////////
//
//	Class AFilePackageWinPCK
//
///////////////////////////////////////////////////////////////////////////

class AFilePackageWinPCK : public AFilePackBase
{
public:
	AFilePackageWinPCK();
	virtual ~AFilePackageWinPCK();
	
	// Make FileEntryCache public for callback access
	struct FileEntryCache {
		AFilePackage::FILEENTRY entry;
		LPCENTRY pWinPckEntry;
		char szNormalizedName[MAX_PATH];
	};

	// Open/Close operations
	bool Open(const char* szPckPath, AFilePackage::OPENMODE mode, bool bEncrypt = false);
	bool Open(const char* szPckPath, const char* szFolder, AFilePackage::OPENMODE mode, bool bEncrypt = false);
	virtual bool Close();

	// File operations
	bool ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen);
	bool ReadFile(AFilePackage::FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD* pdwBufferLen);
	bool ReadCompressedFile(const char* szFileName, LPBYTE pCompressedBuffer, DWORD* pdwBufferLen);
	
	// File management
	bool AppendFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress);
	bool RemoveFile(const char* szFileName);
	bool ReplaceFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress);
	
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
	void ClearFileCache();
	bool AddCacheFileName(const char* szFile);
	
	// Static compression functions
	static int Compress(LPBYTE pFileBuffer, DWORD dwFileLength, LPBYTE pCompressedBuffer, DWORD* pdwCompressedLength);
	static int Uncompress(LPBYTE pCompressedBuffer, DWORD dwCompressedLength, LPBYTE pFileBuffer, DWORD* pdwFileLength);

	// WinPCK specific
	static bool InitializeWinPCK();
	static void SetAlgorithmId(int id, int customGuardByte0 = 0, int customGuardByte1 = 0, int customMaskDword = 0, int customCheckMask = 0);
	
	// Public helper methods for callback
	void NormalizeFileName(const char* szIn, char* szOut);
	bool ConvertWinPCKEntry(LPCENTRY pWinEntry, AFilePackage::FILEENTRY* pFileEntry, const char* szFileName);
	
	// Member variables accessible by callback
	std::vector<FileEntryCache> m_fileEntries;
	std::map<std::string, int> m_fileIndexMap;

protected:	
	struct SharedFileItem {
		DWORD dwFileID;
		bool bTempMem;
		int iRefCnt;
		BYTE* pFileData;
		DWORD dwFileLen;
		FileEntryCache* pEntry;
	};

private:
	// WinPCK state
	static bool s_bWinPCKInitialized;
	static CRITICAL_SECTION s_csInit;
	bool m_bOpened;
	bool m_bReadOnly;
	
	// Package info
	char m_szPckFileName[MAX_PATH];
	char m_szFolder[MAX_PATH];
	DWORD m_dwFileCount;
	
	
	// Shared files
	std::map<DWORD, SharedFileItem> m_sharedFiles;
	DWORD m_dwNextFileID;
	CRITICAL_SECTION m_csSharedFiles;
	
	// Helper functions
	bool BuildFileEntryCache();
	void ProcessNode(LPCENTRY pNode, const char* path);
	DWORD GenerateFileID();
	static void InitializeAlgorithms();
	static void LoadPckIni();
	void CheckMultiPartFiles(const char* szPckPath);
};

#endif // _AFILEPACKAGE_WINPCK_H_