#pragma once

#include <stdbool.h>
#include <tchar.h>
#include <Shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#define PATH_CURRENT_DIRECTORY _T(".")

PTCHAR Path_GetCombined(TCHAR szBeginPath[], TCHAR szEndPath[]);

void Path_ReleaseCombined(PTCHAR *pszDirPath);