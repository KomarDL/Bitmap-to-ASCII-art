#include "Glyph.h"
#include "Path.h"
#include "Font.h"
#include "FirstStartInitialization.h"

HDC CreateMemoryDC(HDC hdc)
{
	HDC hdcResult = CreateCompatibleDC(hdc);
	if (!hdcResult)
	{
		return NULL;
	}

	Font_Initialize(hdcResult);

	RECT rc;
	//Symbol doesn't matter
	DrawTextW(hdc, L"X", -1, &rc, DT_CALCRECT);

	HBITMAP hbm = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	if (!hbm)
	{
		return NULL;
	}
	
	//LOGBRUSH lbr = { 0 };
	//lbr.lbColor = RGB(255, 255, 255);
	//lbr.lbStyle = BS_SOLID;
	//HBRUSH hbr = CreateBrushIndirect(&lbr);

	//SelectObject(hdcResult, hbr);
	SelectObject(hdcResult, hbm);


	//BitBlt(hdcResult, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
	return hdcResult;
}

BOOL FirstStart_CreateDataFiles(HDC hdc, CONST WCHAR szDataPath[], CONST WCHAR szSymbols[], CONST WCHAR szGlyphDirPath[])
{
	BOOL fResult = CreateDirectoryW(szGlyphDirPath, NULL);
	if (fResult)
	{
		PZPWSTR pszFileNames = Path_GetGlyphsFileNames(szSymbols, GLYPH_FILE_EXTENSION);
		fResult = (pszFileNames != NULL);
		if (fResult)
		{
			HDC hdcMem = CreateMemoryDC(hdc);
			
			fResult = (hdcMem != NULL);
			if (fResult)
			{
				SIZE_T i = -1;
				while (++i < wcslen(szSymbols) && fResult)
				{
					PWSTR szPath = Path_GetCombined(szGlyphDirPath, pszFileNames[i]);
					fResult = Glyph_Save(hdcMem, szPath, szSymbols[i]);
					Path_ReleaseCombined(szPath);
				}
				DeleteDC(hdcMem);
			}
		}
		else
		{
			RemoveDirectoryW(szGlyphDirPath);
		}
		Path_ReleaseGlyphsFileNames(pszFileNames, lstrlenW(szSymbols));
	}
	return fResult;
}