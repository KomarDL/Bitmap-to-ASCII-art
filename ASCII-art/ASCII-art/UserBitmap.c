
#include "UserBitmap.h"

PVOID UserBmp_Map(WCHAR szPath[], PHANDLE hFile, PHANDLE hMapping)
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

	UnmapViewOfFile(pvResult);
	CloseHandle(*hMapping);
	CloseHandle(*hFile);

	return pvResult;
}

void UserBmp_Unmap(PVOID pvData, HANDLE hFile, HANDLE hMapping)
{
	UnmapViewOfFile(pvData);
	CloseHandle(hMapping);
	CloseHandle(hFile);
}