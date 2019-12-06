#include "Glyph.h"
#include "Path.h"
#include "FirstStartInitialization.h"

BOOL FirstStart_CreateDataFiles(WCHAR szDataPath[], WCHAR szSymbols[], WCHAR szGlyphDirPath[])
{
	BOOL fResult = CreateDirectoryW(szGlyphDirPath, NULL);
	if (fResult)
	{
		PZPWSTR pszFileNames = Path_GetGlyphsFileNames(szSymbols, GLYPH_FILE_EXTENSION);
		fResult = (pszFileNames != NULL);
		if (fResult)
		{
			//combine with dir path
			//create glyphs
		}
		else
		{
			RemoveDirectoryW(szGlyphDirPath);
		}
		
	}
}