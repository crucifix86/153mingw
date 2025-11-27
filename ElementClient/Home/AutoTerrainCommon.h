/*
 * FILE: AutoTerrainCommon.h
 *
 * DESCRIPTION: Class for general data
 *
 * CREATED BY: Jiang Dalong, 2005/04/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <AString.h>
#include <A3DTypes.h>
#include <WINDOWS.H>

const int		HILL_PROFILE_CURVE_POINTS = 7;		// The number of points on curve

// Two blending textures info
struct TEXTUREPARAMINFO
{
	float	fTextureRatio;
	float	fMainTexture;
	float	fPatchTexture;
	float	fPatchDivision;
	float	fMainTextureU;
	float	fMainTextureV;
	float	fPatchTextureU;
	float	fPatchTextureV;
	DWORD	dwTextureSeed;
	AString	strMainTextureFile;
	AString	strPatchTextureFile;
};

// Part terrain generation type.
enum OPERATIONTYPE
{
	OT_NORMAL		= 0,	
	OT_HILL_RECT,			
	OT_HILL_POLY,
	OT_HILL_LINE,
	OT_TEXTURE_POLY,
	OT_WATER,
	OT_ROAD,
	OT_TREE_POLY,
	OT_TREE_LINE,
	OT_GRASS_POLY,
	OT_BUILDING,
	OT_TERRAIN_FLAT,
	OT_LIGHT,
};

// Model operation type
enum MODELOPERATIONTYPE
{
	MOT_NONE		= 0,
	MOT_TRANSLATE,
	MOT_ROTATE,
	MOT_SCALE,
};

// ID and corresponding weight for tree and grass
struct PLANTIDWEIGHTINFO
{
	DWORD	dwID;
	float	fWeight;
};

// ID and corresponding index for tree and grass
struct PLANTIDINDEXINFO
{
	DWORD		dwID;
	int			nIndex;
};

enum LIGHTSTATUS
{
	LS_DAY_ENABLE		= 1,
	LS_NIGHT_ENABLE		= 2,
};


