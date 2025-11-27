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
#include <functional>

#define new A_DEBUG_NEW

// PCK version constants
#define AFPCK_VERSION_203 0x00020003

// Entry type constants
#define ENTRY_TYPE_FILE   1
#define ENTRY_TYPE_FOLDER 2

// Static members
bool AFilePackageWinPCK::s_bWinPCKInitialized = false;
CRITICAL_SECTION AFilePackageWinPCK::s_csInit;

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

// Initialize WinPCK library
bool AFilePackageWinPCK::InitializeWinPCK()
{
	static bool s_bInitStarted = false;
	
	EnterCriticalSection(&s_csInit);
	
	if (s_bWinPCKInitialized) {
		LeaveCriticalSection(&s_csInit);
		return true;
	}
	
	if (!s_bInitStarted) {
		s_bInitStarted = true;
		InitializeCriticalSection(&s_csInit);
		
		// Initialize known algorithms
		InitializeAlgorithms();
		
		// Set default settings
		pck_setMaxThread(4);
		pck_setCompressLevel(9);
		pck_setMTMaxMemory(256 * 1024 * 1024); // 256MB
		
		s_bWinPCKInitialized = true;
	}
	
	LeaveCriticalSection(&s_csInit);
	return true;
}

// Initialize known game algorithms
void AFilePackageWinPCK::InitializeAlgorithms()
{
	// Add all known algorithms
	pck_addVersionAlgorithmId(0, AFPCK_VERSION_203);    // Jade Dynasty, Perfect World
	pck_addVersionAlgorithmId(111, AFPCK_VERSION_203);  // Hot Dance Party
	pck_addVersionAlgorithmId(121, AFPCK_VERSION_203);  // Ether Saga Odyssey
	pck_addVersionAlgorithmId(131, AFPCK_VERSION_203);  // Forsaken World
	pck_addVersionAlgorithmId(161, AFPCK_VERSION_203);  // Saint Seiya, Swordsman Online
	pck_addVersionAlgorithmId(171, AFPCK_VERSION_203);  // Client155 default
}

// Set custom algorithm
void AFilePackageWinPCK::SetAlgorithmId(int id, int customGuardByte0, int customGuardByte1, int customMaskDword, int customCheckMask)
{
	if (customGuardByte0 || customGuardByte1 || customMaskDword || customCheckMask) {
		pck_addVersionAlgorithmIdByKeys(id, AFPCK_VERSION_203, L"Custom", 
			customGuardByte0, customGuardByte1, customMaskDword, customCheckMask);
	} else {
		pck_addVersionAlgorithmId(id, AFPCK_VERSION_203);
	}
}

// Open PCK file
bool AFilePackageWinPCK::Open(const char* szPckPath, AFilePackage::OPENMODE mode, bool bEncrypt)
{
	return Open(szPckPath, "", mode, bEncrypt);
}

bool AFilePackageWinPCK::Open(const char* szPckPath, const char* szFolder, AFilePackage::OPENMODE mode, bool bEncrypt)
{
	if (m_bOpened) {
		Close();
	}
	
	// Convert to wide string
	wchar_t wszPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, szPckPath, -1, wszPath, MAX_PATH);
	
	// For CREATENEW mode, we need to handle it differently
	if (mode == AFilePackage::CREATENEW) {
		// WinPCK doesn't support creating empty PCK files directly
		// We'll need to use the create API with dummy content
		return false; // TODO: Implement if needed
	}
	
	// Open existing PCK
	PCKRTN ret = pck_open(wszPath);
	if (ret != WINPCK_OK) {
		return false;
	}
	
	// Check if valid
	if (!pck_IsValidPck()) {
		pck_close();
		return false;
	}
	
	// Save info
	strncpy(m_szPckFileName, szPckPath, MAX_PATH);
	strncpy(m_szFolder, szFolder, MAX_PATH);
	
	// Ensure folder ends with backslash
	int folderLen = strlen(m_szFolder);
	if (folderLen > 0 && m_szFolder[folderLen-1] != '\\') {
		m_szFolder[folderLen] = '\\';
		m_szFolder[folderLen+1] = '\0';
	}
	
	// Get file count
	m_dwFileCount = pck_filecount();
	
	// Build file entry cache
	if (!BuildFileEntryCache()) {
		pck_close();
		return false;
	}
	
	// Check if read-only (try to get a test write handle)
	m_bReadOnly = !pck_isSupportAddFileToPck();
	
	m_bOpened = true;
	return true;
}

// Close PCK file
bool AFilePackageWinPCK::Close()
{
	if (!m_bOpened) {
		return true;
	}
	
	// Close all shared files
	EnterCriticalSection(&m_csSharedFiles);
	for (auto& pair : m_sharedFiles) {
		if (pair.second.bTempMem) {
			a_freetemp(pair.second.pFileData);
		} else {
			a_free(pair.second.pFileData);
		}
	}
	m_sharedFiles.clear();
	LeaveCriticalSection(&m_csSharedFiles);
	
	// Clear caches
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
	
	// Function to recursively process entries
	std::function<void(LPCENTRY, const std::string&)> processNode;
	processNode = [this, &processNode](LPCENTRY pNode, const std::string& path) {
		// List files in this node
		struct CallbackData {
			AFilePackageWinPCK* pThis;
			std::string currentPath;
		} cbData = { this, path };
		
		pck_listByNode(pNode, &cbData, [](void* pTag, int entryType, 
			const wchar_t* lpszFilename, const void* pNode, 
			DWORD dwFileLengthLow, DWORD dwFileLengthHigh, 
			DWORD dwCompressedLength, void* _other) {
			
			CallbackData* pData = (CallbackData*)pTag;
			
			if (entryType == ENTRY_TYPE_FILE) {
				// Convert filename to multibyte
				char szFileName[MAX_PATH];
				WideCharToMultiByte(CP_ACP, 0, lpszFilename, -1, szFileName, MAX_PATH, NULL, NULL);
				
				// Build full path
				std::string fullPath = pData->currentPath + szFileName;
				
				// Create cache entry
				FileEntryCache cache;
				cache.pWinPckEntry = (LPCENTRY)pNode;
				strncpy(cache.szNormalizedName, fullPath.c_str(), MAX_PATH);
				
				// Convert to FILEENTRY format
				pData->pThis->ConvertWinPCKEntry((LPCENTRY)pNode, &cache.entry, fullPath.c_str());
				
				// Add to cache
				int index = pData->pThis->m_fileEntries.size();
				pData->pThis->m_fileEntries.push_back(cache);
				
				// Normalize name for map
				char szNormalized[MAX_PATH];
				pData->pThis->NormalizeFileName(fullPath.c_str(), szNormalized);
				pData->pThis->m_fileIndexMap[szNormalized] = index;
			}
			else if (entryType == ENTRY_TYPE_FOLDER && wcscmp(lpszFilename, L"..") != 0) {
				// Process subdirectory
				char szDirName[MAX_PATH];
				WideCharToMultiByte(CP_ACP, 0, lpszFilename, -1, szDirName, MAX_PATH, NULL, NULL);
				
				std::string subPath = pData->currentPath + szDirName + "\\";
				processNode((LPCENTRY)pNode, subPath);
			}
		});
	};
	
	// Start processing from root
	processNode(pRoot, "");
	
	return true;
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
	
	// Remove folder prefix if needed
	if (m_szFolder[0] && strnicmp(szOut, m_szFolder, strlen(m_szFolder)) == 0) {
		memmove(szOut, szOut + strlen(m_szFolder), strlen(szOut) - strlen(m_szFolder) + 1);
	}
}

// Read file
bool AFilePackageWinPCK::ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen)
{
	if (!m_bOpened || !szFileName || !pFileBuffer || !pdwBufferLen) {
		return false;
	}
	
	// Find file entry
	AFilePackage::FILEENTRY entry;
	if (!GetFileEntry(szFileName, &entry)) {
		return false;
	}
	
	return ReadFile(entry, pFileBuffer, pdwBufferLen);
}

bool AFilePackageWinPCK::ReadFile(AFilePackage::FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD* pdwBufferLen)
{
	if (!m_bOpened || !pFileBuffer || !pdwBufferLen) {
		return false;
	}
	
	// Check buffer size
	if (*pdwBufferLen < fileEntry.dwLength) {
		return false;
	}
	
	// Find cached entry
	char szNormalized[MAX_PATH];
	NormalizeFileName(fileEntry.szFileName, szNormalized);
	
	auto it = m_fileIndexMap.find(szNormalized);
	if (it == m_fileIndexMap.end()) {
		return false;
	}
	
	FileEntryCache& cache = m_fileEntries[it->second];
	
	// Read file data
	PCKRTN ret = pck_GetSingleFileData(cache.pWinPckEntry, (LPSTR)pFileBuffer, *pdwBufferLen);
	if (ret != WINPCK_OK) {
		return false;
	}
	
	*pdwBufferLen = fileEntry.dwLength;
	return true;
}

// Read compressed file
bool AFilePackageWinPCK::ReadCompressedFile(const char* szFileName, LPBYTE pCompressedBuffer, DWORD* pdwBufferLen)
{
	// WinPCK doesn't provide direct compressed data access
	// We'll need to read uncompressed and compress it
	if (!m_bOpened || !szFileName || !pCompressedBuffer || !pdwBufferLen) {
		return false;
	}
	
	// Find file entry
	AFilePackage::FILEENTRY entry;
	if (!GetFileEntry(szFileName, &entry)) {
		return false;
	}
	
	// Check buffer size
	if (*pdwBufferLen < entry.dwCompressedLength) {
		return false;
	}
	
	// Allocate temp buffer for uncompressed data
	BYTE* pTempBuffer = (BYTE*)a_malloc(entry.dwLength);
	if (!pTempBuffer) {
		return false;
	}
	
	// Read uncompressed data
	DWORD dwUncompressedLen = entry.dwLength;
	if (!ReadFile(szFileName, pTempBuffer, &dwUncompressedLen)) {
		a_free(pTempBuffer);
		return false;
	}
	
	// Compress it
	*pdwBufferLen = entry.dwCompressedLength;
	int result = Compress(pTempBuffer, entry.dwLength, pCompressedBuffer, pdwBufferLen);
	
	a_free(pTempBuffer);
	return (result == 0);
}

// File management operations
bool AFilePackageWinPCK::AppendFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress)
{
	if (!m_bOpened || m_bReadOnly) {
		return false;
	}
	
	// Convert filename
	wchar_t wszFileName[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, szFileName, -1, wszFileName, MAX_PATH);
	
	// Create temp file with data
	char szTempPath[MAX_PATH];
	char szTempFile[MAX_PATH];
	GetTempPathA(MAX_PATH, szTempPath);
	GetTempFileNameA(szTempPath, "pck", 0, szTempFile);
	
	FILE* fp = fopen(szTempFile, "wb");
	if (!fp) {
		return false;
	}
	
	fwrite(pFileBuffer, 1, dwFileLength, fp);
	fclose(fp);
	
	// Convert temp filename
	wchar_t wszTempFile[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, szTempFile, -1, wszTempFile, MAX_PATH);
	
	// Add to string array
	pck_StringArrayReset();
	pck_StringArrayAppend(wszTempFile);
	
	// Get parent entry
	char szPath[MAX_PATH];
	char szName[MAX_PATH];
	strcpy(szPath, szFileName);
	
	char* pLastSlash = strrchr(szPath, '\\');
	if (pLastSlash) {
		*pLastSlash = '\0';
		strcpy(szName, pLastSlash + 1);
	} else {
		szPath[0] = '\0';
		strcpy(szName, szFileName);
	}
	
	wchar_t wszPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);
	
	LPCENTRY pParent = szPath[0] ? pck_getFileEntryByPath(wszPath) : pck_getRootNode();
	
	// Set compression level
	DWORD oldLevel = pck_getCompressLevel();
	pck_setCompressLevel(bCompress ? 9 : 0);
	
	// Submit update
	PCKRTN ret = pck_UpdatePckFileSubmit(wszFileName, pParent);
	
	// Restore compression level
	pck_setCompressLevel(oldLevel);
	
	// Delete temp file
	DeleteFileA(szTempFile);
	
	if (ret == WINPCK_OK) {
		// Rebuild cache
		m_dwFileCount = pck_filecount();
		BuildFileEntryCache();
	}
	
	return (ret == WINPCK_OK);
}

bool AFilePackageWinPCK::RemoveFile(const char* szFileName)
{
	if (!m_bOpened || m_bReadOnly) {
		return false;
	}
	
	// Find entry
	char szNormalized[MAX_PATH];
	NormalizeFileName(szFileName, szNormalized);
	
	auto it = m_fileIndexMap.find(szNormalized);
	if (it == m_fileIndexMap.end()) {
		return false;
	}
	
	FileEntryCache& cache = m_fileEntries[it->second];
	
	// Delete entry
	PCKRTN ret = pck_DeleteEntry(cache.pWinPckEntry);
	if (ret != WINPCK_OK) {
		return false;
	}
	
	// Submit deletion
	ret = pck_DeleteEntrySubmit();
	if (ret == WINPCK_OK) {
		// Rebuild cache
		m_dwFileCount = pck_filecount();
		BuildFileEntryCache();
	}
	
	return (ret == WINPCK_OK);
}

bool AFilePackageWinPCK::ReplaceFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress)
{
	// Remove then add
	if (!RemoveFile(szFileName)) {
		return false;
	}
	
	return AppendFile(szFileName, pFileBuffer, dwFileLength, bCompress);
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
	auto it = m_fileIndexMap.find(szNormalized);
	if (it == m_fileIndexMap.end()) {
		return false;
	}
	
	// Copy entry
	*pFileEntry = m_fileEntries[it->second].entry;
	
	if (pnIndex) {
		*pnIndex = it->second;
	}
	
	return true;
}

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
	
	return (m_fileIndexMap.find(szNormalized) != m_fileIndexMap.end());
}

// Get package file size
DWORD AFilePackageWinPCK::GetPackageFileSize()
{
	if (!m_bOpened) {
		return 0;
	}
	
	// WinPCK returns 64-bit size, we truncate to 32-bit for compatibility
	uint64_t size = pck_filesize();
	return (DWORD)min(size, (uint64_t)0xFFFFFFFF);
}

// Generate unique file ID
DWORD AFilePackageWinPCK::GenerateFileID()
{
	return InterlockedIncrement((LONG*)&m_dwNextFileID);
}

// Open shared file
DWORD AFilePackageWinPCK::OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem)
{
	if (!m_bOpened || !szFileName || !ppFileBuf || !pdwFileLen) {
		return 0;
	}
	
	// Find file entry
	AFilePackage::FILEENTRY entry;
	int nIndex;
	if (!GetFileEntry(szFileName, &entry, &nIndex)) {
		return 0;
	}
	
	EnterCriticalSection(&m_csSharedFiles);
	
	// Check if already opened
	for (auto& pair : m_sharedFiles) {
		if (pair.second.pEntry == &m_fileEntries[nIndex]) {
			// Already opened, increase ref count
			pair.second.iRefCnt++;
			*ppFileBuf = pair.second.pFileData;
			*pdwFileLen = pair.second.dwFileLen;
			
			DWORD id = pair.first;
			LeaveCriticalSection(&m_csSharedFiles);
			return id;
		}
	}
	
	// Allocate buffer
	BYTE* pBuffer = bTempMem ? (BYTE*)a_malloctemp(entry.dwLength) : (BYTE*)a_malloc(entry.dwLength);
	if (!pBuffer) {
		LeaveCriticalSection(&m_csSharedFiles);
		return 0;
	}
	
	// Read file
	DWORD dwLen = entry.dwLength;
	if (!ReadFile(entry, pBuffer, &dwLen)) {
		if (bTempMem) {
			a_freetemp(pBuffer);
		} else {
			a_free(pBuffer);
		}
		LeaveCriticalSection(&m_csSharedFiles);
		return 0;
	}
	
	// Create shared file item
	SharedFileItem item;
	item.dwFileID = GenerateFileID();
	item.bTempMem = bTempMem;
	item.iRefCnt = 1;
	item.pFileData = pBuffer;
	item.dwFileLen = dwLen;
	item.pEntry = &m_fileEntries[nIndex];
	
	m_sharedFiles[item.dwFileID] = item;
	
	*ppFileBuf = pBuffer;
	*pdwFileLen = dwLen;
	
	DWORD id = item.dwFileID;
	LeaveCriticalSection(&m_csSharedFiles);
	
	return id;
}

// Close shared file
void AFilePackageWinPCK::CloseSharedFile(DWORD dwFileHandle)
{
	if (!dwFileHandle) {
		return;
	}
	
	EnterCriticalSection(&m_csSharedFiles);
	
	auto it = m_sharedFiles.find(dwFileHandle);
	if (it != m_sharedFiles.end()) {
		it->second.iRefCnt--;
		
		if (it->second.iRefCnt <= 0) {
			// Free memory
			if (it->second.bTempMem) {
				a_freetemp(it->second.pFileData);
			} else {
				a_free(it->second.pFileData);
			}
			
			m_sharedFiles.erase(it);
		}
	}
	
	LeaveCriticalSection(&m_csSharedFiles);
}

// Clear file cache
void AFilePackageWinPCK::ClearFileCache()
{
	// WinPCK manages its own cache
	// We can't directly control it
}

// Add cache file name
bool AFilePackageWinPCK::AddCacheFileName(const char* szFile)
{
	// WinPCK manages its own cache
	// This is a no-op for compatibility
	return true;
}

// Static compression functions
int AFilePackageWinPCK::Compress(LPBYTE pFileBuffer, DWORD dwFileLength, LPBYTE pCompressedBuffer, DWORD* pdwCompressedLength)
{
	// Use zlib directly since WinPCK doesn't expose compression
	uLongf destLen = *pdwCompressedLength;
	int ret = compress2(pCompressedBuffer, &destLen, pFileBuffer, dwFileLength, Z_BEST_SPEED);
	*pdwCompressedLength = destLen;
	
	if (ret == Z_OK) return 0;
	if (ret == Z_BUF_ERROR) return -1;
	return -2;
}

int AFilePackageWinPCK::Uncompress(LPBYTE pCompressedBuffer, DWORD dwCompressedLength, LPBYTE pFileBuffer, DWORD* pdwFileLength)
{
	// Use zlib directly
	uLongf destLen = *pdwFileLength;
	int ret = uncompress(pFileBuffer, &destLen, pCompressedBuffer, dwCompressedLength);
	*pdwFileLength = destLen;
	
	if (ret == Z_OK) return 0;
	if (ret == Z_BUF_ERROR) return -1;
	return -2;
}