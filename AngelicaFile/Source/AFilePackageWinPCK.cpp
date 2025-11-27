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
#include <fstream>
#include <sstream>
#include <vector>
#include <io.h>

// Note: pck_dependencies.h types are already included via the header file

#define new A_DEBUG_NEW

// PCK version constants
#define AFPCK_VERSION_202 0x00020002
#define AFPCK_VERSION_203 0x00020003

// Entry type constants  
#define ENTRY_TYPE_FILE   1
#define ENTRY_TYPE_FOLDER 2

// Static members
bool AFilePackageWinPCK::s_bWinPCKInitialized = false;
CRITICAL_SECTION AFilePackageWinPCK::s_csInit;

// Static callback functions - must match the typedef exactly
static void WinPCKLogCallback(const char log_level, const wchar_t* str)
{
	// Convert to narrow string for our logging
	char szLog[1024];
	WideCharToMultiByte(CP_ACP, 0, str, -1, szLog, 1024, NULL, NULL);
	
	// Log based on level
	switch(log_level) {
		case 'E':
			AFERRLOG(("WinPCK Error: %s", szLog));
			break;
		case 'W':
			AFERRLOG(("WinPCK Warning: %s", szLog));
			break;
		default:
			AFERRLOG(("WinPCK: %s", szLog));
			break;
	}
}

static int32_t WinPCKFeedbackCallback(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam)
{
	// Log important events
	AFERRLOG(("WinPCK Event: %d, wParam: %zu, lParam: %zd", eventId, wParam, lParam));
	return 0;
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
	
	// Check which DLL is loaded
	HMODULE hDll = GetModuleHandleA("pckdll_x86.dll");
	if (!hDll) {
		// Try x64
		hDll = GetModuleHandleA("pckdll_x64.dll");
		if (!hDll) {
			AFERRLOG(("WinPCK: ERROR - Neither pckdll_x86.dll nor pckdll_x64.dll loaded!"));
		} else {
			AFERRLOG(("WinPCK: pckdll_x64.dll loaded successfully"));
		}
	} else {
		AFERRLOG(("WinPCK: pckdll_x86.dll loaded successfully"));
	}
	
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
		AFERRLOG(("WinPCK: Initializing WinPCK DLL"));
		
		// CRITICAL: Register callbacks BEFORE any other operations
		// This is what WinPCK app does and might be required
		pck_regMsgFeedback(NULL, WinPCKFeedbackCallback);
		log_regShowFunc(WinPCKLogCallback);
		
		// Set memory limit for large files (512MB)
		pck_setMTMaxMemory(512 * 1024 * 1024);
		
		// Setup algorithms
		InitializeAlgorithms();
		
		// Check for pck.ini and load custom algorithms
		// LoadPckIni(); // Commented out - we're using MultiPart handler instead
		
		s_bWinPCKInitialized = true;
		AFERRLOG(("WinPCK: Initialization complete"));
	}
	LeaveCriticalSection(&s_csInit);
	
	s_bInitialized = true;
	return true;
}

// Initialize algorithm IDs
void AFilePackageWinPCK::InitializeAlgorithms()
{
	AFERRLOG(("WinPCK: Registering algorithms for PCK versions"));
	
	// Register ALL possible algorithms for both version 202 and 203
	// WinPCK tests algorithms sequentially until it finds the right one
	// We need to register every possible algorithm ID
	
	// Start with known specific algorithms
	int known_algorithms[] = {0, 1, 2, 111, 121, 131, 161, 171, 174};
	
	// Register known algorithms
	for (int i = 0; i < sizeof(known_algorithms)/sizeof(known_algorithms[0]); i++) {
		pck_addVersionAlgorithmId(known_algorithms[i], AFPCK_VERSION_202);
		pck_addVersionAlgorithmId(known_algorithms[i], AFPCK_VERSION_203);
	}
	
	// Also register a wider range of algorithm IDs that might be used
	// Some custom clients use algorithm IDs in different ranges
	for (int id = 3; id <= 255; id++) {
		// Skip already registered ones
		bool already_registered = false;
		for (int j = 0; j < sizeof(known_algorithms)/sizeof(known_algorithms[0]); j++) {
			if (id == known_algorithms[j]) {
				already_registered = true;
				break;
			}
		}
		if (!already_registered) {
			pck_addVersionAlgorithmId(id, AFPCK_VERSION_202);
			pck_addVersionAlgorithmId(id, AFPCK_VERSION_203);
		}
	}
	
	AFERRLOG(("WinPCK: Registered algorithms 0-255 for comprehensive support"));
}

// Load custom algorithms from pck.ini
void AFilePackageWinPCK::LoadPckIni()
{
	const char* iniFile = "pck.ini";
	
	// Check if file exists
	if (_access(iniFile, 0) != 0) {
		AFERRLOG(("WinPCK: pck.ini not found, using default algorithms only"));
		return;
	}
	
	AFERRLOG(("WinPCK: Loading custom algorithms from pck.ini"));
	
	std::ifstream file(iniFile);
	std::string line;
	bool inKeysSection = false;
	int customCount = 0;
	
	while (std::getline(file, line)) {
		// Trim whitespace
		size_t start = line.find_first_not_of(" \t");
		if (start == std::string::npos) continue;
		line = line.substr(start);
		
		// Skip comments
		if (line[0] == ';' || line[0] == '#') continue;
		
		// Check for section
		if (line[0] == '[') {
			inKeysSection = (line == "[Keys]");
			continue;
		}
		
		if (!inKeysSection) continue;
		
		// Parse key entry: AlgorithmId=Version:Name:KEY_1:KEY_2
		size_t eqPos = line.find('=');
		if (eqPos == std::string::npos) continue;
		
		int algorithmId = atoi(line.substr(0, eqPos).c_str());
		std::string value = line.substr(eqPos + 1);
		
		// Split value by ':'
		std::vector<std::string> parts;
		std::stringstream ss(value);
		std::string part;
		while (std::getline(ss, part, ':')) {
			parts.push_back(part);
		}
		
		if (parts.size() >= 4) {
			int version = atoi(parts[0].c_str());
			const char* name = parts[1].c_str();
			
			// Parse hex keys
			int key1 = 0, key2 = 0;
			sscanf(parts[2].c_str(), "%x", &key1);
			sscanf(parts[3].c_str(), "%x", &key2);
			
			// Convert to PCK version format
			int pckVersion = (version == 3) ? AFPCK_VERSION_203 : AFPCK_VERSION_202;
			
			// Convert name to wide char
			wchar_t wideName[256];
			MultiByteToWideChar(CP_ACP, 0, name, -1, wideName, 256);
			
			// Register algorithm with custom keys
			// NOTE: pck_addVersionAlgorithmIdByKeys not available in current pckdll_x86.lib
			// pck_addVersionAlgorithmIdByKeys(algorithmId, pckVersion, wideName, 0, 0, key1, key2);
			
			// Use the simpler registration for now
			pck_addVersionAlgorithmId(algorithmId, pckVersion);
			
			AFERRLOG(("WinPCK: Registered custom algorithm %d (%s) with keys 0x%08X, 0x%08X", 
				algorithmId, name, key1, key2));
			customCount++;
		}
	}
	
	file.close();
	AFERRLOG(("WinPCK: Loaded %d custom algorithms from pck.ini", customCount));
}

// Set algorithm ID
void AFilePackageWinPCK::SetAlgorithmId(int id, int customGuardByte0, int customGuardByte1, int customMaskDword, int customCheckMask)
{
	// For now, just use the simple version
	// The complex version with custom keys may not be exported properly from the DLL
	pck_setVersion(id);
}

// Open package file
bool AFilePackageWinPCK::Open(const char* szPckPath, AFilePackage::OPENMODE mode, bool bEncrypt)
{
	// Match AFilePackGame behavior - create folder from filename
	char szFolder[MAX_PATH];
	strncpy(szFolder, szPckPath, MAX_PATH);
	
	// Strip extension and add backslash
	char* pext = strrchr(szFolder, '.');
	if (pext) {
		// Check if extension is .pck (case insensitive)
		if ((pext[1] == 'p' || pext[1] == 'P') &&
		    (pext[2] == 'c' || pext[2] == 'C') &&
		    (pext[3] == 'k' || pext[3] == 'K') &&
		    pext[4] == '\0') {
			*pext = '\\';
			*(pext + 1) = '\0';
		} else {
			// If not .pck extension, just use empty folder
			szFolder[0] = '\0';
		}
	} else {
		// No extension found
		szFolder[0] = '\0';
	}
	
	return Open(szPckPath, szFolder, mode, bEncrypt);
}

bool AFilePackageWinPCK::Open(const char* szPckPath, const char* szFolder, AFilePackage::OPENMODE mode, bool bEncrypt)
{
	// Close any existing file
	Close();
	
	// Convert path to wide char
	wchar_t wszPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, szPckPath, -1, wszPath, MAX_PATH);
	
	// Log what we're trying to open
	AFERRLOG(("WinPCK: Attempting to open '%s' with folder '%s'", szPckPath, szFolder));
	
	// Check if all parts exist for multi-part archives
	CheckMultiPartFiles(szPckPath);
	
	// Don't log version before opening - let WinPCK auto-detect
	
	// Open the PCK file
	PCKRTN result = pck_open(wszPath);
	bool bSuccess = (result == WINPCK_OK);
	if (!bSuccess) {
		// Log error for debugging
		const char* errorMsg = "Unknown error";
		switch(result) {
			case WINPCK_ERROR: errorMsg = "General error"; break;
			case WINPCK_INVALIDPCK: errorMsg = "Invalid PCK file or file not open"; break;
			case WINPCK_NOTFOUND: errorMsg = "File not found"; break;
			default: break;
		}
		AFERRLOG(("WinPCK: Failed to open PCK file '%s' with x86 DLL, error code: %d (%s)", szPckPath, result, errorMsg));
		
		// For multi-part files, provide more detailed error info
		if (strstr(szPckPath, "litmodels.pck") != NULL || 
		    strstr(szPckPath, "gfx.pck") != NULL ||
		    strstr(szPckPath, "sfx.pck") != NULL) {
			AFERRLOG(("WinPCK: Multi-part PCK failed. Make sure all .pkx/.pkx1 files are present!"));
			CheckMultiPartFiles(szPckPath);
		}
		
		return false;
	}
	
	// Log success with detected version
	int detectedVer = pck_getVersion();
	AFERRLOG(("WinPCK: Successfully opened PCK file '%s' with algorithm version: %d", szPckPath, detectedVer));
	
	// Store file info
	strncpy(m_szPckFileName, szPckPath, MAX_PATH);
	strncpy(m_szFolder, szFolder, MAX_PATH);
	m_bReadOnly = (mode == AFilePackage::OPENEXIST);
	m_bOpened = true;
	
	// Get file count
	m_dwFileCount = pck_filecount();
	AFERRLOG(("WinPCK: File count = %d", m_dwFileCount));
	
	// Build file entry cache
	if (!BuildFileEntryCache()) {
		AFERRLOG(("WinPCK: Failed to build file entry cache"));
		pck_close();
		return false;
	}
	
	AFERRLOG(("WinPCK: Successfully cached %d files", m_fileEntries.size()));
	
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

// Callback data structure for file listing
struct FileListCallbackData {
	AFilePackageWinPCK* pThis;
	std::string currentPath;
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
		sprintf(fullPath, "%s%s", pData->currentPath.c_str(), szFileName);
		
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
	else if (entryType == ENTRY_TYPE_FOLDER && wcscmp(lpszFilename, L"..") != 0) {
		// For directories, we need to process them recursively
		// Build the new path
		char szDirName[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, lpszFilename, -1, szDirName, MAX_PATH, NULL, NULL);
		
		std::string subPath = pData->currentPath + szDirName + "\\";
		
		// Create new callback data for subdirectory
		FileListCallbackData subData;
		subData.pThis = pData->pThis;
		subData.currentPath = subPath;
		
		// List files in subdirectory
		pck_listByNode((LPCENTRY)pNode, &subData, FileListCallback);
	}
}

// Build file entry cache
bool AFilePackageWinPCK::BuildFileEntryCache()
{
	m_fileEntries.clear();
	m_fileIndexMap.clear();
	
	if (m_dwFileCount == 0) {
		AFERRLOG(("WinPCK: No files in PCK"));
		return true;
	}
	
	m_fileEntries.reserve(m_dwFileCount);
	
	// Get root node
	LPCENTRY pRoot = pck_getRootNode();
	if (!pRoot) {
		AFERRLOG(("WinPCK: Failed to get root node"));
		return false;
	}
	
	AFERRLOG(("WinPCK: Processing file entries..."));
	
	// Process all files from root
	ProcessNode(pRoot, "");
	
	AFERRLOG(("WinPCK: Processed %d entries", m_fileEntries.size()));
	
	return true;
}

// Process a node recursively
void AFilePackageWinPCK::ProcessNode(LPCENTRY pNode, const char* path)
{
	FileListCallbackData cbData;
	cbData.pThis = this;
	cbData.currentPath = path;
	
	// List all entries in this node
	pck_listByNode(pNode, &cbData, FileListCallback);
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
	
	// Read file data using WinPCK API
	PCKRTN result = pck_GetSingleFileData(cache.pWinPckEntry, (LPSTR)pFileBuffer, fileEntry.dwLength);
	if (result != WINPCK_OK) {
		return false;
	}
	
	*pdwBufferLen = fileEntry.dwLength;
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
	
	// For now, just read the uncompressed file
	// The original AFilePackage doesn't expose compression directly either
	return ReadFile(fileEntry, pCompressedBuffer, pdwBufferLen);
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
	
	return (DWORD)pck_filesize();
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

// Check for multi-part files
void AFilePackageWinPCK::CheckMultiPartFiles(const char* szPckPath)
{
	char szBaseName[MAX_PATH];
	strncpy(szBaseName, szPckPath, MAX_PATH);
	
	// Remove extension
	char* pExt = strrchr(szBaseName, '.');
	if (pExt) *pExt = '\0';
	
	// Check main file
	if (_access(szPckPath, 0) == 0) {
		WIN32_FILE_ATTRIBUTE_DATA fileInfo;
		if (GetFileAttributesExA(szPckPath, GetFileExInfoStandard, &fileInfo)) {
			LARGE_INTEGER size;
			size.HighPart = fileInfo.nFileSizeHigh;
			size.LowPart = fileInfo.nFileSizeLow;
			AFERRLOG(("WinPCK: Found %s - Size: %lld bytes", szPckPath, size.QuadPart));
		}
	} else {
		AFERRLOG(("WinPCK: Main file %s NOT FOUND!", szPckPath));
	}
	
	// Check for .pkx files
	char szPkxPath[MAX_PATH];
	int pkxCount = 0;
	
	// Check .pkx
	sprintf(szPkxPath, "%s.pkx", szBaseName);
	if (_access(szPkxPath, 0) == 0) {
		WIN32_FILE_ATTRIBUTE_DATA fileInfo;
		if (GetFileAttributesExA(szPkxPath, GetFileExInfoStandard, &fileInfo)) {
			LARGE_INTEGER size;
			size.HighPart = fileInfo.nFileSizeHigh;
			size.LowPart = fileInfo.nFileSizeLow;
			AFERRLOG(("WinPCK: Found %s - Size: %lld bytes", szPkxPath, size.QuadPart));
			pkxCount++;
		}
	}
	
	// Check .pkx1, .pkx2, etc.
	for (int i = 1; i < 10; i++) {
		sprintf(szPkxPath, "%s.pkx%d", szBaseName, i);
		if (_access(szPkxPath, 0) == 0) {
			WIN32_FILE_ATTRIBUTE_DATA fileInfo;
			if (GetFileAttributesExA(szPkxPath, GetFileExInfoStandard, &fileInfo)) {
				LARGE_INTEGER size;
				size.HighPart = fileInfo.nFileSizeHigh;
				size.LowPart = fileInfo.nFileSizeLow;
				AFERRLOG(("WinPCK: Found %s - Size: %lld bytes", szPkxPath, size.QuadPart));
				pkxCount++;
			}
		} else {
			if (i == 1 && pkxCount > 0) {
				AFERRLOG(("WinPCK: WARNING! %s NOT FOUND - This file may contain critical index data!", szPkxPath));
			}
			break;
		}
	}
	
	if (pkxCount > 0) {
		AFERRLOG(("WinPCK: Total multi-part files found: %d", pkxCount + 1));
	}
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

// Static compression functions - use zlib directly
extern "C" {
	int compress2(unsigned char *dest, unsigned long *destLen,
		const unsigned char *source, unsigned long sourceLen, int level);
	int uncompress(unsigned char *dest, unsigned long *destLen,
		const unsigned char *source, unsigned long sourceLen);
}

int AFilePackageWinPCK::Compress(LPBYTE pFileBuffer, DWORD dwFileLength, LPBYTE pCompressedBuffer, DWORD* pdwCompressedLength)
{
	return compress2(pCompressedBuffer, (unsigned long*)pdwCompressedLength, pFileBuffer, dwFileLength, 9);
}

int AFilePackageWinPCK::Uncompress(LPBYTE pCompressedBuffer, DWORD dwCompressedLength, LPBYTE pFileBuffer, DWORD* pdwFileLength)
{
	return uncompress(pFileBuffer, (unsigned long*)pdwFileLength, pCompressedBuffer, dwCompressedLength);
}