#include "stdafx.h"
#include "Cardinal.h"
#include "Jotunheimr.h"
#include "Misc.h"
#include "Constants.h"

#define WS_DEFAULT ((WS_OVERLAPPEDWINDOW | WS_SYSMENU) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME))

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "Main";
	//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
		return 1;

	RECT rect = { 0, 0, C::WINDOW_WIDTH, C::WINDOW_HEIGHT };
	AdjustWindowRect(&rect, WS_DEFAULT, false);

	HWND hwnd = CreateWindow("Main", "Main", WS_DEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	if (!hwnd)
		return 1;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg = { 0 };

	if (Cardinal::Initialize(hwnd))
	{
		while (GetMessage(&msg, NULL, 0, 0))
		{
			//TranslateMessage(&msg); //Translates VK to WM_CHAR 
			DispatchMessageA(&msg); //Sends message to Window Proc
		}
		Cardinal::Uninitialize();
	}
	UnregisterClass("Main", hInstance);

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		Cardinal::RegisterInputDevices();
		break;
	case WM_INPUT:
		Cardinal::BufferInput((HRAWINPUT)lParam);
		break;
	case WM_KILLFOCUS:
		Cardinal::pInput->CLR = true;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}