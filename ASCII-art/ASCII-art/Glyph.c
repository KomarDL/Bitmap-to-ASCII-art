#include <math.h>
#include "Glyph.h"

#define PARTS_AMOUNT 3
#define GAMMA_CORRECTION(colorValue) (255 * pow(colorValue / 255.0, 1.0 / 2.2))
#define KB 0.114
#define KR 0.299

BOOL Glyph_Save(HDC hdcMem, PCWSTR szPath, WCHAR wchSymbol)
{
	RECT rc = { 0 };
	if (wchSymbol == L'&')
	{
		DrawTextW(hdcMem, L"&&", -1, &rc, DT_CALCRECT);
		DrawTextW(hdcMem, L"&&", -1, &rc, 0);
	}
	else
	{
		DrawTextW(hdcMem, &wchSymbol, 1, &rc, DT_CALCRECT);
		DrawTextW(hdcMem, &wchSymbol, 1, &rc, 0);
	}

	// Get the BITMAP from the HBITMAP
	BITMAP bm;
	HBITMAP hbm = GetCurrentObject(hdcMem, OBJ_BITMAP);
	if (!GetObjectW(hbm, sizeof(BITMAP), &bm))
	{
		return FALSE;
	}

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi = { 0 };
	
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;

	DWORD dwBmpSize = ((bm.bmWidth * bi.biBitCount + 31) / 32) * 4 * bm.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcMem, hbm, 0, (UINT)bm.bmHeight, lpbitmap, (PBITMAPINFO)&bi, DIB_RGB_COLORS);

	// A file is created, this is where we will save the glyph.
	HANDLE hFile = CreateFileW(szPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//Unlock and Free the DIB from the heap
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
		return FALSE;
	}

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = BFTYPE_BM; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFile);
	return TRUE;
}

PGlBrightness Glyph_LoadAllBrightness(PCWSTR szDataFile, SIZE_T stLength)
{
	PGlBrightness pgbResult = calloc(stLength, sizeof(GlBrightness));
	if (pgbResult == NULL)
	{
		return NULL;
	}

	HANDLE hFile = CreateFileW(szDataFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		free(pgbResult);
		return NULL;
	}

	for (SIZE_T i = 0; i < stLength && pgbResult != NULL; ++i)
	{
		if (!ReadFile(hFile, &pgbResult[i], sizeof(GlBrightness), NULL, NULL))
		{
			free(pgbResult);
			pgbResult = NULL;
		}
	}
	CloseHandle(hFile);
	return pgbResult;
}

BYTE Glyph_GetAreaBrightness(PINT32 *ppi32Bmp, RECT rc)
{
	DOUBLE ul64Acumulatoare = 0;
	SIZE_T ulElementAmount = (rc.right - rc.left + 1) * (rc.bottom - rc.top + 1);

	for (LONG i = rc.left; i <= rc.right; ++i)
	{
		for (LONG j = rc.top; j <= rc.bottom; ++j)
		{
			DOUBLE firstTerm = KR * GAMMA_CORRECTION(GetRValue(ppi32Bmp[i][j]));
			DOUBLE secondTerm = (1 - KR - KB) * GAMMA_CORRECTION(GetGValue(ppi32Bmp[i][j]));
			DOUBLE thirdTerm = KB * GetBValue(ppi32Bmp[i][j]);
			ul64Acumulatoare += (firstTerm + secondTerm + thirdTerm);
		}
	}
	BYTE bResult = (BYTE)(ul64Acumulatoare / ulElementAmount);
	return bResult;
}

GlBrightness Gl_GetBrightness(CONST PBYTE pbBmpFile)
{
	GlBrightness glbResult = { 0 };
	CONST BITMAPFILEHEADER bmfHeader = *(PBITMAPFILEHEADER)pbBmpFile;
	CONST BITMAPINFOHEADER bi = *(PBITMAPINFOHEADER)(pbBmpFile + sizeof(BITMAPFILEHEADER));
	PBYTE pbBitmap = pbBmpFile + bmfHeader.bfOffBits;

	DWORD dwHeightStep	= bi.biHeight / PARTS_AMOUNT;
	DWORD dwWidthStep	= bi.biWidth / PARTS_AMOUNT;
	//DWORD dwUselessBytesAmount = (4 - (bi.biWidth * 4) % 4) % 4;

	PINT32 *ppi32 = malloc(bi.biHeight * sizeof(PINT32));
	if (ppi32 == NULL)
	{
		return glbResult;
	}
	
	SIZE_T stOffset = 0;
	SIZE_T stColorBytesInRow = bi.biWidth * 4;
	for (LONG i = 0; i < bi.biHeight; ++i)
	{
		ppi32[i] = (PINT32)(pbBitmap + stOffset);
		stOffset += stColorBytesInRow;
	}

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.bottom = dwHeightStep;
	rc.right = bi.biWidth - 1;
	glbResult.up = Glyph_GetAreaBrightness(ppi32, rc);

	rc.top = bi.biHeight - 1 - dwHeightStep;
	rc.left = 0;
	rc.bottom = bi.biHeight - 1;
	rc.right = bi.biWidth - 1;
	glbResult.down = Glyph_GetAreaBrightness(ppi32, rc);

	rc.top = 0;
	rc.left = 0;
	rc.bottom = bi.biHeight - 1;
	rc.right = dwWidthStep;
	glbResult.left = Glyph_GetAreaBrightness(ppi32, rc);
	
	rc.top = 0;
	rc.left = bi.biWidth - 1 - dwWidthStep;
	rc.bottom = bi.biHeight - 1;
	rc.right = bi.biWidth - 1;
	glbResult.right = Glyph_GetAreaBrightness(ppi32, rc);

	rc.top = dwHeightStep + 1;
	rc.left = dwWidthStep + 1;
	rc.bottom = bi.biHeight - 1 - dwHeightStep;
	rc.right = bi.biWidth - 1 - dwWidthStep;
	glbResult.center= Glyph_GetAreaBrightness(ppi32, rc);

	free(ppi32);
	return glbResult;
}

PGlBrightness Glyph_GetBrightness(WCHAR szPath[])
{
	HANDLE hFile = CreateFileW(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	SIZE_T dwFileSize = (SIZE_T)GetFileSize(hFile, NULL);
	if (dwFileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return NULL;
	}

	HANDLE hMapping = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMapping == NULL)
	{
		CloseHandle(hFile);
		return NULL;
	}

	PGlBrightness pgbResult = NULL;
	PVOID pvData = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
	if (pvData == NULL)
	{
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return NULL;
	}
	
	pgbResult = calloc(1, sizeof(GlBrightness));
	if (pgbResult == NULL)
	{
		UnmapViewOfFile(pvData);
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return NULL;
	}

	GlBrightness glbTmp = Gl_GetBrightness(pvData);
	if (!glbTmp.center && !glbTmp.down && !glbTmp.left && !glbTmp.right && !glbTmp.up)
	{
		pgbResult = NULL;
	}
	else
	{
		memcpy_s(pgbResult, sizeof(GlBrightness), &glbTmp, sizeof(GlBrightness));
	}

	UnmapViewOfFile(pvData);
	CloseHandle(hMapping);
	CloseHandle(hFile);

	return pgbResult;
}