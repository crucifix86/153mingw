// DirectShow compatibility macros for MinGW
// These are defined in Microsoft's DirectShow SDK but not in MinGW

#ifndef _DSHOW_COMPAT_H_
#define _DSHOW_COMPAT_H_

// WIDTHBYTES - Calculate scanline width in bytes, rounded up to DWORD
#ifndef WIDTHBYTES
#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)
#endif

// DIBWIDTHBYTES - Calculate DIB scanline width
#ifndef DIBWIDTHBYTES
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((DWORD)(bi).biWidth * (DWORD)(bi).biBitCount)
#endif

// _DIBSIZE - DIB size without overflow checking
#ifndef _DIBSIZE
#define _DIBSIZE(bi) (DIBWIDTHBYTES(bi) * (DWORD)(bi).biHeight)
#endif

// DIBSIZE - Calculate total size of a DIB
#ifndef DIBSIZE
#define DIBSIZE(bi) ((bi).biHeight < 0 ? (-1)*(_DIBSIZE(bi)) : _DIBSIZE(bi))
#endif

// Safe DIBSIZE with overflow checking
#ifndef SAFE_DIBSIZE
#define SAFE_DIBSIZE(pbi, pdw) \
    ((((pbi)->biWidth > 0) && ((pbi)->biHeight != 0) && \
     ((DWORD)(pbi)->biBitCount <= 64) && \
     (DIBWIDTHBYTES(*(pbi)) <= 0xFFFFFFFF / (DWORD)((pbi)->biHeight < 0 ? -(pbi)->biHeight : (pbi)->biHeight))) ? \
     (*(pdw) = DIBSIZE(*(pbi)), S_OK) : E_INVALIDARG)
#endif

// Video info header size constants
#ifndef SIZE_PREHEADER
#define SIZE_PREHEADER (FIELD_OFFSET(VIDEOINFOHEADER, bmiHeader))
#endif

#ifndef SIZE_VIDEOHEADER
#define SIZE_VIDEOHEADER (sizeof(BITMAPINFOHEADER) + SIZE_PREHEADER)
#endif

// Mask/palette size calculations
#ifndef SIZE_MASKS
#define SIZE_MASKS (3 * sizeof(DWORD))
#endif

#ifndef SIZE_PALETTE
#define SIZE_PALETTE (256 * sizeof(RGBQUAD))
#endif

// Check if format is palettized (returns boolean)
#ifndef PALETTISED
#define PALETTISED(pbmi) ((pbmi)->bmiHeader.biBitCount <= 8)
#endif

// TRUECOLOR - Get BITMAPINFO pointer (for truecolor formats, cast to BITMAPINFO*)
#ifndef TRUECOLOR
#define TRUECOLOR(pbmi) ((BITMAPINFO *)(pbmi))
#endif

// COLORS - Get pointer to palette entries
#ifndef COLORS
#define COLORS(pbmi) ((RGBQUAD *)((LPBYTE)(pbmi) + (pbmi)->bmiHeader.biSize))
#endif

// Reset palette entry
#ifndef RESET_PALETTE
#define RESET_PALETTE 256
#endif

// Header containment check
#ifndef HEADER
#define HEADER(pVideoInfo) (&(((VIDEOINFOHEADER *)(pVideoInfo))->bmiHeader))
#endif

// Note: ContainsPalette macro removed - conflicts with function in amvideo.cpp

// BITMASKS - pointer to bitmasks for BI_BITFIELDS
#ifndef BITMASKS
#define BITMASKS(pbmi) ((DWORD *)((LPBYTE)(pbmi) + (pbmi)->bmiHeader.biSize))
#endif

// PALETTE_ENTRIES - calculate palette entry count
#ifndef PALETTE_ENTRIES
#define PALETTE_ENTRIES(pbmi) ((DWORD)((pbmi)->bmiHeader.biClrUsed ? (pbmi)->bmiHeader.biClrUsed : (1 << (pbmi)->bmiHeader.biBitCount)))
#endif

// Note: GetBitCount macro removed - conflicts with function in wxutil.h
// Use HEADER(pVideoInfo)->biBitCount instead if needed

#endif // _DSHOW_COMPAT_H_
