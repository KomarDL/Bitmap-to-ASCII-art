#include "Glyph.h"

SIZE Glyph_GetSize(HDC hdc, UINT uSymbol)
{
	SIZE sResult = { -1, -1 };
	/*  Prepare glyph transformation matrix (no rotation)  */
	MAT2 mt;
	memset(&mt, 0, sizeof(mt));
	mt.eM11.value = 1;
	mt.eM22.value = 1;

	GLYPHMETRICS gm;
	DWORD dwSize = GetGlyphOutline(hdc, uSymbol, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mt);
	if (dwSize != GDI_ERROR)
	{
		sResult.cx = gm.gmBlackBoxX;
		sResult.cy = gm.gmBlackBoxY;
	}
	return sResult;
}

PBYTE Glyph_Get(HDC hdc, UINT uSymbol, PDWORD pdwBufferSize)
{
	/*  Prepare glyph transformation matrix (no rotation)  */
	MAT2 mt;
	memset(&mt, 0, sizeof(mt));
	mt.eM11.value = 1;
	mt.eM22.value = 1;

	GLYPHMETRICS gm;
	*pdwBufferSize = GetGlyphOutline(hdc, uSymbol, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mt);
	if (*pdwBufferSize == GDI_ERROR)
	{
		return NULL;
	}
	PBYTE pbResult = (PBYTE)calloc(*pdwBufferSize, sizeof(BYTE));
	DWORD dwGetGlyphOutlineResult = GetGlyphOutline(hdc, uSymbol, GGO_GRAY8_BITMAP, &gm, (*pdwBufferSize), pbResult, &mt);
	if (dwGetGlyphOutlineResult == GDI_ERROR)
	{
		free(pbResult);
		*pdwBufferSize = -1;
		return NULL;
	}
	return pbResult;
}

void Glyph_WritePallete(FILE *pf, DWORD dwPalleteSize)
{
	RGBQUAD rgbqd = { 0 };
	INT iStep = GLYPH_MAX_COLOR_SHADES / (dwPalleteSize - 1);
	INT iShadeOfGray = GLYPH_MAX_COLOR_SHADES - 1;
	for (UINT i = 0; i < dwPalleteSize - 1; ++i)
	{
		rgbqd.rgbRed = iShadeOfGray;
		rgbqd.rgbGreen = iShadeOfGray;
		rgbqd.rgbBlue = iShadeOfGray;
		iShadeOfGray -= iStep;
		fwrite(&rgbqd, sizeof(RGBQUAD), 1, pf);
	}
	rgbqd.rgbRed = 0;
	rgbqd.rgbGreen = 0;
	rgbqd.rgbBlue = 0;
	fwrite(&rgbqd, sizeof(RGBQUAD), 1, pf);
}


void Glyph_Save(BYTE pbData[], DWORD dwDataSize, SIZE sBitmap, PTCHAR szPath)
{
	BITMAPINFOHEADER bi = { 0 };
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = sBitmap.cx;
	bi.biHeight = -sBitmap.cy;
	bi.biPlanes = 1;
	bi.biBitCount = 8;
	bi.biCompression = BI_RGB;
	bi.biClrUsed = GLYPH_PALLETE_SIZE;
	bi.biSizeImage = dwDataSize;
	/*bi.bmiColors->rgbRed = 255;
	bi.bmiColors->rgbGreen = 255;
	bi.bmiColors->rgbBlue = 255;*/

	BITMAPFILEHEADER bf = { 0 };
	bf.bfType = ('M' << 8) | 'B';
	bf.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) 
		+ bi.biSize + bi.biClrUsed
		* sizeof(RGBQUAD) + bi.biSizeImage);

	bf.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		bi.biSize + bi.biClrUsed
		* sizeof(RGBQUAD);
	
	FILE *pf;
	int writeRes = fopen_s(&pf, szPath, "wb");
	writeRes = fwrite(&bf, sizeof(bf), 1, pf);
	writeRes = fwrite(&bi, sizeof(bi), 1, pf);

	Glyph_WritePallete(pf, bi.biClrUsed);
	
	writeRes = fwrite(pbData, dwDataSize, 1, pf);
	fclose(pf);
}

void Glyph_Release(PBYTE* ppbGlyph)
{
	free(*ppbGlyph);
	*ppbGlyph = NULL;
}