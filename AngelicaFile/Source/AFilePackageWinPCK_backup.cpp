/*
 * FILE: AFilePackageWinPCK.cpp
 *
 * DESCRIPTION: WinPCK wrapper implementation
 *
 * CREATED BY: WinPCK Integration
 *
 * HISTORY:
 *
 * Copyright (c) 2024, All Rights Reserved.
 */

#include "AFPI.h"
#include "AFI.h"
#include "AFilePackageWinPCK.h"
#include "AMemory.h"
#include "AAssist.h"
#include <algorithm>

#define new A_DEBUG_NEW

// PCK version constants
#define AFPCK_VERSION_203 0x00020003

// Entry type constants  
#define ENTRY_TYPE_FILE   1
#define ENTRY_TYPE_FOLDER 2

// Static members
bool AFilePackageWinPCK::s_bWinPCKInitialized = false;
CRITICAL_SECTION AFilePackageWinPCK::s_csInit;

// Callback data structure for file listing
struct FileListCallbackData {
	AFilePackageWinPCK* pThis;
	const char* currentPath;
};

// Static callback function for pck_listByNode
static void FileListCallback(void* pTag, int32_t entryType, 
	const wchar_t* lpszFilename, int32_t nEntryIndex, 
	uint64_t dwFileLengthLow, uint64_t dwFileLengthHigh, 
	void* pNode)
{
	FileListCallbackData* pData = (FileListCallbackData*)pTag;
	
	if (entryType == ENTRY_TYPE_FILE) {
		// Convert filename to multibyte
		char szFileName[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, lpszFilename, -1, szFileName, MAX_PATH, NULL, NULL);
		
		// Build full path
		char fullPath[MAX_PATH];
		sprintf(fullPath, "%s%s", pData->currentPath, szFileName);
		
		// Create cache entry
		AFilePackageWinPCK::FileEntryCache cache;
		cache.pWinPckEntry = (LPCENTRY)pNode;
		strncpy(cache.szNormalizedName, fullPath, MAX_PATH);
		
		// Convert to FILEENTRY format
		pData->pThis->ConvertWinPCKEntry((LPCENTRY)pNode, &cache.entry, fullPath);
		
		// Add to cache
		int index = pData->pThis->m_fileEntries.size();
		pData->pThis->m_fileEntries.push_back(cache);
		
		// Normalize name for map
		char szNormalized[MAX_PATH];
		pData->pThis->NormalizeFileName(fullPath, szNormalized);
		pData->pThis->m_fileIndexMap[szNormalized] = index;
	}
}

// Constructor
AFilePackageWinPCK::AFilePackageWinPCK()
{
	m_bOpened = false;
	m_bReadOnly = false;
	m_dwFileCount = 0;
	m_dwNextFileID = 1;
	m_szPckFileName[0] = '\0';
	m_szFolder[0] = '\0';
	
	InitializeCriticalSection(&m_csSharedFiles);
	
	// Initialize WinPCK if not already done
	if (!s_bWinPCKInitialized) {
		InitializeWinPCK();
	}
}

// Destructor
AFilePackageWinPCK::~AFilePackageWinPCK()
{
	Close();
	DeleteCriticalSection(&m_csSharedFiles);
}

// Initialize WinPCK DLL
bool AFilePackageWinPCK::InitializeWinPCK()
{
	static bool s_bInitialized = false;
	if (s_bInitialized) {
		return true;
	}
	
	InitializeCriticalSection(&s_csInit);
	
	EnterCriticalSection(&s_csInit);
	if (!s_bWinPCKInitialized) {
		// Initialize the DLL
		pck_init();
		
		// Setup algorithms
		InitializeAlgorithms();
		
		s_bWinPCKInitialized = true;
	}
	LeaveCriticalSection(&s_csInit);
	
	s_bInitialized = true;
	return true;
}

// Initialize algorithm IDs
void AFilePackageWinPCK::InitializeAlgorithms()
{
	// Register known algorithm IDs
	pck_addVersionAlgorithmId(0, AFPCK_VERSION_203);    // Jade Dynasty, Perfect World
	pck_addVersionAlgorithmId(111, AFPCK_VERSION_203);  // Hot Dance Party
	pck_addVersionAlgorithmId(121, AFPCK_VERSION_203);  // Ether Saga Odyssey
	pck_addVersionAlgorithmId(131, AFPCK_VERSION_203);  // Forsaken World
	pck_addVersionAlgorithmId(161, AFPCK_VERSION_203);  // Saint Seiya, Swordsman Online
	pck_addVersionAlgorithmId(171, AFPCK_VERSION_203);  // Client155 (current)
}

// Set algorithm ID
void AFilePackageWinPCK::SetAlgorithmId(int id, int customGuardByte0, int customGuardByte1, int customMaskDword, int customCheckMask)
{
	if (customGuardByte0 || customGuardByte1 || customMaskDword || customCheckMask) {
		// Add custom algorithm
		pck_addVersionAlgorithmId(id, AFPCK_VERSION_203, customGuardByte0, customGuardByte1, customMaskDword, customCheckMask);
	} else {
		// Use default algorithm
		pck_setVersionAlgorithmId(id, AFPCK_VERSION_203);
	}
}

// Open package file
bool AFilePackageWinPCK::Open(const char* szPckPath, AFilePackage::OPENMODE mode, bool bEncrypt)
{
	return Open(szPckPath, "", mode, bEncrypt);
}

bool AFilePackageWinPCK::Open(const char* szPckPath, const char* szFolder, AFilePackage::OPENMODE mode, bool bEncrypt)
{
	// Close any existing file
	Close();
	
	// Convert path to wide char
	wchar_t wszPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, szPckPath, -1, wszPath, MAX_PATH);
	
	// Open the PCK file
	bool bSuccess = (pck_open(wszPath) == PCK_OK);
	if (!bSuccess) {
		return false;
	}
	
	// Store file info
	strncpy(m_szPckFileName, szPckPath, MAX_PATH);
	strncpy(m_szFolder, szFolder, MAX_PATH);
	m_bReadOnly = (mode == AFilePackage::OPENEXIST);
	m_bOpened = true;
	
	// Get file count
	m_dwFileCount = pck_getFileCount();
	
	// Build file entry cache
	BuildFileEntryCache();
	
	return true;
}

// Close package
bool AFilePackageWinPCK::Close()
{
	if (!m_bOpened) {
		return true;
	}
	
	// Clear shared files
	{
		EnterCriticalSection(&m_csSharedFiles);
		for (std::map<DWORD, SharedFileItem>::iterator it = m_sharedFiles.begin(); 
			it != m_sharedFiles.end(); ++it) {
			if (it->second.pFileData) {
				a_free(it->second.pFileData);
			}
		}
		m_sharedFiles.clear();
		LeaveCriticalSection(&m_csSharedFiles);
	}
	
	// Clear cache
	m_fileEntries.clear();
	m_fileIndexMap.clear();
	
	// Close PCK
	pck_close();
	
	m_bOpened = false;
	m_dwFileCount = 0;
	
	return true;
}

// Build file entry cache
bool AFilePackageWinPCK::BuildFileEntryCache()
{
	m_fileEntries.clear();
	m_fileIndexMap.clear();
	
	if (m_dwFileCount == 0) {
		return true;
	}
	
	m_fileEntries.reserve(m_dwFileCount);
	
	// Get root node
	LPCENTRY pRoot = pck_getRootNode();
	if (!pRoot) {
		return false;
	}
	
	// Process all files recursively
	ProcessNode(pRoot, "");
	
	return true;
}

// Process a node recursively
void AFilePackageWinPCK::ProcessNode(LPCENTRY pNode, const char* path)
{
	FileListCallbackData cbData;
	cbData.pThis = this;
	cbData.currentPath = path;
	
	// List files in this node
	pck_listByNode(pNode, &cbData, FileListCallback);
	
	// Process subdirectories
	uint32_t childCount = pck_getChildCount(pNode);
	for (uint32_t i = 0; i < childCount; i++) {
		LPCENTRY pChild = pck_getChild(pNode, i);
		if (pChild && pck_isDirectory(pChild)) {
			// Get directory name
			const wchar_t* wszName = pck_getEntryName(pChild);
			if (wszName && wcscmp(wszName, L"..") != 0) {
				char szDirName[MAX_PATH];
				WideCharToMultiByte(CP_ACP, 0, wszName, -1, szDirName, MAX_PATH, NULL, NULL);
				
				char subPath[MAX_PATH];
				sprintf(subPath, "%s%s\\", path, szDirName);
				ProcessNode(pChild, subPath);
			}
		}
	}
}

// Convert WinPCK entry to FILEENTRY format
bool AFilePackageWinPCK::ConvertWinPCKEntry(LPCENTRY pWinEntry, AFilePackage::FILEENTRY* pFileEntry, const char* szFileName)
{
	if (!pWinEntry || !pFileEntry) {
		return false;
	}
	
	// Copy filename
	strncpy(pFileEntry->szFileName, szFileName, MAX_PATH);
	
	// Get file info
	pFileEntry->dwOffset = (DWORD)pck_getFileOffset(pWinEntry);
	pFileEntry->dwLength = (DWORD)pck_getFileSizeInEntry(pWinEntry);
	pFileEntry->dwCompressedLength = (DWORD)pck_getCompressedSizeInEntry(pWinEntry);
	
	// Set other fields
	pFileEntry->unknown = 0;
	pFileEntry->unknown2 = 0;
	pFileEntry->unknown3 = 0;
	pFileEntry->iAccessCnt = 0;
	
	return true;
}

// Normalize file name
void AFilePackageWinPCK::NormalizeFileName(const char* szIn, char* szOut)
{
	// Convert to lowercase and normalize slashes
	int i = 0;
	while (szIn[i] && i < MAX_PATH-1) {
		if (szIn[i] == '/') {
			szOut[i] = '\\';
		} else {
			szOut[i] = tolower(szIn[i]);
		}
		i++;
	}
	szOut[i] = '\0';
	
	// Remove leading ".\"
	if (szOut[0] == '.' && szOut[1] == '\\') {
		memmove(szOut, szOut + 2, strlen(szOut) - 1);
	}
}

// Read file
bool AFilePackageWinPCK::ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen)
{
	AFilePackage::FILEENTRY fileEntry;
	if (!GetFileEntry(szFileName, &fileEntry)) {
		return false;
	}
	
	return ReadFile(fileEntry, pFileBuffer, pdwBufferLen);
}

bool AFilePackageWinPCK::ReadFile(AFilePackage::FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD* pdwBufferLen)
{
	if (!m_bOpened || !pFileBuffer || !pdwBufferLen) {
		return false;
	}
	
	// Find cached entry
	char szNormalized[MAX_PATH];
	NormalizeFileName(fileEntry.szFileName, szNormalized);
	
	std::map<std::string, int>::iterator it = m_fileIndexMap.find(szNormalized);
	if (it == m_fileIndexMap.end()) {
		return false;
	}
	
	FileEntryCache& cache = m_fileEntries[it->second];
	
	// Check buffer size
	if (*pdwBufferLen < fileEntry.dwLength) {
		*pdwBufferLen = fileEntry.dwLength;
		return false;
	}
	
	// Read file data
	size_t bytesRead = 0;
	if (pck_getFileContentByEntry(cache.pWinPckEntry, pFileBuffer, &bytesRead) != PCK_OK) {
		return false;
	}
	
	*pdwBufferLen = (DWORD)bytesRead;
	return true;
}

// Read compressed file
bool AFilePackageWinPCK::ReadCompressedFile(const char* szFileName, LPBYTE pCompressedBuffer, DWORD* pdwBufferLen)
{
	// WinPCK doesn't provide direct access to compressed data
	// We'll need to read uncompressed and then compress it
	AFilePackage::FILEENTRY fileEntry;
	if (!GetFileEntry(szFileName, &fileEntry)) {
		return false;
	}
	
	// Allocate temp buffer for uncompressed data
	LPBYTE pTempBuffer = (LPBYTE)a_malloc(fileEntry.dwLength);
	if (!pTempBuffer) {
		return false;
	}
	
	DWORD dwTempLen = fileEntry.dwLength;
	bool bResult = ReadFile(fileEntry, pTempBuffer, &dwTempLen);
	if (bResult) {
		// Compress the data
		DWORD dwCompressedLen = *pdwBufferLen;
		if (Compress(pTempBuffer, dwTempLen, pCompressedBuffer, &dwCompressedLen) == 0) {
			*pdwBufferLen = dwCompressedLen;
		} else {
			bResult = false;
		}
	}
	
	a_free(pTempBuffer);
	return bResult;
}

// Get file entry
bool AFilePackageWinPCK::GetFileEntry(const char* szFileName, AFilePackage::FILEENTRY* pFileEntry, int* pnIndex)
{
	if (!m_bOpened || !szFileName || !pFileEntry) {
		return false;
	}
	
	// Normalize filename
	char szNormalized[MAX_PATH];
	NormalizeFileName(szFileName, szNormalized);
	
	// Find in map
	std::map<std::string, int>::iterator it = m_fileIndexMap.find(szNormalized);
	if (it == m_fileIndexMap.end()) {
		return false;
	}
	
	*pFileEntry = m_fileEntries[it->second].entry;
	if (pnIndex) {
		*pnIndex = it->second;
	}
	
	return true;
}

// Get file entry by index
const AFilePackage::FILEENTRY* AFilePackageWinPCK::GetFileEntryByIndex(int nIndex)
{
	if (!m_bOpened || nIndex < 0 || nIndex >= (int)m_fileEntries.size()) {
		return NULL;
	}
	
	return &m_fileEntries[nIndex].entry;
}

// Check if file exists
bool AFilePackageWinPCK::IsFileExist(const char* szFileName)
{
	if (!m_bOpened || !szFileName) {
		return false;
	}
	
	char szNormalized[MAX_PATH];
	NormalizeFileName(szFileName, szNormalized);
	
	return m_fileIndexMap.find(szNormalized) != m_fileIndexMap.end();
}

// Get package file size
DWORD AFilePackageWinPCK::GetPackageFileSize()
{
	if (!m_bOpened) {
		return 0;
	}
	
	return (DWORD)pck_getPckFileSize();
}

// Shared file operations
DWORD AFilePackageWinPCK::OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem)
{
	if (!m_bOpened || !szFileName) {
		return 0;
	}
	
	// Get file entry
	AFilePackage::FILEENTRY fileEntry;
	int nIndex;
	if (!GetFileEntry(szFileName, &fileEntry, &nIndex)) {
		return 0;
	}
	
	EnterCriticalSection(&m_csSharedFiles);
	
	// Check if already opened
	for (std::map<DWORD, SharedFileItem>::iterator it = m_sharedFiles.begin();
		it != m_sharedFiles.end(); ++it) {
		if (it->second.pEntry == &m_fileEntries[nIndex]) {
			// Already opened, increase ref count
			it->second.iRefCnt++;
			if (ppFileBuf) *ppFileBuf = it->second.pFileData;
			if (pdwFileLen) *pdwFileLen = it->second.dwFileLen;
			DWORD dwID = it->first;
			LeaveCriticalSection(&m_csSharedFiles);
			return dwID;
		}
	}
	
	// Allocate buffer
	BYTE* pBuffer = (BYTE*)a_malloc(fileEntry.dwLength);
	if (!pBuffer) {
		LeaveCriticalSection(&m_csSharedFiles);
		return 0;
	}
	
	// Read file
	DWORD dwLen = fileEntry.dwLength;
	if (!ReadFile(fileEntry, pBuffer, &dwLen)) {
		a_free(pBuffer);
		LeaveCriticalSection(&m_csSharedFiles);
		return 0;
	}
	
	// Create shared item
	SharedFileItem item;
	item.dwFileID = GenerateFileID();
	item.bTempMem = bTempMem;
	item.iRefCnt = 1;
	item.pFileData = pBuffer;
	item.dwFileLen = dwLen;
	item.pEntry = &m_fileEntries[nIndex];
	
	m_sharedFiles[item.dwFileID] = item;
	
	if (ppFileBuf) *ppFileBuf = pBuffer;
	if (pdwFileLen) *pdwFileLen = dwLen;
	
	DWORD dwID = item.dwFileID;
	LeaveCriticalSection(&m_csSharedFiles);
	
	return dwID;
}

void AFilePackageWinPCK::CloseSharedFile(DWORD dwFileHandle)
{
	if (!dwFileHandle) {
		return;
	}
	
	EnterCriticalSection(&m_csSharedFiles);
	
	std::map<DWORD, SharedFileItem>::iterator it = m_sharedFiles.find(dwFileHandle);
	if (it != m_sharedFiles.end()) {
		it->second.iRefCnt--;
		if (it->second.iRefCnt <= 0) {
			// Free resources
			if (it->second.pFileData) {
				a_free(it->second.pFileData);
			}
			m_sharedFiles.erase(it);
		}
	}
	
	LeaveCriticalSection(&m_csSharedFiles);
}

// Generate unique file ID
DWORD AFilePackageWinPCK::GenerateFileID()
{
	return m_dwNextFileID++;
}

// Cache operations
void AFilePackageWinPCK::ClearFileCache()
{
	// WinPCK handles its own caching
}

bool AFilePackageWinPCK::AddCacheFileName(const char* szFile)
{
	// WinPCK handles its own caching
	return true;
}

// File modification operations (not implemented for read-only wrapper)
bool AFilePackageWinPCK::AppendFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress)
{
	// Not implemented in this wrapper
	return false;
}

bool AFilePackageWinPCK::RemoveFile(const char* szFileName)
{
	// Not implemented in this wrapper
	return false;
}

bool AFilePackageWinPCK::ReplaceFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress)
{
	// Not implemented in this wrapper
	return false;
}

// Static compression functions
int AFilePackageWinPCK::Compress(LPBYTE pFileBuffer, DWORD dwFileLength, LPBYTE pCompressedBuffer, DWORD* pdwCompressedLength)
{
	return pck_compress(pCompressedBuffer, (long*)pdwCompressedLength, pFileBuffer, dwFileLength);
}

int AFilePackageWinPCK::Uncompress(LPBYTE pCompressedBuffer, DWORD dwCompressedLength, LPBYTE pFileBuffer, DWORD* pdwFileLength)
{
	return pck_decompress(pFileBuffer, (long*)pdwFileLength, pCompressedBuffer, dwCompressedLength);
}