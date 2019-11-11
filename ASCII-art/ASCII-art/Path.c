#include "Path.h"

PTCHAR Path_GetCombined(TCHAR szBeginPath[], TCHAR szEndPath[])
{
	PTCHAR szFullPath = calloc(MAX_PATH, sizeof(TCHAR));
	PTCHAR szResult = PathCombine(szFullPath, szBeginPath, szEndPath);
	if (szResult != NULL)
	{
		INT iRealPathSize = (_tcslen(szResult) + 1) * sizeof(TCHAR);
		szResult = realloc(szResult, iRealPathSize);
	}
	else
	{
		free(szFullPath);
	}
	return szResult;
}

void Path_ReleaseCombined(PTCHAR *pszDirPath)
{
	free(*pszDirPath);
	*pszDirPath = NULL;
}