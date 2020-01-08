#include <wchar.h>
#include "Font.h"

BOOL Font_InitializeFirstTime(HDC hdc)
{

	INT iHeight = -MulDiv(256, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	HFONT hfNew = CreateFontW(iHeight, iHeight, 0, 0, FW_BOLD, 0, 0, 0, OEM_CHARSET,
							OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
							FIXED_PITCH | FF_DONTCARE, NULL);
	SelectObject(hdc, hfNew);
	return TRUE;
}

BOOL Font_IsRectSmaller(RECT rFirst, RECT rSecond)
{
	LONG lWidthDifference = (rFirst.right - rFirst.left) - (rSecond.right - rSecond.left);
	LONG lHeightDifference = (rFirst.bottom - rFirst.top) - (rSecond.bottom - rSecond.top);

	return ((lWidthDifference < 0) || (lHeightDifference < 0)) ? TRUE : FALSE;
}

LOGFONTW Font_GetDownscaledLogicalFont(HDC hdc)
{
	PWSTR szFontName[LF_FACESIZE];
	INT iLength = LF_FACESIZE;
	GetTextFaceW(hdc, iLength, (LPWSTR)szFontName);

	TEXTMETRICW tmFontData;
	GetTextMetricsW(hdc, &tmFontData);

	LOGFONTW lfResult;
	ZeroMemory(&lfResult, sizeof(LOGFONTW));

	lfResult.lfHeight = tmFontData.tmHeight - 1;
	lfResult.lfWidth = tmFontData.tmHeight - 1;
	lfResult.lfWeight = tmFontData.tmWeight;
	lfResult.lfItalic = tmFontData.tmItalic;
	lfResult.lfUnderline = tmFontData.tmUnderlined;
	lfResult.lfStrikeOut = tmFontData.tmStruckOut;
	lfResult.lfCharSet = tmFontData.tmCharSet;
	lfResult.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	lfResult.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lfResult.lfQuality = DEFAULT_QUALITY;
	lfResult.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
	wmemcpy_s(lfResult.lfFaceName, LF_FACESIZE, (PCWSTR)szFontName, iLength);
	return lfResult;
}

HFONT Font_Downscale(HDC hdc)
{
	LOGFONTW lfNewLogicalFont = Font_GetDownscaledLogicalFont(hdc);
	HFONT hfResult = CreateFontIndirectW(&lfNewLogicalFont);
	SelectObject(hdc, hfResult);
	return hfResult;
}

HFONT Font_Select(HDC hdc, PWSTR pszText, const RECT rCurrentCell)
{
	RECT rRequiredForText = rCurrentCell;
	HFONT hfResult = NULL;
	TEXTMETRIC tmFontData;
	GetTextMetricsW(hdc, &tmFontData);
	DrawTextW(hdc, pszText, -1, &rRequiredForText, DT_NOPREFIX | DT_CALCRECT);
	BOOL fIsDefaultFont = TRUE;
	while (Font_IsRectSmaller(rCurrentCell, rRequiredForText) && (tmFontData.tmAveCharWidth > 1))
	{
		if (hfResult != NULL)
		{
			DeleteObject(hfResult);
			hfResult = NULL;
		}
		hfResult = Font_Downscale(hdc);
		DrawTextW(hdc, pszText, -1, &rRequiredForText, DT_NOPREFIX | DT_CALCRECT);
		GetTextMetricsW(hdc, &tmFontData);
	}
	return hfResult;
}