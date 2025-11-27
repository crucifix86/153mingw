/*
 * FILE: A3DVertexShader.h
 *
 * DESCRIPTION: Routines for vertex shader
 *
 * CREATED BY: duyuxin, 2003/10/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DVERTEXSHADER_H_
#define _A3DVERTEXSHADER_H_

#include "A3DVertex.h"
#include "A3DDevObject.h"
#include "AString.h"

#ifdef DX8
#include <D3dx8core.h>
#else
#include "A3DPlatform.h"
#endif

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
	
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////
#ifdef DX8
#else
class A3DVertexDecl
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DVertexDecl();
	virtual ~A3DVertexDecl();

public:		//	Attributes

	//	Initlaize object
	bool Init(A3DDevice* pDevice, const D3DVERTEXELEMENT9* aDecls);
	//	Release object
	void Release();

	//	Apply vertex declaration to device
	bool Appear();

public:		//	Operations

	//	Get D3D vertex declaration
	IDirect3DVertexDeclaration9* GetD3DVertDecl() { return m_pD3DVertDecl; }

protected:	//	Attributes

	A3DDevice* m_pA3DDevice;		//	A3D device object

	IDirect3DVertexDeclaration9* m_pD3DVertDecl;		//	D3D vertex declaration

protected:	//	Operations

};
#endif
///////////////////////////////////////////////////////////////////////////
//
//	Class A3DVertexShader
//
///////////////////////////////////////////////////////////////////////////

class A3DVertexShader : public A3DDevObject
{
public:		//	Types

	friend class A3DShaderMan;

public:		//	Constructors and Destructors

	A3DVertexShader();
	virtual ~A3DVertexShader();

public:		//	Attributes

	static A3DVertexShader*	m_pCurShader;	//	Current applied vertex shader

public:		//	Operations

	//	Initialize object
	bool Init(A3DDevice* pDevice);
	//	Release object
	void Release();	

	//	Load vertx shader from file
#ifdef DX8
	bool Load(const char* szFile, bool bBinary, DWORD* pdwDecl);
#else
	bool Load(const char* szFile, bool bBinary, D3DVERTEXELEMENT9* pdwDecl);

#endif
	//	Apply this vertex shader
	bool Appear();
	//	Abolish vertex shader
	bool Disappear();

	//	Before device reset
	virtual bool BeforeDeviceReset() { return true; }
	//	After device reset
	virtual bool AfterDeviceReset() { return true; }

	//	Get vertex shader file name
	const char* GetFileName() { return m_strFileName; }
	//	Get vertex shader ID
	DWORD GetShaderID() { return m_dwShaderID; }

protected:	//	Attributes

	A3DDevice*		m_pA3DDevice;			//	Device pointer
	AString			m_strFileName;			//	File name relative to shader path
	ID3DXBuffer*    m_pD3DXShaderBuf;		//	Vertex shader code buffer
#ifdef DX8
	DWORD			m_hD3DShader;			//	D3D vertex shader handle
#else
	IDirect3DVertexShader9* m_pD3DShader;			//	D3D vertex shader handle
	A3DVertexDecl* m_pDecl;				//  vertex decl
#endif
	DWORD			m_dwShaderID;			//	Vertex shader ID
	DWORD			m_dwPosInMan;			//	Position in manager

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DVERTEXSHADER_H_


