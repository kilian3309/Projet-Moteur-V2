#pragma once
#include "DXUT.h"
#include <string>
#include <Uxtheme.h>
#pragma comment(lib, "UxTheme.lib")

/*
	Système de compteur compil-time
*/
int LoadingMax = 13;

RECT loadingTextRect;
HDC loadingDeviceContext;

//Variables de la fenêtre de chargement
HWND hwndPB; //Handle vers la progress bar
HWND hwndLS; //Handle vers la fenêtre de chargement

void MessageBoxK(std::wstring title, std::wstring text) {
	MessageBox(0, text.c_str(), title.c_str(), MB_OK);
}

void MessageBoxK(std::wstring title, int text) {
	MessageBoxK(title, std::to_wstring(text));
}

/*
	
*/
void IncrementLoading(LPCWSTR txt=LPCWSTR(L"Loading...")) {
	SendMessage(hwndPB, PBM_STEPIT, 0, 0);
	if (SendMessage(hwndPB, PBM_GETPOS, 0, 0) == SendMessage(hwndPB, PBM_GETRANGE, 0, 0)) {
		SendMessage(hwndLS, WM_DESTROY, 0, 0);
	}
	//DrawText(loadingDeviceContext, txt, 11, &loadingTextRect, DT_CENTER);
	PAINTSTRUCT ps;
	loadingDeviceContext = BeginPaint(hwndLS, &ps);
	TextOut(loadingDeviceContext, 200, 200, txt, wcslen(txt));
	EndPaint(hwndLS, &ps);
}


LRESULT CALLBACK MsgProcLS(HWND win, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		DestroyWindow(win);

		return 0;
	default:
		return DefWindowProc(win, msg, wParam, lParam);
	}
}

/*
	Creation de la loading screen
*/
void CreateLoadingScreen(HINSTANCE hInst = (HINSTANCE)nullptr, int maxRange = 6) {
	if (!hInst) {
		hInst = (HINSTANCE)GetModuleHandle(nullptr);
	}

	MessageBoxK(L"fd", LoadingMax);

	WNDCLASS WCLoadScreen;
	WCLoadScreen.style = 0;
	WCLoadScreen.lpfnWndProc = MsgProcLS;
	WCLoadScreen.cbClsExtra = NULL;
	WCLoadScreen.cbWndExtra = NULL;
	WCLoadScreen.hInstance = hInst;
	WCLoadScreen.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WCLoadScreen.hCursor = LoadCursor(NULL, IDC_ARROW);
	WCLoadScreen.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
	WCLoadScreen.lpszMenuName = NULL;
	WCLoadScreen.lpszClassName = L"WCClass";

	RegisterClass(&WCLoadScreen);

	RECT rcClient;


	hwndLS = CreateWindow(L"WCCLass", L"Loading Engine...", WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInst, NULL);

	GetWindowRect(hwndLS, &rcClient);

	int width = rcClient.right - rcClient.left;
	int height = rcClient.bottom - rcClient.top;

	//MessageBox(hwndLS, LPWSTR(width), L"", MB_OK);

	hwndPB = CreateWindowEx(0, PROGRESS_CLASS, (LPCTSTR)NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 100, 500, 600, 20, hwndLS, 0, hInst, NULL);
	//MessageBox(hwndLS,  LPWSTR(std::to_wstring((int)rcClient.bottom / 3) + L"" + std::to_wstring((int)rcClient.right / 5)), L"", MB_OK);
	SetWindowTheme(hwndPB, L"", L"");
	SendMessage(hwndPB, (UINT)PBM_SETBARCOLOR, 0, RGB(0, 148, 255));

	
	GetWindowRect(hwndPB, &loadingTextRect);
	loadingDeviceContext = GetDC(hwndPB);

	if (loadingDeviceContext == NULL) {
		MessageBoxK(L"ERROR", L"Impossible d'obtenir le DeviceContext !");
	}

	MessageBoxK(L"h", std::to_wstring(loadingTextRect.bottom) + L"|" + std::to_wstring(loadingTextRect.top) + L"|" + std::to_wstring(loadingTextRect.right) + L"|" + std::to_wstring(loadingTextRect.left));

	/*
	loadingTextRect->top = 100;
	loadingTextRect->bottom = 200;
	loadingTextRect->right = 50;
	loadingTextRect->left = 10;*/




	//hwnPB = CreateWindow(L"WCClass", L"Loading Engine...", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, NULL, NULL, hInst, NULL);
	//hwndPB = CreateWindowEx(0, PROGRESS_CLASS, L"Loading Engine...", WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 300, 50, NULL, (HMENU)0, hInst, NULL);

	SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0, maxRange));
	SendMessage(hwndPB, PBM_SETSTEP, (WPARAM)1, 0);


	
}




