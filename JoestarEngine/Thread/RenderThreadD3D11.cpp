#include "RenderThreadD3D11.h"
#include "../Misc/Application.h"
#include "../Misc/GlobalConfig.h"

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

namespace Joestar {
    LRESULT CALLBACK WinSunProc(
        HWND hwnd,      // handle to window
        UINT uMsg,      // message identifier
        WPARAM wParam,  // first message parameter
        LPARAM lParam   // second message parameter
    )
    {
        switch(uMsg)
        {
        case WM_CHAR:
            char szChar[20];
            //sprintf(szChar,"char code is %d",wParam);
            MessageBox(hwnd, TEXT("char"),TEXT("char"),0);
            break;
        case WM_LBUTTONDOWN:
            MessageBox(hwnd, TEXT("mouse clicked"), TEXT("message"),0);
            HDC hdc;
            hdc=GetDC(hwnd);
            TextOut(hdc,0,50,TEXT("点击"),strlen("点击"));
            //ReleaseDC(hwnd,hdc);
            break;
        case WM_PAINT:
            HDC hDC;
            PAINTSTRUCT ps;
            hDC=BeginPaint(hwnd,&ps);
            //TextOut(hDC,0,0,"",strlen("http://www.sunxin.org"));
            EndPaint(hwnd,&ps);
            break;
        case WM_CLOSE:
            if(IDYES==MessageBox(hwnd, TEXT("是否真的结束？"), TEXT("message"),MB_YESNO))
            {
               DestroyWindow(hwnd);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd,uMsg,wParam,lParam);
    }
    return 0;
}
	RenderThreadD3D11::RenderThreadD3D11()
	{
		InitRenderContext();
	}

	bool RenderThreadD3D11::InitRenderContext()
	{
        Application* app = Application::GetApplication();
        GlobalConfig* cfg = app->GetEngineContext()->GetSubSystem<GlobalConfig>();
        uint32_t width = cfg->GetConfig<uint32_t>(CONFIG_WINDOW_WIDTH);
        uint32_t height = cfg->GetConfig<uint32_t>(CONFIG_WINDOW_HEIGHT);
        mClientWidth = width;
        mClientHeight = height;

        WNDCLASS wndcls;
        wndcls.cbClsExtra = 0;
        wndcls.cbWndExtra = 0;
        wndcls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndcls.hCursor = LoadCursor(NULL, IDC_CROSS);
        wndcls.hIcon = LoadIcon(NULL, IDI_ERROR);
        mhAppInst = GetModuleHandle(NULL);
        wndcls.hInstance = mhAppInst;
        wndcls.lpfnWndProc = WinSunProc;
        wndcls.lpszClassName = TEXT("Joestar");
        wndcls.lpszMenuName = NULL;
        wndcls.style = CS_HREDRAW | CS_VREDRAW;
        RegisterClass(&wndcls);

        mhMainWnd = CreateWindow(TEXT("Joestar"), TEXT("Joestar Engine"), WS_OVERLAPPEDWINDOW, 0, 0, 
            width, height, NULL, NULL, mhAppInst, NULL);

        ShowWindow(mhMainWnd, 1);

        UINT createDeviceFlags = 0;

#if  defined(DEBUG)||defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevel;
        ID3D11Device* md3dDevice;
        ID3D11DeviceContext* md3dImmediateContext;
        HRESULT hr = D3D11CreateDevice(
            0,                     //  默认显示适配器
            D3D_DRIVER_TYPE_HARDWARE,
            0,                     //  不使用软件设备
            createDeviceFlags,
            0, 0,               //  默认的特征等级数组
            D3D11_SDK_VERSION,
            &md3dDevice,
            &featureLevel,
            &md3dImmediateContext);
        if (FAILED(hr))
        {
            MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
            return  false;
        }
        if (featureLevel != D3D_FEATURE_LEVEL_11_0)
        {
            MessageBox(0, L"Direct3D FeatureLevel 11 unsupported.", 0, 0);
            return  false;
        }

        UINT  m4xMsaaQuality;
        hr = md3dDevice->CheckMultisampleQualityLevels(
            DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
        bool mEnable4xMsaa = m4xMsaaQuality > 0;

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = 800;    // 使用窗口客户区宽度
        sd.BufferDesc.Height = 600;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        // 是否使用4X MSAA?
        if (mEnable4xMsaa)
        {
            sd.SampleDesc.Count = 4;
            // m4xMsaaQuality是通过CheckMultisampleQualityLevels()方法获得的
            sd.SampleDesc.Quality = m4xMsaaQuality - 1;
        }
        // NoMSAA
        else
        {
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
        }
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = mhMainWnd;
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;

        IDXGIDevice* dxgiDevice = 0;
        hr = md3dDevice->QueryInterface(__uuidof(IDXGIDevice),
            (void**)&dxgiDevice);
        IDXGIAdapter* dxgiAdapter = 0;
        hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
            (void**)&dxgiAdapter);
        // 获得IDXGIFactory 接口
        IDXGIFactory* dxgiFactory = 0;
        hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory),
            (void**)&dxgiFactory);
        // 现在，创建交换链
        IDXGISwapChain* mSwapChain;
        hr = dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain);
        // 释放COM接口
        ReleaseCOM(dxgiDevice);
        ReleaseCOM(dxgiAdapter);
        ReleaseCOM(dxgiFactory);

        ID3D11RenderTargetView* mRenderTargetView;
        ID3D11Texture2D* backBuffer;
        mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&backBuffer));
        md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView);
        ReleaseCOM(backBuffer);

        D3D11_TEXTURE2D_DESC depthStencilDesc;
        depthStencilDesc.Width = 800;
        depthStencilDesc.Height = 600;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        // 使用4X MSAA?——必须与交换链的MSAA的值匹配
        if (mEnable4xMsaa)
        {
            depthStencilDesc.SampleDesc.Count = 4;
            depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
        }
        //  不使用MSAA
        else
        {
            depthStencilDesc.SampleDesc.Count = 1;
            depthStencilDesc.SampleDesc.Quality = 0;
        }
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;
        ID3D11Texture2D* mDepthStencilBuffer;
        ID3D11DepthStencilView* mDepthStencilView;

        hr = md3dDevice->CreateTexture2D(
            &depthStencilDesc, 0, &mDepthStencilBuffer);

        hr = md3dDevice->CreateDepthStencilView(
            mDepthStencilBuffer, 0, &mDepthStencilView);

        md3dImmediateContext->OMSetRenderTargets(
            1, &mRenderTargetView, mDepthStencilView);

        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        vp.Width = static_cast<float>(800);
        vp.Height = static_cast<float>(600);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        md3dImmediateContext->RSSetViewports(1, &vp);
	}
}