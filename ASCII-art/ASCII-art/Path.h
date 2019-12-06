#pragma once

#include <stdbool.h>
#include <tchar.h>
#include <Shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#define PATH_CURRENT_DIRECTORY L"."

PWSTR Path_GetCombined(WCHAR szBeginPath[], WCHAR szEndPath[]);

void Path_ReleaseCombined(PWSTR szDirPath);

PWSTR Path_GetGlyphFileName(WORD wSymbolCode, CONST WCHAR szExtention[]);

PZPWSTR Path_GetGlyphsFileNames(CONST WCHAR szSymbols[], CONST WCHAR szExtention[]);