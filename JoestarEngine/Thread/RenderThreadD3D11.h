#pragma once
#include <d3d11.h>
#include "RenderThread.h"
namespace Joestar {
	class RenderThreadD3D11 : public RenderThread {
	public:
		RenderThreadD3D11(Vector<GFXCommandBuffer*>& cmdBuffers, Vector<GFXCommandBuffer*>& computeBuffers);
		bool InitRenderContext();
		bool InitWindow();
		void PrepareCompute();

	private:
		void ThreadFunc();
		void DrawScene();
		void DispatchCompute();
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

		ID3D11Buffer* mVB;
		ID3D11Buffer* mIB;
		ID3D11Buffer* mCB;
		ID3D11RasterizerState* mRS;
		ID3D11InputLayout* mInputLayout;
		ID3D11VertexShader* vs;
		ID3D11PixelShader* ps;
		ID3D11Texture2D* mDiffTex;
		ID3D11ShaderResourceView* mDiffSRV;
		ID3D11SamplerState* mSampleState;
		ID3D11BlendState* mBlendState;
		ID3D11DepthStencilState* mDepthStencilState;

		//compute
		ID3D11Buffer* mInputBuffer;
		ID3D11Buffer* mOutputBuffer;
		ID3D11Buffer* mOutputDebugBuffer;
		ID3D11ShaderResourceView* mInputASRV;
		ID3D11UnorderedAccessView* mOutputUAV;
		ID3D11ComputeShader* cs;
	};
}