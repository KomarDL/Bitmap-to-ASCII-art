#include "Font.h"

BOOL Font_Initialize(HDC hdc)
{

	INT iHeight = -MulDiv(256, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	HFONT hfNew = CreateFontW(iHeight, iHeight, 0, 0, FW_BOLD, 0, 0, 0, OEM_CHARSET,
							OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
							FIXED_PITCH | FF_DONTCARE, NULL);
	SelectObject(hdc, hfNew);
	return TRUE;
}