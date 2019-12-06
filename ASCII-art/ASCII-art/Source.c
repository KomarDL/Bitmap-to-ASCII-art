#define UNICODE
#define _UNICODE

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <wingdi.h>
#include "Glyph.h"
#include "Path.h"

// Global variables

// The main window class name.
WCHAR szWindowClass[] = L"Spreadsheet";

// The string that appears in the application's title bar.
WCHAR szTitle[] = L"Spreadsheet";

CONST WCHAR szDataPath[] = L"Data.glf";

HINSTANCE hInst;

// old font handle
HFONT hfOld = NULL;
HFONT hfNew = NULL;

WCHAR szGlyphs[] = L"`.,:;/\\%&*?@#=";


// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ INT       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour",
			MB_ICONERROR);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 500,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			L"Call to CreateWindow failed!",
			L"Windows Desktop Guided Tour",
			MB_ICONERROR);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	MSG msg;
	BOOL bRet;

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			MessageBox(NULL,
				L"Call to GetMessage failed!",
				L"Windows Desktop Guided Tour",
				MB_ICONERROR);
			return -1;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (INT)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		BOOL fContinue = TRUE;
		if (!PathFileExists(szDataPath))
		{
			
		}
		//PWSTR szDirPath = Path_GetCombined(PATH_CURRENT_DIRECTORY, GLYPH_DIRECTORY_NAME);
		//if (szDirPath != NULL)
		//{
		//	if (!PathFileExists(szDirPath))
		//	{
		//		fContinue = CreateDirectory(szDirPath, NULL);
		//		if (fContinue)
		//		{
		//			//Glyph_StrToGlyphs(szGlyphs, szDirPath);
		//		}
		//	}
		//	else
		//	{
		//		fContinue = PathIsDirectory(szDirPath);
		//		if (fContinue)
		//		{
		//			if (PathIsDirectoryEmpty(szDirPath))
		//			{
		//				//Glyph_StrToGlyphs(szGlyphs, szDirPath);
		//			}
		//		}
		//		else
		//		{
		//			//msg for user
		//		}
		//	}
		//}
		//Path_ReleaseCombined(&szDirPath);
		//if (!fContinue)
		//{
		//	PostQuitMessage(0);
		//}
		//else
		//{
		//	hdc = GetDC(hWnd);

		//	INT iHeight = -MulDiv(256, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		//	hfNew = CreateFont(iHeight, iHeight, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET,
		//		OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		//		FIXED_PITCH | FF_MODERN, NULL);
		//	hfOld = SelectObject(hdc, hfNew);
		//	ReleaseDC(hWnd, hdc);
		//}

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);


		SIZE sBitmap = Glyph_GetSize(hdc, L'A');
		DWORD dwGlyphSize;
		PBYTE pbGlyph = Glyph_Get(hdc, L'A', &dwGlyphSize);
		Glyph_Save(pbGlyph, dwGlyphSize, sBitmap, L"1.bmp");
		Glyph_Release(&pbGlyph);
		
		RECT rc = { 0 };
		DrawText(hdc, L"A", -1, &rc, DT_CALCRECT);
		DrawText(hdc, L"A", -1, &rc, 0);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
