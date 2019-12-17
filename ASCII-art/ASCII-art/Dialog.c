#include "Dialog.h"

BOOL Dialog_Open(HWND hWnd, LPOPENFILENAMEW pofn)
{
	PWSTR szPath = calloc(MAX_PATH, sizeof(WCHAR));
	if (szPath == NULL)
	{
		return FALSE;
	}

	ZeroMemory(pofn, sizeof(*pofn));
	pofn->lStructSize = sizeof(*pofn);
	pofn->hwndOwner = hWnd;
	pofn->lpstrFile = szPath;
	pofn->nMaxFile = MAX_PATH;
	pofn->lpstrFilter = L"Bitmap images\0*.bmp\0\0";
	pofn->lpstrTitle = L"Choose bitmap to convert to ASCII-art";
	pofn->Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	return GetOpenFileNameW(pofn);
}