#include "Window.h"
#include "../Misc/GlobalConfig.h"
#include "../IO/HID.h"

namespace Joestar {
    U32 CurPosX = 0, CurPosY = 0;
    HID* HIDSystem;
    LRESULT CALLBACK WinProc(
        HWND hwnd,      // handle to window
        UINT uMsg,      // message identifier
        WPARAM wParam,  // first message parameter
        LPARAM lParam   // second message parameter
    )
    {
        U32 xPos = 0;   // horizontal position 
        U32 yPos = 0;   // vertical position 
        switch (uMsg)
        {
        //case WM_CHAR:
            //char szChar[20];
            ////sprintf(szChar,"char code is %d",wParam);
            //MessageBox(hwnd, TEXT("char"), TEXT("char"), 0);
            //break;
        case WM_LBUTTONDOWN:
            //MessageBox(hwnd, TEXT("mouse clicked"), TEXT("message"), 0);
            //HDC hdc;
            //hdc = GetDC(hwnd);
            //TextOut(hdc, 0, 50, TEXT("点击"), strlen("点击"));
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
        case WM_KEYDOWN:
            switch (wParam)
            {
            case(0x57):
                HIDSystem->SetKeyboardInputBit(KEY_W);
                break;
            case(0x41):
                HIDSystem->SetKeyboardInputBit(KEY_A);
                break;
            case(0x53):
                HIDSystem->SetKeyboardInputBit(KEY_S);
                break;
            case(0x44):
                HIDSystem->SetKeyboardInputBit(KEY_D);
                break;
            default:
                break;
            }
            break;
        case WM_MOVE:
            xPos = (U32)(short)LOWORD(lParam);   // horizontal position 
            yPos = (U32)(short)HIWORD(lParam);   // vertical position 
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }

	Window::Window(EngineContext* ctx) : Super(ctx)
	{
		InitWindow();
        HIDSystem = GetSubsystem<HID>();
	}

    Window::~Window() = default;

	void Window::InitWindow()
	{
        GlobalConfig* cfg = GetSubsystem<GlobalConfig>();
        U32 width = cfg->GetConfig<U32>(CONFIG_WINDOW_WIDTH);
        U32 height = cfg->GetConfig<U32>(CONFIG_WINDOW_HEIGHT);
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

        mhMainWnd = CreateWindow(TEXT("Joestar"), TEXT("Joestar Engine"), WS_OVERLAPPEDWINDOW, 300, 300,
            width, height, NULL, NULL, mhAppInst, NULL);

	}

    void Window::Show()
    {
        ShowWindow(mhMainWnd, SW_SHOWNORMAL);
        UpdateWindow(mhMainWnd);
        //SetFocus(mhMainWnd);
        //EnableWindow(mhMainWnd, true);
        //SetForegroundWindow(mhMainWnd);
    }

    void Window::Update(F32 dt)
    {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}