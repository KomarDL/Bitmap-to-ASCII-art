#pragma once
#define UNICODE
#define _UNICODE

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
	LONG top[3];
	LONG middle[3];
	LONG bottom[3];
} GlBrightness, *PGlBrightness;

BOOL Glyph_Save(HDC hdc, PCWSTR szPath, WCHAR wchSymbol);