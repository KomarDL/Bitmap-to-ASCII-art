#include "Glyph.h"

//SIZE Glyph_GetSize(HDC hdcMem, UINT uSymbol)
//{
//	SIZE sResult = { -1, -1 };
//	/*  Prepare glyph transformation matrix (no rotation)  */
//	MAT2 mt;
//	memset(&mt, 0, sizeof(mt));
//	mt.eM11.value = 1;
//	mt.eM22.value = 1;
//
//	GLYPHMETRICS gm;
//	DWORD dwSize = GetGlyphOutlineW(hdcMem, uSymbol, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mt);
//	if (dwSize != GDI_ERROR)
//	{
//		sResult.cx = gm.gmBlackBoxX;
//		sResult.cy = gm.gmBlackBoxY;
//	}
//	return sResult;
//}
//
//PBYTE Glyph_Get(HDC hdcMem, UINT uSymbol, PDWORD pdwBufferSize)
//{
//	/*  Prepare glyph transformation matrix (no rotation)  */
//	MAT2 mt;
//	memset(&mt, 0, sizeof(mt));
//	mt.eM11.value = 1;
//	mt.eM22.value = 1;
//
//	GLYPHMETRICS gm;
//	*pdwBufferSize = GetGlyphOutlineW(hdcMem, uSymbol, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mt);
//	if (*pdwBufferSize == GDI_ERROR)
//	{
//		return NULL;
//	}
//	PBYTE pbResult = (PBYTE)calloc(*pdwBufferSize, sizeof(BYTE));
//	UINT uiOtmBuffSize = GetOutlineTextMetricsW(hdcMem, 0, NULL);
//	POUTLINETEXTMETRICW potm = malloc(uiOtmBuffSize);
//	GetOutlineTextMetricsW(hdcMem, uiOtmBuffSize, potm);
//	gm.gmCellIncX = gm.gmCellIncY = potm->otmEMSquare;
//	DWORD dwGetGlyphOutlineResult = GetGlyphOutlineW(hdcMem, uSymbol, GGO_GRAY8_BITMAP, &gm, (*pdwBufferSize), pbResult, &mt);
//	if (dwGetGlyphOutlineResult == GDI_ERROR)
//	{
//		free(pbResult);
//		*pdwBufferSize = -1;
//		return NULL;
//	}
//	return pbResult;
//}
//
//void Glyph_WritePallete(FILE *pf, DWORD dwPalleteSize)
//{
//	RGBQUAD rgbqd = { 0 };
//	INT iStep = GLYPH_MAX_COLOR_SHADES / (dwPalleteSize - 1);
//	INT iShadeOfGray = GLYPH_MAX_COLOR_SHADES - 1;
//	for (UINT i = 0; i < dwPalleteSize - 1; ++i)
//	{
//		rgbqd.rgbRed = iShadeOfGray;
//		rgbqd.rgbGreen = iShadeOfGray;
//		rgbqd.rgbBlue = iShadeOfGray;
//		iShadeOfGray -= iStep;
//		fwrite(&rgbqd, sizeof(RGBQUAD), 1, pf);
//	}
//	rgbqd.rgbRed = 0;
//	rgbqd.rgbGreen = 0;
//	rgbqd.rgbBlue = 0;
//	fwrite(&rgbqd, sizeof(RGBQUAD), 1, pf);
//}
//
//
//void Glyph_Save(BYTE pbData[], DWORD dwDataSize, SIZE sBitmap, CONST WCHAR szPath[])
//{
//	BITMAPINFOHEADER bi = { 0 };
//	bi.biSize = sizeof(BITMAPINFOHEADER);
//	bi.biWidth = sBitmap.cx;
//	bi.biHeight = -sBitmap.cy;
//	bi.biPlanes = 1;
//	bi.biBitCount = 8;
//	bi.biCompression = BI_RGB;
//	bi.biClrUsed = GLYPH_PALLETE_SIZE;
//	bi.biSizeImage = dwDataSize;
//	/*bi.bmiColors->rgbRed = 255;
//	bi.bmiColors->rgbGreen = 255;
//	bi.bmiColors->rgbBlue = 255;*/
//
//	BITMAPFILEHEADER bf = { 0 };
//	bf.bfType = ('M' << 8) | 'B';
//	bf.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) 
//		+ bi.biSize + bi.biClrUsed
//		* sizeof(RGBQUAD) + bi.biSizeImage);
//
//	bf.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
//		bi.biSize + bi.biClrUsed
//		* sizeof(RGBQUAD);
//	
//	FILE *pf;
//	int writeRes = _wfopen_s(&pf, szPath, L"wb");
//	writeRes = fwrite(&bf, sizeof(bf), 1, pf);
//	writeRes = fwrite(&bi, sizeof(bi), 1, pf);
//
//	Glyph_WritePallete(pf, bi.biClrUsed);
//	
//	writeRes = fwrite(pbData, dwDataSize, 1, pf);
//	fclose(pf);
//}
//
//void Glyph_Release(PBYTE pbGlyph)
//{
//	free(pbGlyph);}



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
	bmfHeader.bfType = 0x4D42; //BM   

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