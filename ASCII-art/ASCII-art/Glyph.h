#pragma once
//#define UNICODE
//#define _UNICODE

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdbool.h>

#define GLYPH_PALLETE_SIZE 65
#define GLYPH_MAX_COLOR_SHADES 256
#define GLYPH_FILE_EXTENSION L".bmp"

//PBYTE Glyph_Get(HDC hdc, UINT uSymbol, PDWORD pdwBufferSize);
//
//SIZE Glyph_GetSize(HDC hdc, UINT uSymbol);
//
//void Glyph_Release(PBYTE pbGlyph);
//
//void Glyph_Save(BYTE pbData[], DWORD dwDataSize, SIZE sBitmap, CONST WCHAR szPath[]);

typedef struct _GlyphBrightness
{
	BYTE up, down;
	BYTE left, right;
	BYTE center;
} GlBrightness, *PGlBrightness;

BOOL Glyph_Save(HDC hdc, PCWSTR szPath, WCHAR wchSymbol);
PGlBrightness Glyph_GetBrightness(WCHAR szPath[]);