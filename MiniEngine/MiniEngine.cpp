#include "pch.h"
#include <iostream>
#include <Windows.h>

#include "CDirectX12Base.h"
#include "VulkanBase.h"

//#define DX12

bool bQuit = false;

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
		bQuit = true;
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
#ifdef DX12
	CDirectX12Base base; // Instance of DirectX12 module
	base.Init(hwnd);
#else
	CVulkanBase base;
	base.Init();
#endif // DX12

	//////////////////////////////////////////////////////////////////////////
	// Tick

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
			SetWindowTextA(hwnd, "MiniEngine");
			base.Tick();
		}
	} while (!bQuit);

	//////////////////////////////////////////////////////////////////////////
	// Fini
	base.Fini();

	return 0;
}
