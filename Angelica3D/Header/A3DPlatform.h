/*
 * FILE: A3DPlatform.h
 *
 * DESCRIPTION: header for a platform related headers list
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DPLATFORM_H_
#define _A3DPLATFORM_H_

#include <Windows.h>
#include <StdIO.h>

#ifdef DX8
#include <d3d8.h>
#include <d3dx8.h>
#define SHADER_DIR "Shaders\\"
#elif defined(DX9)
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <d3d9types.h>
#include <d3dx9math.h>
#define SHADER_DIR "Shaders\\2.2\\"
#else
#pragma message("Missing DX option")
#endif

#include <assert.h>

#include <FastMemcpy.h>

//	#include <AC.h> 
//	#include <AF.h>

#endif

