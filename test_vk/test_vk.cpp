#include "stdafx.h"
#include "vk/vk.h"
#include "test_vk.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TEST_VK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST_VK));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST_VK));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TEST_VK);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


INT Run()
{
	BOOL	bGotMsg = FALSE;
	MSG		msg;
	msg.message = WM_NULL;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		bGotMsg = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			// 描画
			vk::render();
		}
	}

	return (INT)msg.wParam;
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	VkResult result = vk::initialize(hInstance, hWnd);

	if (result != VK_SUCCESS)
	{
		std::map<VkResult, std::wstring> error_list =
		{
			std::make_pair(VK_SUCCESS,							L"VK_SUCCESS"),
			std::make_pair(VK_NOT_READY,						L"VK_NOT_READY"),
			std::make_pair(VK_TIMEOUT,							L"VK_TIMEOUT"),
			std::make_pair(VK_EVENT_SET,						L"VK_EVENT_SET"),
			std::make_pair(VK_EVENT_RESET,						L"VK_EVENT_RESET"),
			std::make_pair(VK_INCOMPLETE,						L"VK_INCOMPLETE"),
			std::make_pair(VK_ERROR_OUT_OF_HOST_MEMORY,			L"VK_ERROR_OUT_OF_HOST_MEMORY"),
			std::make_pair(VK_ERROR_OUT_OF_DEVICE_MEMORY,		L"VK_ERROR_OUT_OF_DEVICE_MEMORY"),
			std::make_pair(VK_ERROR_INITIALIZATION_FAILED,		L"VK_ERROR_INITIALIZATION_FAILED"),
			std::make_pair(VK_ERROR_DEVICE_LOST,				L"VK_ERROR_DEVICE_LOST"),
			std::make_pair(VK_ERROR_MEMORY_MAP_FAILED,			L"VK_ERROR_MEMORY_MAP_FAILED"),
			std::make_pair(VK_ERROR_LAYER_NOT_PRESENT,			L"VK_ERROR_LAYER_NOT_PRESENT"),
			std::make_pair(VK_ERROR_EXTENSION_NOT_PRESENT,		L"VK_ERROR_EXTENSION_NOT_PRESENT"),
			std::make_pair(VK_ERROR_FEATURE_NOT_PRESENT,		L"VK_ERROR_FEATURE_NOT_PRESENT"),
			std::make_pair(VK_ERROR_INCOMPATIBLE_DRIVER,		L"VK_ERROR_INCOMPATIBLE_DRIVER"),
			std::make_pair(VK_ERROR_TOO_MANY_OBJECTS,			L"VK_ERROR_TOO_MANY_OBJECTS"),
			std::make_pair(VK_ERROR_FORMAT_NOT_SUPPORTED,		L"VK_ERROR_FORMAT_NOT_SUPPORTED"),
			std::make_pair(VK_ERROR_SURFACE_LOST_KHR,			L"VK_ERROR_SURFACE_LOST_KHR"),
			std::make_pair(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,	L"VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"),
			std::make_pair(VK_SUBOPTIMAL_KHR,					L"VK_SUBOPTIMAL_KHR"),
			std::make_pair(VK_ERROR_OUT_OF_DATE_KHR,			L"VK_ERROR_OUT_OF_DATE_KHR"),
			std::make_pair(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,	L"VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"),
			std::make_pair(VK_ERROR_VALIDATION_FAILED_EXT,		L"VK_ERROR_VALIDATION_FAILED_EXT"),
		};

		if (error_list.find(result) != error_list.end())
		{
			MessageBox(hWnd, TEXT("error"), error_list.at(result).c_str(), MB_OK);
		}
		else
		{
			MessageBox(hWnd, TEXT("error"), TEXT("UnknownError"), MB_OK);
		}
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	Run();

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
