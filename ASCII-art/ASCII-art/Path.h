#pragma once
#include <Shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#define PATH_CURRENT_DIRECTORY L"."

PWSTR Path_GetCombined(CONST WCHAR szBeginPath[], CONST WCHAR szEndPath[]);

void Path_ReleaseCombined(PWSTR szDirPath);

PWSTR Path_GetGlyphFileName(WORD wSymbolCode, CONST WCHAR szExtention[]);

PZPWSTR Path_GetGlyphsFileNames(CONST WCHAR szSymbols[], CONST WCHAR szExtention[]);

void Path_ReleaseGlyphsFileNames(PZPWSTR pszNames, SIZE_T stLength);

PZPWSTR Path_GetAllGlyphs(CONST WCHAR szSymbols[], CONST WCHAR szExtention[], CONST WCHAR szDirPath[]);