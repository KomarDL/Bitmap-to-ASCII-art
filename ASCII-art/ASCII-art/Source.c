//#define UNICODE
//#define _UNICODE

#include <windows.h>
#include <stdlib.h>
#include <wingdi.h>
#include <wchar.h>
#include "Glyph.h"
#include "Path.h"
#include "FirstStartInitialization.h"
#include "Font.h"
#include "Dialog.h"

#define IMAGE_EXTENSION L"bmp"

// Global variables

// The main window class name.
WCHAR szWindowClass[] = L"To ASCII-art";

// The string that appears in the application's title bar.
WCHAR szTitle[] = L"To ASCII-art";

CONST WCHAR szDataPath[] = L"Data.glf";

CONST WCHAR szGlyphDirPath[] = L"Glyphs";

HINSTANCE hInst;

// old font handle
HFONT hfOld = NULL;
HFONT hfNew = NULL;

//all symbols
CONST WCHAR szSymbols[] = L"!`.,:;/\\%&*?@#=";

//Glyphs brightness
PGlBrightness pglGlyphs = NULL;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ INT       nCmdShow
)
{
	WNDCLASSEXW wcex;

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

	if (!RegisterClassExW(&wcex))
	{
		MessageBoxW(NULL,
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
	HWND hWnd = CreateWindowW(
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
		MessageBoxW(NULL,
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

	/*HACCEL hAccel = LoadAcceleratorsW(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	BOOL bRet = 0;
	while (bRet = GetMessageW(&msg, NULL, 0, 0))
	{
		if (-1 == bRet)
		{
			MessageBoxW(NULL,
				L"Call to GetMessage failed!",
				L"Windows Desktop Guided Tour",
				MB_ICONERROR);
			return -1;
		}
		else
		{
			if (!TranslateAcceleratorW(hWnd, hAccel, &msg))
			{
				if (!IsDialogMessageW(hWnd, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}
			}
		}
	}*/

	while ((bRet = GetMessageW(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			MessageBoxW(NULL,
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
		hdc = GetDC(hWnd);

		Font_Initialize(hdc);

		BOOL fContinue = TRUE;
		if (!PathFileExistsW(szDataPath))
		{
			if (!FirstStart_CreateDataFiles(hdc, szDataPath, szSymbols, szGlyphDirPath))
			{
				MessageBoxW(NULL,
					L"Can't create data",
					L"Windows Desktop Guided Tour",
					MB_ICONERROR);
				SendMessageW(hWnd, WM_DESTROY, 0, 0);
				return 0;
			}
		}
		ReleaseDC(hWnd, hdc);

		//loading brightness for each glyph
		pglGlyphs = Glyph_LoadAllBrightness(szDataPath, wcslen(szSymbols));

		if (pglGlyphs == NULL)
		{
			MessageBoxW(NULL,
				L"Can't read .glf file",
				L"Windows Desktop Guided Tour",
				MB_ICONERROR);
			SendMessageW(hWnd, WM_DESTROY, 0, 0);
			return 0;
		}

		OPENFILENAMEW ofn = { 0 };
		if (Dialog_Open(hWnd, &ofn))
		{
			if (wmemcmp(ofn.lpstrFile + ofn.nFileExtension, IMAGE_EXTENSION, sizeof(IMAGE_EXTENSION)) == 0)
			{

			}
			else
			{
				MessageBoxW(NULL,
					L"Wrong file type",
					L"Windows Desktop Guided Tour",
					MB_ICONERROR);
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
