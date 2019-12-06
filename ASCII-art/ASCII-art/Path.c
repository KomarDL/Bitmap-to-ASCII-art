#include "Path.h"

PWSTR Path_GetCombined(WCHAR szBeginPath[], WCHAR szEndPath[])
{
	PWSTR szFullPath = calloc(MAX_PATH, sizeof(WCHAR));
	PWSTR szResult = PathCombine(szFullPath, szBeginPath, szEndPath);
	if (szResult != NULL)
	{
		INT iRealPathSize = (_tcslen(szResult) + 1) * sizeof(WCHAR);
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