
#include "UserBitmap.h"

#define SIZEOF_REPLACEMENT_MATRIX 3

PVOID UserBmp_Map(CONST WCHAR szPath[], PHANDLE hFile, PHANDLE hMapping)
{
	*hFile = CreateFileW(szPath, GENERIC_READ, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (*hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	SIZE_T dwFileSize = (SIZE_T)GetFileSize(*hFile, NULL);
	if (dwFileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(*hFile);
		return NULL;
	}

	*hMapping = CreateFileMappingW(*hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (*hMapping == NULL)
	{
		CloseHandle(*hFile);
		return NULL;
	}

	PVOID pvResult = MapViewOfFile(*hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
	if (pvResult == NULL)
	{
		CloseHandle(*hMapping);
		CloseHandle(*hFile);
		return NULL;
	}

	return pvResult;
}

void UserBmp_Unmap(PVOID pvData, HANDLE hFile, HANDLE hMapping)
{
	UnmapViewOfFile(pvData);
	CloseHandle(hMapping);
	CloseHandle(hFile);
}

GlBrightness UserBmp_GetBrightness(PINT32 *ppi32, POINT pntStart)
{
	GlBrightness glbResult = { 0 };
	RECT rc = { 0 };

	rc.left = pntStart.x;
	rc.top = pntStart.y;
	rc.right = rc.left + SIZEOF_REPLACEMENT_MATRIX - 1;
	rc.bottom = rc.top;
	glbResult.up = Glyph_GetAreaBrightness(ppi32, rc);

	rc.left = pntStart.x;
	rc.top = pntStart.y + SIZEOF_REPLACEMENT_MATRIX - 1;
	rc.right = rc.left + SIZEOF_REPLACEMENT_MATRIX - 1;
	rc.bottom = rc.top;
	glbResult.down = Glyph_GetAreaBrightness(ppi32, rc);

	rc.left = pntStart.x;
	rc.top = pntStart.y;
	rc.right = rc.left;
	rc.bottom = rc.top + SIZEOF_REPLACEMENT_MATRIX - 1;
	glbResult.left = Glyph_GetAreaBrightness(ppi32, rc);

	rc.left = pntStart.x + SIZEOF_REPLACEMENT_MATRIX - 1;
	rc.top = pntStart.y;
	rc.right = rc.left;
	rc.bottom = rc.top + SIZEOF_REPLACEMENT_MATRIX - 1;
	glbResult.right = Glyph_GetAreaBrightness(ppi32, rc);

	rc.left = pntStart.x + SIZEOF_REPLACEMENT_MATRIX / 2;
	rc.top = pntStart.y + SIZEOF_REPLACEMENT_MATRIX / 2;;
	rc.right = rc.left;
	rc.bottom = rc.top;
	glbResult.center = Glyph_GetAreaBrightness(ppi32, rc);

	return glbResult;
}

PZPWSTR UserBmp_GenerateAsciiArt(PBYTE pbFileData, CONST WCHAR szSymbols[], CONST PGlBrightness pgb, PPOINT ppntMatrixSize)
{
	CONST BITMAPFILEHEADER bmfHeader = *(PBITMAPFILEHEADER)pbFileData;
	CONST BITMAPINFOHEADER bi = *(PBITMAPINFOHEADER)(pbFileData + sizeof(BITMAPFILEHEADER));
	PBYTE pbBitmap = pbFileData + bmfHeader.bfOffBits;

	//processing 32 bit
	POINT pntArtSize = { bi.biWidth / SIZEOF_REPLACEMENT_MATRIX,
						 bi.biHeight / SIZEOF_REPLACEMENT_MATRIX };


	PINT32 *ppi32 = malloc(bi.biHeight * sizeof(PINT32));
	if (ppi32 == NULL)
	{
		return NULL;
	}

	SIZE_T stOffset = 0;
	SIZE_T stColorBytesInRow = bi.biWidth * 4;
	for (LONG i = 0; i < bi.biHeight; ++i)
	{
		ppi32[i] = (PINT32)(pbBitmap + stOffset);
		stOffset += stColorBytesInRow;
	}

	PZPWSTR pszArtMatrix = malloc(pntArtSize.y * sizeof(PWSTR));
	ppntMatrixSize->y = pntArtSize.y;
	ppntMatrixSize->x = pntArtSize.x + 2;
	for (LONG i = 0; i < pntArtSize.y; ++i)
	{
		pszArtMatrix[i] = calloc(ppntMatrixSize->x, sizeof(WCHAR));
		pszArtMatrix[i][ppntMatrixSize->x - 2] = L'\n';
	}
	
	if (pszArtMatrix == NULL)
	{
		free(ppi32);
		return NULL;
	}

	POINT pnt = { 0 };
	for (SIZE_T i = 0; i < (SIZE_T)pntArtSize.y; ++i)
	{
		for (SIZE_T j = 0; j < (SIZE_T)pntArtSize.x; ++j)
		{
			GlBrightness gb = UserBmp_GetBrightness(ppi32, pnt);
			pszArtMatrix[i][j] = Replace_GetSymbol(szSymbols, pgb, gb);
			pnt.y += SIZEOF_REPLACEMENT_MATRIX;
		}
		pnt.y = 0;
		pnt.x += SIZEOF_REPLACEMENT_MATRIX;
	}

	return pszArtMatrix;
}

PZPWSTR UserBmp_ProcessFile(CONST WCHAR szPath[], CONST WCHAR szSymbols[], CONST PGlBrightness pgb, PPOINT ppntMatrixSize)
{
	//need not null handles
	HANDLE	hFile	 = (HANDLE)100,
			hMapping = (HANDLE)100;
	PBYTE pbFileData = UserBmp_Map(szPath, &hFile, &hMapping);

	if (pbFileData == NULL)
	{
		return NULL;
	}

	if (!UserBmp_IsSupported(pbFileData))
	{
		UserBmp_Unmap((PVOID)pbFileData, hFile, hMapping);
		return NULL;
	}

	PZPWSTR szResult = UserBmp_GenerateAsciiArt(pbFileData, szSymbols, pgb, ppntMatrixSize);

	UserBmp_Unmap((PVOID)pbFileData, hFile, hMapping);
	return szResult;
}

BOOL UserBmp_IsSupported(PBYTE pbFileData)
{
	CONST BITMAPFILEHEADER bmfHeader = *(PBITMAPFILEHEADER)pbFileData;
	CONST BITMAPINFOHEADER bi = *(PBITMAPINFOHEADER)(pbFileData + sizeof(BITMAPFILEHEADER));
	
	return (bmfHeader.bfType == BFTYPE_BM && bi.biBitCount == 32 && bi.biCompression == BI_RGB);
}