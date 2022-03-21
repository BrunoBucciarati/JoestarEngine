#include "Window.h"
//#include <WinUser.h>
//#include <wingdi.h>
#include "../Misc/GlobalConfig.h"

namespace Joestar {
    LRESULT CALLBACK WinProc(
        HWND hwnd,      // handle to window
        UINT uMsg,      // message identifier
        WPARAM wParam,  // first message parameter
        LPARAM lParam   // second message parameter
    )
    {
        switch (uMsg)
        {
        case WM_CHAR:
            char szChar[20];
            //sprintf(szChar,"char code is %d",wParam);
            MessageBox(hwnd, TEXT("char"), TEXT("char"), 0);
            break;
        case WM_LBUTTONDOWN:
            MessageBox(hwnd, TEXT("mouse clicked"), TEXT("message"), 0);
            HDC hdc;
            hdc = GetDC(hwnd);
            TextOut(hdc, 0, 50, TEXT("点击"), strlen("点击"));
            //ReleaseDC(hwnd,hdc);
            break;
        case WM_PAINT:
            HDC hDC;
            PAINTSTRUCT ps;
            hDC = BeginPaint(hwnd, &ps);
            //TextOut(hDC,0,0,"",strlen("http://www.sunxin.org"));
            EndPaint(hwnd, &ps);
            break;
        case WM_CLOSE:
            if (IDYES == MessageBox(hwnd, TEXT("是否真的结束?"), TEXT("message"), MB_YESNO))
            {
                DestroyWindow(hwnd);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }

	Window::Window(EngineContext* ctx) : Super(ctx)
	{
		InitWindow();
	}

    Window::~Window() = default;

	void Window::InitWindow()
	{
        GlobalConfig* cfg = GetSubsystem<GlobalConfig>();
        uint32_t width = cfg->GetConfig<uint32_t>(CONFIG_WINDOW_WIDTH);
        uint32_t height = cfg->GetConfig<uint32_t>(CONFIG_WINDOW_HEIGHT);
        mWidth = width;
        mHeight = height;

        WNDCLASS wndcls;
        wndcls.cbClsExtra = 0;
        wndcls.cbWndExtra = 0;
        wndcls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndcls.hCursor = LoadCursor(NULL, IDC_CROSS);
        wndcls.hIcon = LoadIcon(NULL, IDI_ERROR);
        mhAppInst = GetModuleHandle(NULL);
        wndcls.hInstance = mhAppInst;
        wndcls.lpfnWndProc = WinProc;
        wndcls.lpszClassName = TEXT("Joestar");
        wndcls.lpszMenuName = NULL;
        wndcls.style = CS_HREDRAW | CS_VREDRAW;
        RegisterClass(&wndcls);

        mhMainWnd = CreateWindow(TEXT("Joestar"), TEXT("Joestar Engine"), WS_OVERLAPPEDWINDOW, 0, 0,
            width, height, NULL, NULL, mhAppInst, NULL);

	}

    void Window::Show()
    {
        ShowWindow(mhMainWnd, 1);
    }
}