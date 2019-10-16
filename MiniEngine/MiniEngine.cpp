#include "pch.h"
#include <iostream>
#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_SIZE:
		// TODO: Recalculate render resolution
		break;
	case WM_KEYDOWN:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMessage, wParam, lParam);
	}

	return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSA windows_class = { 0 };
	windows_class.lpfnWndProc = WindowProc; // Callback function of windows
	windows_class.lpszClassName = "MiniEngine";
	windows_class.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassA(&windows_class);

	HWND hwnd = CreateWindowA(
		windows_class.lpszClassName, // Window name (identical with class name)
		windows_class.lpszClassName, // Class name
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, // Left upper corner in pixels => Windows decides, where to place the window
		CW_USEDEFAULT, CW_USEDEFAULT, // Width and height of the window => Windows decides, how big the window is
		HWND_DESKTOP, // The window is a sub-window of the desktop
		NULL, // Handle of the menu, doesn't exist => NULL
		hInstance,
		0 // No window generating data
	);

	if (!hwnd)
	{
		MessageBoxA(NULL, "Create Window failed!", "MiniEngine", NULL);
		return 1;
	}

	ShowWindow(hwnd, SW_SHOW);

	//////////////////////////////////////////////////////////////////////////
	// Init

	//////////////////////////////////////////////////////////////////////////
	// Tick
	bool bQuit = false;

	do
	{
		MSG msg;
		// In if only one message per tick gets computed,
		// in while all messages are computed
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bQuit = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// TODO: Call Vulkan or DirectX 12 tick
		}
	} while (!bQuit);

	//////////////////////////////////////////////////////////////////////////
	// Fini

	return 0;
}
