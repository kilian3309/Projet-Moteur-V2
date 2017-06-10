#pragma once
#include "DXUT.h"
#include <string>
#include <Uxtheme.h>
#pragma comment(lib, "UxTheme.lib")

/*
	Système de compteur compil-time
*/
#define COUNTER_READ_CRUMB(TAG, RANK, ACC) counter_crumb(TAG(), constant_index<RANK>(), constant_index<ACC>())
//Lecture du compteur
#define COUNTER_READ( TAG ) COUNTER_READ_CRUMB(TAG, 1, COUNTER_READ_CRUMB(TAG, 2, COUNTER_READ_CRUMB(TAG, 4, COUNTER_READ_CRUMB(TAG, 8, \
		COUNTER_READ_CRUMB( TAG, 16, COUNTER_READ_CRUMB(TAG, 32, COUNTER_READ_CRUMB(TAG, 64, COUNTER_READ_CRUMB( TAG, 128, 0))))))))

//Incrémentation du compteur
#define COUNTER_INC(TAG) \
constexpr \
constant_index<COUNTER_READ(TAG)+1> \
counter_crumb(TAG, constant_index<(COUNTER_READ(TAG)+1)&~COUNTER_READ(TAG)>, constant_index<(COUNTER_READ(TAG)+1)&COUNTER_READ(TAG)>) {return {};}
#define COUNTER_LINK_NAMESPACE(NS) using NS::counter_crumb;


template<std::size_t n>
struct constant_index : std::integral_constant< std::size_t, n > {};

template<typename id, std::size_t rank, std::size_t acc>
constexpr constant_index<acc> counter_crumb(id, constant_index<rank>, constant_index<acc>) { return {}; } 


//Variables de la fenêtre de chargement
HWND hwndPB; //Handle vers la progress bar
HWND hwndLS; //Handle vers la fenêtre de chargement

void MessageBoxK(std::wstring title, std::wstring text) {
	MessageBox(0, text.c_str(), title.c_str(), MB_OK);
}

void MessageBoxK(std::wstring title, int text) {
	MessageBoxK(title, std::to_wstring(text));
}

int _IncrCount;

#define IncrementLoading _IncrementLoading()

/*
	
*/
void _IncrementLoading() {
	static int _Count = 0;
	++_Count;
	_IncrCount = _Count;
	SendMessage(hwndPB, PBM_STEPIT, 0, 0);
	if (SendMessage(hwndPB, PBM_GETPOS, 0, 0) == SendMessage(hwndPB, PBM_GETRANGE, 0, 0)) {
		SendMessage(hwndLS, WM_DESTROY, 0, 0);
	}
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

	MessageBoxK(L"fd", _IncrCount);

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




