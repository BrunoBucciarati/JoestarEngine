#pragma once
#include <d3d11.h>
#include "RenderThread.h"
namespace Joestar {
	class RenderThreadD3D11 : public RenderThread {
	public:
		RenderThreadD3D11();
		bool InitRenderContext();

	private:
		HINSTANCE mhAppInst;     // Ӧ�ó���ʵ�����
		HWND      mhMainWnd;     // �����ھ��
		bool      mAppPaused;    // �����Ƿ�����ͣ״̬
		bool      mMinimized;    // �����Ƿ���С��
		bool      mMaximized;    // �����Ƿ����
		bool      mResizing;     // �����Ƿ��ڸı��С��״̬
		UINT      m4xMsaaQuality;// 4X MSAA�����ȼ�

		//  D3D11�豸(��4.2.1)��������(��4.2.4)���������/ģ�建���2D����(��4.2.6)��
		//  ��ȾĿ��(��4.2.5)�����/ģ����ͼ(��4.2.6)�����ӿ�(��4.2.8)��
		ID3D11Device* md3dDevice;
		ID3D11DeviceContext* md3dImmediateContext;
		IDXGISwapChain* mSwapChain;
		ID3D11Texture2D* mDepthStencilBuffer;
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11DepthStencilView* mDepthStencilView;
		D3D11_VIEWPORT mScreenViewport;

		//  ����ı�������D3DApp���캯�������õġ����ǣ������������������д��Щֵ��

		//  ���ڱ��⡣D3DApp��Ĭ�ϱ�����"D3D11 Application"��
		std::wstring mMainWndCaption;

		//  Hardware device����reference device��D3DAppĬ��ʹ��D3D_DRIVER_TYPE_HARDWARE��
		D3D_DRIVER_TYPE md3dDriverType;
		//  ����Ϊtrue��ʹ��4XMSAA(��4.1.8)��Ĭ��Ϊfalse��
		bool mEnable4xMsaa;

		int mClientWidth;
		int mClientHeight;
	};
}