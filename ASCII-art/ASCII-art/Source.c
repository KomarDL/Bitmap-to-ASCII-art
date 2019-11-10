//#define UNICODE
//#define _UNICODE

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <wingdi.h>
#include "Glyph.h"

// Global variables

// The main window class name.
TCHAR szWindowClass[] = _T("Spreadsheet");

// The string that appears in the application's title bar.
TCHAR szTitle[] = _T("Spreadsheet");

HINSTANCE hInst;

// old font handle
HFONT hfOld = NULL;
HFONT hfNew = NULL;


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
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
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
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
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
				_T("Call to GetMessage failed!"),
				_T("Windows Desktop Guided Tour"),
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
		hdc = GetDC(hWnd);
		
		INT iHeight = -MulDiv(32, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		hfNew = CreateFont(iHeight, iHeight, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET,
			OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			FIXED_PITCH | FF_MODERN, NULL);
		hfOld = SelectObject(hdc, hfNew);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);


		SIZE sBitmap = Glyph_GetSize(hdc, _T('A'));
		DWORD dwGlyphSize;
		PBYTE pbGlyph = Glyph_Get(hdc, _T('A'), &dwGlyphSize);
		Glyph_Save(pbGlyph, dwGlyphSize, sBitmap, "1.bmp");
		Glyph_Release(&pbGlyph);

		RECT rc = { 0 };
		DrawText(hdc, _T("A"), -1, &rc, DT_CALCRECT);
		DrawText(hdc, _T("A"), -1, &rc, 0);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
