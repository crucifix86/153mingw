/*
 * FILE: A3DVertexShader.cpp
 *
 * DESCRIPTION: Routines for vertex shader
 *
 * CREATED BY: duyuxin, 2003/10/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DVertexShader.h"
#include "A3DPI.h"
#include "A3DDevice.h"
#include "AF.h"
#include "A3DMacros.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DVertexShader
//
///////////////////////////////////////////////////////////////////////////

A3DVertexShader* A3DVertexShader::m_pCurShader = NULL;

A3DVertexShader::A3DVertexShader()
{
	m_dwClassID			= A3D_CID_VERTEXSHADER;
	m_pA3DDevice		= NULL;
	m_pD3DXShaderBuf	= NULL;
#ifdef DX8
	m_hD3DShader		= NULL;
#else
	m_pD3DShader = NULL;
	m_pDecl = NULL;
#endif
	m_dwShaderID		= 0;
	m_dwPosInMan		= 0;
}

A3DVertexShader::~A3DVertexShader()
{
}

/*	Initialize object

	Return true for success, otherwise return false;
*/
bool A3DVertexShader::Init(A3DDevice* pDevice)
{
	m_pA3DDevice = pDevice;
	return true;
}

//	Release object
void A3DVertexShader::Release()
{
	if (!m_pA3DDevice)
		return;

#ifdef DX8
	IDirect3DDevice8* pD3DDevice = m_pA3DDevice->GetD3DDevice();
	if (m_hD3DShader)
	{
		pD3DDevice->DeleteVertexShader(m_hD3DShader);
		m_hD3DShader = NULL;
	}
#else
	IDirect3DDevice9* pD3DDevice = m_pA3DDevice->GetD3DDevice();
	if (m_pD3DShader)
	{
		m_pD3DShader->Release();
		m_pD3DShader = NULL;
	}

	if (m_pDecl)
	{
		A3DRELEASE(m_pDecl);
	}

#endif

	if (m_pD3DXShaderBuf)
	{
		m_pD3DXShaderBuf->Release();
		m_pD3DXShaderBuf = NULL;
	}
}

/*	Load vertex shader from file

	Return true for success, otherwise return false;

	szFile: file name
	bBinary: true, load binary vertex file. false, load ascii vertex file
	pdwDecl: vertex shader declarator
*/
#ifdef DX8
bool A3DVertexShader::Load(const char* szFile, bool bBinary, DWORD* pdwDecl)
#else
bool A3DVertexShader::Load(const char* szFile, bool bBinary, D3DVERTEXELEMENT9* pdwDecl)
#endif
{
	char szFullFileName[MAX_PATH];
	af_GetFullPath(szFullFileName, (char*)szFile);

	HRESULT hr;

	if (bBinary)
	{
		AFileImage File;

		if (!File.Open(szFullFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
		{
			File.Close();
			g_A3DErrLog.Log("A3DVertexShader::Load, Cannot open file %s!", szFullFileName);
			return false;
		}

		if (D3DXCreateBuffer(File.GetFileLength(), &m_pD3DXShaderBuf) != D3D_OK)
		{
			File.Close();
		//	g_A3DErrLog.Log("A3DVertexShader::Load, Failed to create D3DXBuffer!");
			return false;
		}

		//	Copy file data
		memcpy(m_pD3DXShaderBuf->GetBufferPointer(), File.GetFileBuffer(), File.GetFileLength());

		File.Close();
	}
	else	//	Load ascii file
	{
		AFileImage File;

		if (!File.Open(szFullFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
		{
			File.Close();
			g_A3DErrLog.Log("A3DVertexShader::Load, Cannot open file %s!", szFullFileName);
			return false;
		}

#ifdef DX8
		if ((hr = D3DXAssembleShader(File.GetFileBuffer(), File.GetFileLength(), 0, NULL, &m_pD3DXShaderBuf, NULL)) != D3D_OK)
#else
		if ((hr = D3DXAssembleShader((LPCSTR)File.GetFileBuffer(), File.GetFileLength(), 0, NULL, 0, &m_pD3DXShaderBuf, NULL)) != D3D_OK)
#endif
		{
			g_A3DErrLog.Log("A3DVertexShader::Load, Failed to load vertex shader. %s", szFullFileName);
			return false;
		}

		File.Close();
	}

#ifdef DX8
	IDirect3DDevice8* pD3DDevice = m_pA3DDevice->GetD3DDevice();
#else
	IDirect3DDevice9* pD3DDevice = m_pA3DDevice->GetD3DDevice();
#endif
	DWORD dwUsage = 0;
	
	if( !m_pA3DDevice->IsHardwareTL() || !m_pA3DDevice->TestVertexShaderVersion(1, 1) )
		dwUsage = D3DUSAGE_SOFTWAREPROCESSING;

#ifdef DX8
	if ((hr = pD3DDevice->CreateVertexShader(pdwDecl, (DWORD*)m_pD3DXShaderBuf->GetBufferPointer(),
											&m_hD3DShader, dwUsage)) != D3D_OK)
#else
	if ((hr = pD3DDevice->CreateVertexShader((DWORD*)m_pD3DXShaderBuf->GetBufferPointer(), &m_pD3DShader)) != D3D_OK)
#endif
	{
		g_A3DErrLog.Log("A3DVertexShader::Load, Failed to create vertex shader.");
		return false;
	}

	//	Record file name
	af_GetRelativePath(szFullFileName, m_strFileName);
	m_dwShaderID = a_MakeIDFromLowString(m_strFileName);

	//	After DX 8.1, vertex shaders needn't to be recreated when device reset, 
	//	they will be remebered by DX automatically.	so we release vertex buffer here.
	m_pD3DXShaderBuf->Release();
	m_pD3DXShaderBuf = NULL;

	//Create vert decl
	if (m_pDecl)
	{
		A3DRELEASE(m_pDecl);
	}
	m_pDecl = new A3DVertexDecl;
	if (!m_pDecl->Init(m_pA3DDevice, pdwDecl))
	{
		delete m_pDecl;
		m_pDecl = NULL;
		assert(false);
	}

	return true;
}

/*	Apply vertex shader

	Return true for success, otherwise return false.
*/
bool A3DVertexShader::Appear()
{
#ifdef DX8
	if (!m_hD3DShader)
#else
	if (!m_pD3DShader)
#endif
		return false;

	if (m_pCurShader == this)
		return true;

#ifdef DX8
	if (!m_pA3DDevice->SetDeviceVertexShader(m_hD3DShader))
	{
		g_A3DErrLog.Log("A3DVertexShader::Appear, Failed to apply vertex shader.");
		return false;
	}
#else
	if (m_pDecl)
	{
		m_pDecl->Appear();
		//m_pA3DDevice->SetHLSLShader(NULL);
		m_pA3DDevice->GetD3DDevice()->SetVertexShader(m_pD3DShader);
		m_pA3DDevice->ClearDeviceVertexShader();
	}
	else
	{
		assert(false);
		return false;
	}
#endif
	m_pCurShader = this;

	return true;
}

//	Abolish vertex shader
bool A3DVertexShader::Disappear()
{
	if (m_pCurShader == this)
	{
		m_pA3DDevice->ClearVertexShader();
		m_pCurShader = NULL;
	}

	return true;
}

#ifdef DX8
#else
///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DVertexDecl
//
///////////////////////////////////////////////////////////////////////////

A3DVertexDecl::A3DVertexDecl()
{
	m_pA3DDevice = NULL;
	m_pD3DVertDecl = NULL;
}

A3DVertexDecl::~A3DVertexDecl()
{
}

//	Initlaize object
bool A3DVertexDecl::Init(A3DDevice* pDevice, const D3DVERTEXELEMENT9* aDecls)
{
	m_pA3DDevice = pDevice;
	if (!m_pA3DDevice)
		return true;

	if (m_pA3DDevice->GetD3DDevice()->CreateVertexDeclaration(aDecls, &m_pD3DVertDecl) != D3D_OK)
		return false;

	return true;
}

//	Release object
void A3DVertexDecl::Release()
{
	if (m_pD3DVertDecl)
	{
		m_pD3DVertDecl->Release();
		m_pD3DVertDecl = NULL;
	}
}

//	Apply vertex declaration to device
bool A3DVertexDecl::Appear()
{
	if (!m_pD3DVertDecl)
		return false;

	return m_pA3DDevice->GetD3DDevice()->SetVertexDeclaration(m_pD3DVertDecl) == D3D_OK;
}

#endif
