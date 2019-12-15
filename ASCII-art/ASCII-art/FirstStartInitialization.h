#pragma once
#define UNICODE
#define _UNICODE

#include <Windows.h>

BOOL FirstStart_CreateDataFiles(HDC hdc, CONST WCHAR szDataPath[], CONST WCHAR szSymbols[], CONST WCHAR szGlyphDirPath[]);