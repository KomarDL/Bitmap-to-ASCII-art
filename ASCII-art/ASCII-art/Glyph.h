#pragma once

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdbool.h>

#define GLYPH_PALLETE_SIZE 65
#define GLYPH_MAX_COLOR_SHADES 256
#define GLYPH_DIRECTORY_NAME _T("Glyphs")

PBYTE Glyph_Get(HDC hdc, UINT uSymbol, PDWORD pdwBufferSize);

SIZE Glyph_GetSize(HDC hdc, UINT uSymbol);

void Glyph_Release(PBYTE* pbGlyph);

void Glyph_Save(BYTE pbData[], DWORD dwDataSize, SIZE sBitmap, PTCHAR szPath);