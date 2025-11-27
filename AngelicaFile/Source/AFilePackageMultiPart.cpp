/*
 * FILE: AFilePackageMultiPart.cpp
 *
 * DESCRIPTION: Multi-part PCK file handler implementation
 *
 * CREATED BY: Direct implementation based on Python extractor
 *
 * HISTORY:
 *
 * Copyright (c) 2024, All Rights Reserved.
 */

#include "AFPI.h"
#include "AFI.h"
#include "AFilePackageMultiPart.h"
#include "AMemory.h"
#include "AAssist.h"
#include "ADebugLog.h"
#include <algorithm>
#include <io.h>

#define new A_DEBUG_NEW

// Note: We use hardcoded keys from Python script for litmodels.pck
// Regular PCKs use different keys via AFPCK_MASKDWORD/AFPCK_CHECKMASK

// zlib functions
extern "C" {
	int compress2(unsigned char *dest, unsigned long *destLen,
		const unsigned char *source, unsigned long sourceLen, int level);
	int uncompress(unsigned char *dest, unsigned long *destLen,
		const unsigned char *source, unsigned long sourceLen);
}

AFilePackageMultiPart::AFilePackageMultiPart()
{
	m_bOpened = false;
	m_bReadOnly = false;
	m_dwFileCount = 0;
	m_dwVersion = 0;
	m_qwIndexTableOffset = 0;
	m_dwNextFileID = 1;
	m_szPckFileName[0] = '\0';
	m_szFolder[0] = '\0';
	
	InitializeCriticalSection(&m_csSharedFiles);
}

AFilePackageMultiPart::~AFilePackageMultiPart()
{
	Close();
	DeleteCriticalSection(&m_csSharedFiles);
}

bool AFilePackageMultiPart::Open(const char* szPckPath, AFilePackage::OPENMODE mode, bool bEncrypt)
{
	// Match AFilePackGame behavior - create folder from filename
	char szFolder[MAX_PATH];
	strncpy(szFolder, szPckPath, MAX_PATH);
	
	// Strip extension and add backslash
	char* pext = strrchr(szFolder, '.');
	if (pext) {
		if ((pext[1] == 'p' || pext[1] == 'P') &&
		    (pext[2] == 'c' || pext[2] == 'C') &&
		    (pext[3] == 'k' || pext[3] == 'K') &&
		    pext[4] == '\0') {
			*pext = '\\';
			*(pext + 1) = '\0';
		} else {
			szFolder[0] = '\0';
		}
	} else {
		szFolder[0] = '\0';
	}
	
	return Open(szPckPath, szFolder, mode, bEncrypt);
}

bool AFilePackageMultiPart::Open(const char* szPckPath, const char* szFolder, AFilePackage::OPENMODE mode, bool bEncrypt)
{
	ALOG_INFO("AFilePackageMultiPart::Open: pck=[%s] folder=[%s] mode=%d encrypt=%d",
		szPckPath, szFolder ? szFolder : "(null)", (int)mode, bEncrypt);
	Close();

	// Don't log at error level - this is normal operation
	// AFERRLOG(("MultiPart: Opening '%s' with folder '%s'", szPckPath, szFolder));
	
	// Store info
	strncpy(m_szPckFileName, szPckPath, MAX_PATH);
	strncpy(m_szFolder, szFolder, MAX_PATH);
	m_bReadOnly = (mode == AFilePackage::OPENEXIST);
	
	// Open all parts
	if (!OpenMultiPartFiles(szPckPath)) {
		AFERRLOG(("MultiPart: Failed to open multi-part files"));
		return false;
	}
	
	// Detect version
	if (!DetectVersion()) {
		AFERRLOG(("MultiPart: Failed to detect version"));
		Close();
		return false;
	}
	
	// Read file index
	if (!ReadFileIndexTable()) {
		AFERRLOG(("MultiPart: Failed to read file index"));
		Close();
		return false;
	}
	
	m_bOpened = true;
	AFERRLOG(("MultiPart: Successfully opened with %d files", m_dwFileCount));
	
	return true;
}

bool AFilePackageMultiPart::Close()
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
	
	// Close file handles
	for (size_t i = 0; i < m_fileParts.size(); i++) {
		if (m_fileParts[i].hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(m_fileParts[i].hFile);
		}
	}
	m_fileParts.clear();
	
	// Clear cache
	m_fileEntries.clear();
	m_fileIndexMap.clear();
	
	m_bOpened = false;
	m_dwFileCount = 0;
	
	return true;
}

bool AFilePackageMultiPart::OpenMultiPartFiles(const char* szPckPath)
{
	// Extract base name
	char szBaseName[MAX_PATH];
	strncpy(szBaseName, szPckPath, MAX_PATH);
	char* pExt = strrchr(szBaseName, '.');
	if (pExt) *pExt = '\0';
	
	ULONGLONG qwTotalOffset = 0;
	
	// Open main PCK file
	HANDLE hFile = CreateFileA(szPckPath, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		AFERRLOG(("MultiPart: Failed to open main file: %s", szPckPath));
		return false;
	}
	
	LARGE_INTEGER fileSize;
	GetFileSizeEx(hFile, &fileSize);
	
	FilePartInfo part;
	part.hFile = hFile;
	part.qwSize = fileSize.QuadPart;
	part.qwOffset = qwTotalOffset;
	m_fileParts.push_back(part);
	
	qwTotalOffset += fileSize.QuadPart;
	// Log at info level, not error
	// AFERRLOG(("MultiPart: Opened %s - Size: %lld bytes", szPckPath, fileSize.QuadPart));
	
	// Open PKX files
	int pkxNum = 0;
	while (true) {
		char szPkxPath[MAX_PATH];
		if (pkxNum == 0) {
			sprintf(szPkxPath, "%s.pkx", szBaseName);
		} else {
			sprintf(szPkxPath, "%s.pkx%d", szBaseName, pkxNum);
		}
		
		hFile = CreateFileA(szPkxPath, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			if (pkxNum == 0) {
				// No .pkx file, single part archive - this is normal!
				// Don't log as error
			}
			break;
		}
		
		GetFileSizeEx(hFile, &fileSize);
		
		part.hFile = hFile;
		part.qwSize = fileSize.QuadPart;
		part.qwOffset = qwTotalOffset;
		m_fileParts.push_back(part);
		
		qwTotalOffset += fileSize.QuadPart;
		AFERRLOG(("MultiPart: Opened %s - Size: %lld bytes", szPkxPath, fileSize.QuadPart));
		
		pkxNum++;
	}
	
	AFERRLOG(("MultiPart: Total parts: %d, Total size: %lld bytes", 
		m_fileParts.size(), qwTotalOffset));
	
	return true;
}

bool AFilePackageMultiPart::DetectVersion()
{
	// Try v2.0.3 first (288 bytes), then v2.0.2 (280 bytes)
	PCKTAIL_V203 tail203;
	if (ReadTailData(&tail203, sizeof(tail203))) {
		// Check version markers
		if (tail203.dwVersion == AFPCK_VERSION_203) {
			m_dwVersion = AFPCK_VERSION_203;
			return ParseV203Tail(&tail203);
		}
	}
	
	// Try v2.0.2
	PCKTAIL_V202 tail202;
	if (ReadTailData(&tail202, sizeof(tail202))) {
		if (tail202.dwVersion == AFPCK_VERSION_202) {
			m_dwVersion = AFPCK_VERSION_202;
			return ParseV202Tail(&tail202);
		}
	}
	
	AFERRLOG(("MultiPart: Failed to detect PCK version"));
	return false;
}

bool AFilePackageMultiPart::ReadTailData(void* pBuffer, size_t size)
{
	if (m_fileParts.empty()) return false;
	
	// Read from the last file
	FilePartInfo& lastPart = m_fileParts.back();
	
	// Seek to tail position
	LARGE_INTEGER pos;
	pos.QuadPart = lastPart.qwSize - size;
	if (!SetFilePointerEx(lastPart.hFile, pos, NULL, FILE_BEGIN)) {
		return false;
	}
	
	DWORD bytesRead;
	return ::ReadFile(lastPart.hFile, pBuffer, (DWORD)size, &bytesRead, NULL) && 
		bytesRead == size;
}

bool AFilePackageMultiPart::ParseV202Tail(const PCKTAIL_V202* pTail)
{
	m_dwFileCount = pTail->dwFileCount;
	
	// Decrypt index table offset
	m_qwIndexTableOffset = pTail->dwEntryOffset ^ PCK_INDEX_ENTRY_ADDR_KEY;
	
	AFERRLOG(("MultiPart: Version 2.0.2 detected"));
	AFERRLOG(("MultiPart: File count: %u", m_dwFileCount));
	AFERRLOG(("MultiPart: Index table offset: 0x%llX", m_qwIndexTableOffset));
	
	return true;
}

bool AFilePackageMultiPart::ParseV203Tail(const PCKTAIL_V203* pTail)
{
	m_dwFileCount = pTail->dwFileCount;
	
	// Decrypt index table offset (64-bit)
	ULONGLONG encryptedOffset = pTail->qwEntryOffset;
	
	// For 64-bit values, may need to sign-extend the XOR key
	if (encryptedOffset > 0x100000000ULL) {
		// Sign extend 32-bit key to 64-bit
		ULONGLONG xorKey = 0xFFFFFFFFA8937462ULL;
		m_qwIndexTableOffset = encryptedOffset ^ xorKey;
	} else {
		m_qwIndexTableOffset = encryptedOffset ^ PCK_INDEX_ENTRY_ADDR_KEY;
	}
	
	AFERRLOG(("MultiPart: Version 2.0.3 detected"));
	AFERRLOG(("MultiPart: File count: %u", m_dwFileCount));
	AFERRLOG(("MultiPart: Index table offset: 0x%llX", m_qwIndexTableOffset));
	
	return true;
}

bool AFilePackageMultiPart::ReadFileIndexTable()
{
	if (m_dwFileCount == 0) {
		AFERRLOG(("MultiPart: No files in PCK"));
		return true;
	}
	
	m_fileEntries.clear();
	m_fileIndexMap.clear();
	m_fileEntries.reserve(m_dwFileCount);
	
	// Calculate index data size
	ULONGLONG totalSize = GetTotalSize();
	ULONGLONG indexDataSize = totalSize - m_qwIndexTableOffset;
	
	AFERRLOG(("MultiPart: Reading %llu bytes of index data...", indexDataSize));
	
	// Allocate buffer for index data
	BYTE* pIndexData = (BYTE*)a_malloc((size_t)indexDataSize);
	if (!pIndexData) {
		AFERRLOG(("MultiPart: Failed to allocate index buffer"));
		return false;
	}
	
	// Read index data
	if (!ReadAtOffset(m_qwIndexTableOffset, pIndexData, (size_t)indexDataSize)) {
		a_free(pIndexData);
		return false;
	}
	
	// Process each file entry
	size_t offset = 0;
	DWORD processedFiles = 0;
	
	while (processedFiles < m_dwFileCount && offset + 8 <= indexDataSize) {
		// Read encrypted length headers
		DWORD encLen1 = *(DWORD*)(pIndexData + offset);
		DWORD encLen2 = *(DWORD*)(pIndexData + offset + 4);
		
		// Decrypt lengths
		DWORD len1 = encLen1 ^ PCK_INDEX_COMPRESS_KEY1;
		DWORD len2 = encLen2 ^ PCK_INDEX_COMPRESS_KEY2;
		
		if (len1 != len2) {
			AFERRLOG(("MultiPart: Length mismatch at entry %u", processedFiles));
			offset += 8;
			continue;
		}
		
		offset += 8;
		
		// Determine entry size based on version
		size_t expectedSize = (m_dwVersion == AFPCK_VERSION_202) ? 
			sizeof(PCKFILEINDEX_V202) : sizeof(PCKFILEINDEX_V203);
		
		BYTE* pEntryData = NULL;
		size_t entryDataSize = 0;
		
		if (len1 == expectedSize) {
			// Uncompressed entry
			if (offset + expectedSize > indexDataSize) break;
			pEntryData = pIndexData + offset;
			entryDataSize = expectedSize;
			offset += expectedSize;
		} else {
			// Compressed entry
			if (offset + len1 > indexDataSize) break;
			
			// Decompress
			BYTE* pDecompressed = (BYTE*)a_malloc(expectedSize);
			if (!pDecompressed) break;
			
			unsigned long destLen = (unsigned long)expectedSize;
			if (uncompress(pDecompressed, &destLen, pIndexData + offset, len1) == Z_OK) {
				pEntryData = pDecompressed;
				entryDataSize = destLen;
			} else {
				a_free(pDecompressed);
			}
			offset += len1;
		}
		
		if (pEntryData && entryDataSize == expectedSize) {
			// Parse entry
			FileEntryCache cache;
			
			if (m_dwVersion == AFPCK_VERSION_202) {
				PCKFILEINDEX_V202* pIndex = (PCKFILEINDEX_V202*)pEntryData;
				strncpy(cache.entry.szFileName, pIndex->szFileName, MAX_PATH);
				cache.entry.dwOffset = pIndex->dwOffset;
				cache.entry.dwLength = pIndex->dwFileClearSize;
				cache.entry.dwCompressedLength = pIndex->dwFileCipherSize;
			} else {
				PCKFILEINDEX_V203* pIndex = (PCKFILEINDEX_V203*)pEntryData;
				strncpy(cache.entry.szFileName, pIndex->szFileName, MAX_PATH);
				cache.entry.dwOffset = (DWORD)pIndex->qwOffset;
				cache.entry.dwLength = pIndex->dwFileClearSize;
				cache.entry.dwCompressedLength = pIndex->dwFileCipherSize;
			}
			
			// Check if compressed
			cache.bCompressed = (cache.entry.dwCompressedLength < cache.entry.dwLength);
			
			// Normalize filename
			char szNormalized[MAX_PATH];
			NormalizeFileName(cache.entry.szFileName, szNormalized);
			
			// Add to cache
			int index = m_fileEntries.size();
			m_fileEntries.push_back(cache);
			m_fileIndexMap[szNormalized] = index;
			
			processedFiles++;
			
			// Free decompressed data if allocated
			if (pEntryData != pIndexData + offset - entryDataSize) {
				a_free(pEntryData);
			}
		}
	}
	
	a_free(pIndexData);
	
	AFERRLOG(("MultiPart: Successfully read %u file entries", processedFiles));
	return processedFiles > 0;
}

bool AFilePackageMultiPart::ReadAtOffset(ULONGLONG qwOffset, void* pBuffer, size_t size)
{
	BYTE* pDest = (BYTE*)pBuffer;
	size_t remaining = size;
	
	for (size_t i = 0; i < m_fileParts.size(); i++) {
		FilePartInfo& part = m_fileParts[i];
		
		if (qwOffset < part.qwOffset + part.qwSize) {
			// This file contains data we need
			ULONGLONG localOffset = qwOffset - part.qwOffset;
			ULONGLONG availableInFile = part.qwSize - localOffset;
			
			size_t toRead = (size_t)min((ULONGLONG)remaining, availableInFile);
			
			// Seek and read
			LARGE_INTEGER pos;
			pos.QuadPart = localOffset;
			if (!SetFilePointerEx(part.hFile, pos, NULL, FILE_BEGIN)) {
				return false;
			}
			
			DWORD bytesRead;
			if (!::ReadFile(part.hFile, pDest, (DWORD)toRead, &bytesRead, NULL) ||
				bytesRead != toRead) {
				return false;
			}
			
			pDest += toRead;
			remaining -= toRead;
			qwOffset += toRead;
			
			if (remaining == 0) {
				return true;
			}
		}
	}
	
	return remaining == 0;
}

ULONGLONG AFilePackageMultiPart::GetTotalSize()
{
	ULONGLONG total = 0;
	for (size_t i = 0; i < m_fileParts.size(); i++) {
		total += m_fileParts[i].qwSize;
	}
	return total;
}

void AFilePackageMultiPart::NormalizeFileName(const char* szIn, char* szOut)
{
	// Convert to lowercase and normalize slashes
	// MBCS-safe: preserve GBK multi-byte characters (lead bytes 0x81-0xFE)
	const unsigned char* pIn = (const unsigned char*)szIn;
	unsigned char* pOut = (unsigned char*)szOut;
	int i = 0;

	while (pIn[i] && i < MAX_PATH-1) {
		// Check if this is a GBK lead byte (0x81-0xFE)
		if (pIn[i] >= 0x81 && pIn[i] <= 0xFE && pIn[i+1] && i < MAX_PATH-2) {
			// Copy both bytes of GBK character unchanged
			pOut[i] = pIn[i];
			pOut[i+1] = pIn[i+1];
			i += 2;
		} else if (pIn[i] == '/') {
			pOut[i] = '\\';
			i++;
		} else {
			// ASCII character - safe to lowercase
			if (pIn[i] >= 'A' && pIn[i] <= 'Z') {
				pOut[i] = pIn[i] + ('a' - 'A');
			} else {
				pOut[i] = pIn[i];
			}
			i++;
		}
	}
	pOut[i] = '\0';

	// Remove leading ".\"
	if (szOut[0] == '.' && szOut[1] == '\\') {
		memmove(szOut, szOut + 2, strlen(szOut) - 1);
	}
}

bool AFilePackageMultiPart::ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen)
{
	AFilePackage::FILEENTRY fileEntry;
	if (!GetFileEntry(szFileName, &fileEntry)) {
		return false;
	}
	
	return ReadFile(fileEntry, pFileBuffer, pdwBufferLen);
}

bool AFilePackageMultiPart::ReadFile(AFilePackage::FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD* pdwBufferLen)
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
	
	// Read compressed data
	if (cache.entry.dwCompressedLength > 0) {
		BYTE* pCompressed = (BYTE*)a_malloc(cache.entry.dwCompressedLength);
		if (!pCompressed) {
			return false;
		}
		
		if (!ReadAtOffset(cache.entry.dwOffset, pCompressed, cache.entry.dwCompressedLength)) {
			a_free(pCompressed);
			return false;
		}
		
		// Check if compressed (zlib header)
		if (cache.bCompressed && pCompressed[0] == 0x78 && 
			(pCompressed[1] == 0x01 || pCompressed[1] == 0x5E || 
			 pCompressed[1] == 0x9C || pCompressed[1] == 0xDA)) {
			// Decompress
			unsigned long destLen = *pdwBufferLen;
			int result = uncompress(pFileBuffer, &destLen, pCompressed, cache.entry.dwCompressedLength);
			a_free(pCompressed);
			
			if (result != Z_OK) {
				return false;
			}
			
			*pdwBufferLen = destLen;
		} else {
			// Not compressed, copy directly
			memcpy(pFileBuffer, pCompressed, cache.entry.dwCompressedLength);
			*pdwBufferLen = cache.entry.dwCompressedLength;
			a_free(pCompressed);
		}
	}
	
	return true;
}

bool AFilePackageMultiPart::ReadCompressedFile(const char* szFileName, LPBYTE pCompressedBuffer, DWORD* pdwBufferLen)
{
	// Just read the raw compressed data
	AFilePackage::FILEENTRY fileEntry;
	if (!GetFileEntry(szFileName, &fileEntry)) {
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
	
	if (*pdwBufferLen < cache.entry.dwCompressedLength) {
		*pdwBufferLen = cache.entry.dwCompressedLength;
		return false;
	}
	
	if (!ReadAtOffset(cache.entry.dwOffset, pCompressedBuffer, cache.entry.dwCompressedLength)) {
		return false;
	}
	
	*pdwBufferLen = cache.entry.dwCompressedLength;
	return true;
}

bool AFilePackageMultiPart::GetFileEntry(const char* szFileName, AFilePackage::FILEENTRY* pFileEntry, int* pnIndex)
{
	if (!m_bOpened || !szFileName || !pFileEntry) {
		return false;
	}
	
	char szNormalized[MAX_PATH];
	NormalizeFileName(szFileName, szNormalized);
	
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

const AFilePackage::FILEENTRY* AFilePackageMultiPart::GetFileEntryByIndex(int nIndex)
{
	if (!m_bOpened || nIndex < 0 || nIndex >= (int)m_fileEntries.size()) {
		return NULL;
	}
	
	return &m_fileEntries[nIndex].entry;
}

bool AFilePackageMultiPart::IsFileExist(const char* szFileName)
{
	if (!m_bOpened || !szFileName) {
		return false;
	}
	
	char szNormalized[MAX_PATH];
	NormalizeFileName(szFileName, szNormalized);
	
	return m_fileIndexMap.find(szNormalized) != m_fileIndexMap.end();
}

DWORD AFilePackageMultiPart::GetPackageFileSize()
{
	// Return total size (capped at DWORD max)
	ULONGLONG total = GetTotalSize();
	return (DWORD)min(total, (ULONGLONG)0xFFFFFFFF);
}

DWORD AFilePackageMultiPart::OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem)
{
	if (!m_bOpened || !szFileName) {
		return 0;
	}
	
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

void AFilePackageMultiPart::CloseSharedFile(DWORD dwFileHandle)
{
	if (!dwFileHandle) {
		return;
	}
	
	EnterCriticalSection(&m_csSharedFiles);
	
	std::map<DWORD, SharedFileItem>::iterator it = m_sharedFiles.find(dwFileHandle);
	if (it != m_sharedFiles.end()) {
		it->second.iRefCnt--;
		if (it->second.iRefCnt <= 0) {
			if (it->second.pFileData) {
				a_free(it->second.pFileData);
			}
			m_sharedFiles.erase(it);
		}
	}
	
	LeaveCriticalSection(&m_csSharedFiles);
}

DWORD AFilePackageMultiPart::GenerateFileID()
{
	return m_dwNextFileID++;
}

int AFilePackageMultiPart::Compress(LPBYTE pFileBuffer, DWORD dwFileLength, LPBYTE pCompressedBuffer, DWORD* pdwCompressedLength)
{
	return compress2(pCompressedBuffer, (unsigned long*)pdwCompressedLength, pFileBuffer, dwFileLength, 9);
}

int AFilePackageMultiPart::Uncompress(LPBYTE pCompressedBuffer, DWORD dwCompressedLength, LPBYTE pFileBuffer, DWORD* pdwFileLength)
{
	return uncompress(pFileBuffer, (unsigned long*)pdwFileLength, pCompressedBuffer, dwCompressedLength);
}