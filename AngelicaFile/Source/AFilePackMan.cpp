 /*
 * FILE: AFilePackage.cpp
 *
 * DESCRIPTION: A File Package Class for Angelica Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AFilePackMan.h"
#include "AFilePackage.h"
// #include "AFilePackageWinPCK.h"  // Removed - using default AFilePackage instead
#include "AFilePackageMultiPart.h"
#include <io.h>
#include "AFilePackGame.h"
#include "AFI.h"
#include "AFPI.h"
#include "ADebugLog.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

int	AFPCK_GUARDBYTE0 = 0xfdfdfeee;
int	AFPCK_GUARDBYTE1 = 0xf00dbeef;
int AFPCK_MASKDWORD = 0xa8937462;
int	AFPCK_CHECKMASK	 = 0x59374231;

int 		AFPCK_MASKDWORDCUSTOM  = 0x350DAA8C;
int		AFPCK_CHECKMASKCUSTOM	 = 0x3E367B13;
int		AFPCK_GUARDBYTE0CUSTOM = 0x1207940;
int		AFPCK_GUARDBYTE1CUSTOM = 0x549D22B4;

int		AFPCK_SAFETAG1CC =	0x3F071506;
int		AFPCK_SAFETAG2CC =	0x2A1E5540;

AFilePackMan g_AFilePackMan;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	AFilePackMan
//
///////////////////////////////////////////////////////////////////////////

AFilePackMan::AFilePackMan() : m_FilePcks(), m_bUseWinPCK(false)
{
	ALOG_INFO("AFilePackMan constructor called");
}

AFilePackMan::~AFilePackMan()
{
	ALOG_INFO("AFilePackMan destructor called");
	CloseAllPackages();
}

bool AFilePackMan::CreateFilePackage(const char * szPckFile, const char* szFolder, bool bEncrypt/* false */)
{
	AFilePackage * pFilePackage;
	if (!(pFilePackage = new AFilePackage))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, szFolder, AFilePackage::CREATENEW, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
		return false;
	}

	m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackage(const char * szPckFile, bool bCreateNew, bool bEncrypt)
{
	AFilePackBase * pFilePackage;
	
	if (!bCreateNew) {
		// Use MultiPart handler for existing files
		if (!(pFilePackage = new AFilePackageMultiPart))
		{
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
			return false;
		}
		
		if (!((AFilePackageMultiPart*)pFilePackage)->Open(szPckFile, AFilePackage::OPENEXIST, bEncrypt))
		{
			delete pFilePackage;
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
			return false;
		}
	}
	else {
		// Use original implementation
		if (!(pFilePackage = new AFilePackage))
		{
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
			return false;
		}

		if (!((AFilePackage*)pFilePackage)->Open(szPckFile, bCreateNew ? AFilePackage::CREATENEW : AFilePackage::OPENEXIST, bEncrypt))
		{
			delete pFilePackage;
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
			return false;
		}
	}

	m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackage(const char * szPckFile, const char* szFolder, bool bEncrypt/* false */)
{
	AFilePackBase * pFilePackage;
	
	// Check if this is a known multi-part PCK file
	if (strstr(szPckFile, "litmodels.pck") != NULL || strstr(szPckFile, "building.pck") != NULL) {
		AFERRLOG(("AFilePackMan: Using MultiPart handler for multi-part PCK: %s", szPckFile));
		if (!(pFilePackage = new AFilePackageMultiPart))
		{
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
			return false;
		}
		
		if (!((AFilePackageMultiPart*)pFilePackage)->Open(szPckFile, szFolder, AFilePackage::OPENEXIST, bEncrypt))
		{
			delete pFilePackage;
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
			return false;
		}
	}
	else {
		// Use original implementation for all other PCKs
		if (!(pFilePackage = new AFilePackage))
		{
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
			return false;
		}
		
		if (!((AFilePackage*)pFilePackage)->Open(szPckFile, szFolder, AFilePackage::OPENEXIST, bEncrypt))
		{
			delete pFilePackage;
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
			return false;
		}
	}

	m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackageInGame(const char* szPckFile, bool bEncrypt/* false */)
{
	ALOG_INFO("OpenFilePackageInGame(1): pck=[%s] encrypt=%d", szPckFile, bEncrypt);
	AFilePackBase* pFilePackage;

	// Check if this is a known multi-part PCK file
	if (strstr(szPckFile, "litmodels.pck") != NULL || strstr(szPckFile, "building.pck") != NULL) {
		// Use MultiPart handler with Python script logic for multi-part PCKs
		ALOG_INFO("OpenFilePackageInGame: Using MultiPart handler for: %s", szPckFile);
		AFERRLOG(("AFilePackMan: Using MultiPart handler for multi-part PCK: %s", szPckFile));

		if (!(pFilePackage = new AFilePackageMultiPart))
		{
			ALOG_ERROR("OpenFilePackageInGame: Failed to allocate AFilePackageMultiPart");
			return false;
		}

		if (!((AFilePackageMultiPart*)pFilePackage)->Open(szPckFile, AFilePackage::OPENEXIST, bEncrypt))
		{
			ALOG_ERROR("OpenFilePackageInGame: AFilePackageMultiPart::Open failed for: %s", szPckFile);
			delete pFilePackage;
			return false;
		}

		m_FilePcks.push_back(pFilePackage);
		ALOG_INFO("OpenFilePackageInGame: SUCCESS (multipart) pck=[%s], total_pcks=%d", szPckFile, (int)m_FilePcks.size());
		return true;
	}

	// Use default AFilePackage for all other PCKs
	ALOG_DEBUG("OpenFilePackageInGame: Using standard AFilePackage for: %s", szPckFile);
	if (!(pFilePackage = new AFilePackage))
	{
		ALOG_ERROR("OpenFilePackageInGame: Failed to allocate AFilePackage");
		return false;
	}

	if (!((AFilePackage*)pFilePackage)->Open(szPckFile, AFilePackage::OPENEXIST, bEncrypt))
	{
		ALOG_ERROR("OpenFilePackageInGame: AFilePackage::Open failed for: %s", szPckFile);
		delete pFilePackage;
		return false;
	}

	m_FilePcks.push_back(pFilePackage);
	ALOG_INFO("OpenFilePackageInGame: SUCCESS pck=[%s], total_pcks=%d", szPckFile, (int)m_FilePcks.size());
	return true;
}

bool AFilePackMan::OpenFilePackageInGame(const char* szPckFile, const char* szFolder, bool bEncrypt/* false */)
{
	ALOG_INFO("OpenFilePackageInGame(2): pck=[%s] folder=[%s] encrypt=%d", szPckFile, szFolder ? szFolder : "(null)", bEncrypt);
	AFilePackBase* pFilePackage;

	// Check if this is a known multi-part PCK file
	if (strstr(szPckFile, "litmodels.pck") != NULL || strstr(szPckFile, "building.pck") != NULL) {
		// Use MultiPart handler with Python script logic for multi-part PCKs
		ALOG_INFO("OpenFilePackageInGame: Using MultiPart handler for: %s", szPckFile);
		AFERRLOG(("AFilePackMan: Using MultiPart handler for multi-part PCK: %s", szPckFile));

		if (!(pFilePackage = new AFilePackageMultiPart))
		{
			ALOG_ERROR("OpenFilePackageInGame: Failed to allocate AFilePackageMultiPart");
			return false;
		}

		if (!((AFilePackageMultiPart*)pFilePackage)->Open(szPckFile, szFolder, AFilePackage::OPENEXIST, bEncrypt))
		{
			ALOG_ERROR("OpenFilePackageInGame: AFilePackageMultiPart::Open failed for: %s", szPckFile);
			delete pFilePackage;
			return false;
		}

		m_FilePcks.push_back(pFilePackage);
		ALOG_INFO("OpenFilePackageInGame: SUCCESS (multipart) pck=[%s], total_pcks=%d", szPckFile, (int)m_FilePcks.size());
		return true;
	}

	// Use default AFilePackage for all other PCKs
	ALOG_DEBUG("OpenFilePackageInGame: Using standard AFilePackage for: %s", szPckFile);
	if (!(pFilePackage = new AFilePackage))
	{
		ALOG_ERROR("OpenFilePackageInGame: Failed to allocate AFilePackage");
		return false;
	}

	if (!((AFilePackage*)pFilePackage)->Open(szPckFile, szFolder, AFilePackage::OPENEXIST, bEncrypt))
	{
		ALOG_ERROR("OpenFilePackageInGame: AFilePackage::Open failed for: %s", szPckFile);
		delete pFilePackage;
		return false;
	}

	m_FilePcks.push_back(pFilePackage);
	ALOG_INFO("OpenFilePackageInGame: SUCCESS pck=[%s], total_pcks=%d", szPckFile, (int)m_FilePcks.size());
	return true;
}

bool AFilePackMan::CloseFilePackage(AFilePackBase* pFilePck)
{
	int nCount = m_FilePcks.size();
	for(int i=0; i<nCount; i++)
	{
		if( m_FilePcks[i] == pFilePck )
		{
			m_FilePcks.erase(&m_FilePcks[i]);

			pFilePck->Close();
			delete pFilePck;
			return true;
		}
	}

	return true;
}

bool AFilePackMan::CloseAllPackages()
{
	while(m_FilePcks.size())
	{
		AFilePackBase* pFilePck = m_FilePcks[0];

		m_FilePcks.erase(&m_FilePcks[0]);

		pFilePck->Close();
		delete pFilePck;
	}

	m_FilePcks.clear();
	
	return true;
}

AFilePackBase* AFilePackMan::GetFilePck(const char * szPath)
{
	// Disabled - read from disk instead of PCK files
	return NULL;
}

bool AFilePackMan::SetAlgorithmID(int id)
{
	// Set for original implementation (if still used)
	switch(id)
	{
	case 111:
		AFPCK_GUARDBYTE0 = 0xab12908f;
		AFPCK_GUARDBYTE1 = 0xb3231902;
		AFPCK_MASKDWORD  = 0x2a63810e;
		AFPCK_CHECKMASK	 = 0x18734563;
		break;

	default:
		AFPCK_GUARDBYTE0 = 0xfdfdfeee + id * 0x72341f2;
		AFPCK_GUARDBYTE1 = 0xf00dbeef + id * 0x1237a73;
		AFPCK_MASKDWORD  = 0xa8937462 + id * 0xab2321f;
		AFPCK_CHECKMASK	 = 0x59374231 + id * 0x987a223;
		break;
	}

	return true;
}

