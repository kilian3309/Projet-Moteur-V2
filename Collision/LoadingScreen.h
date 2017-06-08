
#include "DXUT.h"
#include <Uxtheme.h>
#pragma comment(lib, "UxTheme.lib")

#pragma once
//Variables de la fenêtre de chargement
HWND hwndPB; //Handle vers la progress bar
HWND hwndLS; //Handle vers la fenêtre de chargement

LRESULT CALLBACK MsgProcLS(HWND win, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		//PostQuitMessage(0);
		DestroyWindow(win);

		return 0;
	default:
		return DefWindowProc(win, msg, wParam, lParam);
	}
}

void CreateLoadingScreen(HINSTANCE hInst = (HINSTANCE)nullptr, int maxRange = 6) {
	if (!hInst) {
		hInst = (HINSTANCE)GetModuleHandle(nullptr);
	}


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






	//hwnPB = CreateWindow(L"WCClass", L"Loading Engine...", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, NULL, NULL, hInst, NULL);
	//hwndPB = CreateWindowEx(0, PROGRESS_CLASS, L"Loading Engine...", WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 300, 50, NULL, (HMENU)0, hInst, NULL);

	SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0, maxRange));
	SendMessage(hwndPB, PBM_SETSTEP, (WPARAM)1, 0);

}


void _IncrementLoading() {
	SendMessage(hwndPB, PBM_STEPIT, 0, 0);
	//MessageBoxK(L"hg", to_wstring(SendMessage(hwndPB, PBM_GETRANGE, 0, 0)));
	//MessageBoxK(L"", to_wstring(SendMessage(hwndPB, PBM_GETPOS, 0, 0)) + L"|"+ to_wstring(SendMessage(hwndPB, PBM_GETRANGE, 0, 0)));
	if (SendMessage(hwndPB, PBM_GETPOS, 0, 0) == SendMessage(hwndPB, PBM_GETRANGE, 0, 0)) {
		//MessageBoxK(L"f", L"ff");
		//SendMessage(hwndLS, WM_DESTROY, 0, 0);
	}

}

/*
Ceci marche mais c'est pas ouf... Sa sert à faire en sorte que la barre de chargement ai comme maximum le même nombre de fois qu'on l'incrémente
Ex: Si on l'incrémente 5 fois dans tout le code, son maximum sera fixé à 5 pour qu'elle puisse atteindre 100%
*/
#define __ILIN __COUNTER__
#define IncrementLoading __ILIN; \
						 _IncrementLoading()

