#include <stdlib.h>
#include "Path.h"

#define PATH_MAX_GLYPH_FILE_NAME_LENGTH 33

PWSTR Path_GetCombined(CONST WCHAR szBeginPath[], CONST WCHAR szEndPath[])
{
	PWSTR szFullPath = calloc(MAX_PATH, sizeof(WCHAR));
	PWSTR szResult = PathCombineW(szFullPath, szBeginPath, szEndPath);
	if (szResult != NULL)
	{
		INT iRealPathSize = (wcslen(szResult) + 1) * sizeof(WCHAR);
		szResult = realloc(szResult, iRealPathSize);
	}
	else
	{
		free(szFullPath);
	}
	return szResult;
}

void Path_ReleaseCombined(PWSTR szDirPath)
{
	free(szDirPath);
}

PWSTR Path_GetGlyphFileName(WORD wSymbolCode, CONST WCHAR szExtention[])
{
	SIZE_T stResultSize = MAX_PATH * sizeof(WCHAR);
	PWSTR szResult = malloc(stResultSize);
	errno_t err = _ultow_s(wSymbolCode, szResult, PATH_MAX_GLYPH_FILE_NAME_LENGTH, 10);
	if (err || !PathAddExtensionW(szResult, szExtention))
	{
		free(szResult);
		szResult = NULL;
	}
	return szResult;
}

PZPWSTR Path_GetGlyphsFileNames(CONST WCHAR szSymbols[], CONST WCHAR szExtention[])
{
	SIZE_T stSymbolsLength = wcslen(szSymbols);
	PZPWSTR pszResult = calloc(stSymbolsLength, sizeof(PWSTR));
	if (pszResult != NULL)
	{
		BOOL fError = FALSE;
		for (SIZE_T i = 0; i < stSymbolsLength && !fError; ++i)
		{
			pszResult[i] = Path_GetGlyphFileName(szSymbols[i], szExtention);
			fError = (pszResult[i] == NULL);
		}

		if (fError)
		{
			for (PZPWSTR i = pszResult; *i != NULL; ++i)
			{
				free(*i);
			}
			free(pszResult);
			pszResult = NULL;
		}
	}
	return pszResult;
}

void Path_ReleaseGlyphsFileNames(PZPWSTR pszNames, SIZE_T stLength)
{
	for (SIZE_T i = 0; i < stLength; ++i)
	{
		free(pszNames[i]);
	}
	free(pszNames);
}