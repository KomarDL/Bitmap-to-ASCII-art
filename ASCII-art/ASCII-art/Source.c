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
#include "Menu.h"
#include "Shortcat.h"

#define IMAGE_EXTENSION L"bmp"

// Global variables

// The main window class name.
WCHAR szWindowClass[] = L"To ASCII-art";

// The string that appears in the application's title bar.
WCHAR szTitle[] = L"To ASCII-art";

WCHAR szHelpMsg[] = L"";

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
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU);
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

	HWND hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX) &~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0,
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

	HACCEL hAccel = LoadAcceleratorsW(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	if (hAccel == NULL)
	{
		MessageBoxW(NULL,
			L"Call to LoadAccelerators failed!",
			L"Windows Desktop Guided Tour",
			MB_ICONERROR);
		return -1;
	}

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
				
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				
			}
		}
	}

	return (INT)msg.wParam;
}

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
		//maximize Window
		SendMessageW(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case ID_MENU_OPEN:
			OPENFILENAMEW ofn = { 0 };
			if (Dialog_Open(hWnd, &ofn))
			{
				fContinue = (wmemcmp(ofn.lpstrFile + ofn.nFileExtension, IMAGE_EXTENSION, sizeof(IMAGE_EXTENSION)) == 0);
				if (fContinue)
				{
					
				}
				
				if (!fContinue)
				{
					MessageBoxW(NULL,
						L"Wrong file type",
						L"Windows Desktop Guided Tour",
						MB_ICONERROR);
				}
			}
			break;
		case ID_MENU_HELP:
			MessageBoxW(hWnd, szHelpMsg, L"Help", MB_ICONINFORMATION);
			break;
		case ID_MENU_EXIT:
			PostQuitMessage(0);
			break;
		}
	case WM_MOVING:
		//disable moving
		if (lParam)
		{
			PRECT lpRect = (LPRECT)lParam;
			GetWindowRect(hWnd, lpRect);
		}
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
