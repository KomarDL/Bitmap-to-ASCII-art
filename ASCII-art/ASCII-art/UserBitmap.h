#pragma once

#include <Windows.h>
#include "Glyph.h"
#include "Replace.h"

PVOID UserBmp_Map(CONST WCHAR szPath[], PHANDLE hFile, PHANDLE hMapping);
void UserBmp_Unmap(PVOID pvData, HANDLE hFile, HANDLE hMapping);
PZPWSTR UserBmp_GenerateAsciiArt(PBYTE pbFileData, CONST WCHAR szSymbols[], CONST PGlBrightness pgb, PPOINT ppntMatrixSize);
PZPWSTR UserBmp_ProcessFile(CONST WCHAR szPath[], CONST WCHAR szSymbols[], CONST PGlBrightness pgb, PPOINT ppntMatrixSize);
BOOL UserBmp_IsSupported(PBYTE pbFileData);

