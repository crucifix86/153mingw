/*
 * FILE: ACPlatform.h
 *
 * DESCRIPTION: header for a platform related headers list
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ACPLATFORM_H_
#define _ACPLATFORM_H_

// Prevent Windows.h from defining min/max macros that conflict with STL
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <StdIO.h>
#include <StdLib.h>

#include <assert.h>

// For MinGW: use std::min/max from algorithm (already included by sal_fix.h)
// and bring them into global namespace for compatibility
#ifndef _MSC_VER
#include <algorithm>
using std::min;
using std::max;
#else
// For MSVC, Windows.h with NOMINMAX won't define min/max,
// define manually if needed
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif

// Also provide a_min/a_max for explicit use
template<class T> inline const T& a_min(const T& a, const T& b) { return (a < b) ? a : b; }
template<class T> inline const T& a_max(const T& a, const T& b) { return (a > b) ? a : b; }

#endif

