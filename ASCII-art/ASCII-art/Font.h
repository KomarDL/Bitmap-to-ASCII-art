#pragma once
#include <Windows.h>

BOOL Font_InitializeFirstTime(HDC hdc);
BOOL Font_Initialize(HDC hdc);
HFONT Font_Select(HDC hdc, PWSTR pszText, const RECT rCurrentCell);