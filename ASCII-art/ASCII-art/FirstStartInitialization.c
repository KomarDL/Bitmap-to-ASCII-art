#include "Glyph.h"
#include "Path.h"
#include "Font.h"
#include "FirstStartInitialization.h"

HDC Fi_CreateMemoryDC(HDC hdc)
{
	HDC hdcResult = CreateCompatibleDC(hdc);
	if (!hdcResult)
	{
		return NULL;
	}

	Font_InitializeFirstTime(hdcResult);

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

BOOL Fi_SaveAllGlyphs(HDC hdc, PZPCWSTR pszFileNames, CONST WCHAR szSymbols[], CONST WCHAR szGlyphDirPath[])
{
	HDC hdcMem = Fi_CreateMemoryDC(hdc);

	BOOL fResult = (hdcMem != NULL);
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
	return fResult;
}

PGlBrightness* Fi_GetBrightness(PZPCWSTR pszFileNames, CONST WCHAR szGlyphDirPath[], SIZE_T stResultLenght)
{
	BOOL fResult = TRUE;
	PGlBrightness* ppgbResult = calloc(stResultLenght, sizeof(GlBrightness));
	if (ppgbResult != NULL)
	{
		SIZE_T i = -1;

		while (++i < stResultLenght && fResult)
		{
			PWSTR szPath = Path_GetCombined(szGlyphDirPath, pszFileNames[i]);
			ppgbResult[i] = Glyph_GetBrightness(szPath);
			fResult = (ppgbResult[i] != NULL);
			Path_ReleaseCombined(szPath);
		}
		
		if (!fResult)
		{
			for (SIZE_T j = 0; j < i; ++j)
			{
				free(ppgbResult[i]);
			}
			free(ppgbResult);
			ppgbResult = NULL;
		}
	}
	return ppgbResult ;
}

BOOL Fi_SaveBrightness(PGlBrightness* ppgb, SIZE_T stArrLenght,CONST WCHAR szPath[])
{
	HANDLE hFile = CreateFileW(szPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
								CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL fContinue = TRUE;
	for (SIZE_T i = 0; i < stArrLenght && fContinue; ++i)
	{
		DWORD dwBytesWritten = 0;
		WriteFile(hFile, (LPVOID)ppgb[i], sizeof(GlBrightness), &dwBytesWritten, NULL);
	}

	CloseHandle(hFile);
	if (!fContinue)
	{
		DeleteFileW(szPath);
	}
	return fContinue;
}

BOOL FirstStart_CreateDataFiles(HDC hdc, CONST WCHAR szDataPath[], CONST WCHAR szSymbols[], CONST WCHAR szGlyphDirPath[])
{
	PZPWSTR pszFileNames;
	BOOL fResult = CreateDirectoryW(szGlyphDirPath, NULL);
	if (fResult)
	{
		pszFileNames = Path_GetGlyphsFileNames(szSymbols, GLYPH_FILE_EXTENSION);
		fResult = (pszFileNames != NULL);
		if (fResult)
		{
			//if all glyphs saved successfully
			if (Fi_SaveAllGlyphs(hdc, pszFileNames, szSymbols, szGlyphDirPath))
			{
				PGlBrightness* ppgb = Fi_GetBrightness(pszFileNames, szGlyphDirPath, wcslen(szSymbols));
				fResult = (ppgb != NULL);
				//if brightness geted successfully
				if (fResult)
				{
					fResult = Fi_SaveBrightness(ppgb, wcslen(szSymbols), szDataPath);
				}
			}
			Path_ReleaseGlyphsFileNames(pszFileNames, wcslen(szSymbols));
		}
		else
		{
			RemoveDirectoryW(szGlyphDirPath);
		}
	}
	return fResult;
}