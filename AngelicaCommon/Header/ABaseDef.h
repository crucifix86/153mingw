/*
 * FILE: ABaseDef.h
 *
 * DESCRIPTION: Base definition
 *
 * CREATED BY: duyuxin, 2003/6/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ABASEDEF_H_
#define _ABASEDEF_H_

#include "ACPlatform.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#ifdef BOLA_DEVBUILD
// Disable all asserts for Wine compatibility - they crash on non-fatal issues
// Override both ASSERT and assert to be no-ops
#ifdef assert
#undef assert
#endif
#define assert(x)	((void)0)
#ifndef ASSERT
#define ASSERT(x)	((void)0)
#endif
#else
// Normal build - use standard assert
#ifndef ASSERT
#include <assert.h>
#define ASSERT	assert
#endif
#endif	//	BOLA_DEVBUILD

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	unsigned value
typedef unsigned long		DWORD;		//	32-bit;
typedef unsigned short 		WORD;		//	16-bit;
typedef unsigned char		BYTE;		//	8-bit;

typedef int					BOOL;
#define TRUE				1
#define FALSE				0

#ifdef UNICODE
	typedef wchar_t			ACHAR;
	#define _AL(str)		L##str
#else
	typedef char			ACHAR;
	#define _AL(str)		str
#endif

//	typedef int				INT
//	commone integer ---- int, but the size undetermined, so not defined, use carefully;

//	signed value
typedef long				LONG;		//	32-bit;
typedef short				SHORT;		//	16-bit;
typedef char				CHAR;		//	8-bit;
typedef float				FLOAT;		//	float
typedef double				DOUBLE;		//	double

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_ABASEDEF_H_
