#include "window.h"
#include <wingdi.h>
#define MAX_LOADSTRING 100

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
	WCHAR szTitle[100];
	Window mainWindow;

	LoadStringW(hInstance, TITLE, szTitle, MAX_LOADSTRING);

	if (!mainWindow.create(szTitle, WS_OVERLAPPEDWINDOW))
		return -1;

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	ShowWindow(mainWindow.window(), nCmdShow);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(mainWindow.window(), hAccel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}