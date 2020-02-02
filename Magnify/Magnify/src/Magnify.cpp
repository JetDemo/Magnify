#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "resource.h"

HWND hParent = nullptr, hBitMap = nullptr, hDeskTop = nullptr;
HDC hMemDC = nullptr;
HBITMAP hBit = nullptr;
HMODULE hModule = nullptr;
const unsigned int MAGNIFY_WIDTH = 240;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    UNREFERENCED_PARAMETER(szCmdLine && hPrevInstance);

    WNDCLASS wc;

    static TCHAR* szAppName = TEXT("Magnify");

    HWND hwnd = NULL;

    MSG msg;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_MAGNIFY));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szAppName;

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, TEXT("Oops..."),
                   szAppName, MB_ICONERROR);

        return 0;
    }

    hwnd = CreateWindow(szAppName,
                        TEXT("Magnify-JetMeta@github"),
                        WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_SIZEBOX,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        295,
                        320,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    ShowWindow(hwnd, iCmdShow);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    POINT point;
    HDC hDeskTopDC, hBitDC;

    switch (message)
    {
    case WM_CREATE:
	{
		RECT r;
		GetClientRect(hwnd,&r);
		const unsigned int x_offset = (r.right - r.left - MAGNIFY_WIDTH)>>1;
		const unsigned int y_offset = (r.bottom - r.top - MAGNIFY_WIDTH) >> 1;
        hBitMap = CreateWindowEx(0, TEXT("STATIC"),
                                 NULL,
                                 SS_CENTER | WS_CHILD | WS_VISIBLE,
								 x_offset, y_offset, MAGNIFY_WIDTH, MAGNIFY_WIDTH,
                                 hwnd,
                                 NULL,
                                 hModule,
                                 NULL);

        hDeskTop = GetDesktopWindow();
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        hDeskTopDC = GetDC(hDeskTop);
        hMemDC = CreateCompatibleDC(hDeskTopDC);
        hBit = CreateCompatibleBitmap(hDeskTopDC, MAGNIFY_WIDTH, MAGNIFY_WIDTH);
        SelectObject(hMemDC, hBit);

        ReleaseDC(hDeskTop, hDeskTopDC);
        SetTimer(hwnd, 1, 80, NULL);

        return 0;
	}
    case WM_CLOSE:
        DeleteDC(hMemDC);
        DeleteObject(hBit);
        DestroyWindow(hwnd);
        PostQuitMessage(0);

        return 0;

    case WM_TIMER:
    {
        if (!IsIconic(hwnd))
        {
            GetCursorPos(&point);
            hDeskTopDC = GetDC(hDeskTop);
            hBitDC = GetDC(hBitMap);
            PatBlt(hMemDC, 0, 0, MAGNIFY_WIDTH, MAGNIFY_WIDTH, BLACKNESS);
            StretchBlt(hMemDC, 0, 0, MAGNIFY_WIDTH, MAGNIFY_WIDTH, hDeskTopDC, point.x - 15, point.y - 15, 30, 30, SRCCOPY);
            BitBlt(hBitDC, 0, 0, MAGNIFY_WIDTH, MAGNIFY_WIDTH, hMemDC, 0, 0, SRCCOPY);
            ReleaseDC(hDeskTop, hDeskTopDC);
            ReleaseDC(hBitMap, hBitDC);
        }
    }

    return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
