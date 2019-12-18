#pragma once

#include <Windows.h>

PVOID UserBmp_Map(WCHAR szPath[], PHANDLE hFile, PHANDLE hMapping);
void UserBmp_Unmap(PVOID pvData, HANDLE hFile, HANDLE hMapping);


