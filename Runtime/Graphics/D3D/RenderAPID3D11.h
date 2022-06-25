#pragma once
#include "../RenderAPIProtocol.h"
#include "../../Platform/Platform.h"
#include "../../Container/Vector.h"
#include "RenderStructsD3D11.h"

namespace Joestar
{
	class RenderAPID3D11 : public RenderAPIProtocol
	{
	public:
		void CreateDevice() override;
		void CreateSwapChain() override;
		void CreateSyncObjects(U32 num = 1) override;
		void CreateCommandPool(GPUResourceHandle handle, GPUQueue queue) override;
		void CreateCommandBuffers(GPUResourceHandle handle, GPUCommandBufferCreateInfo& createInfo) override;
		void CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo) override;
		void CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo) override;
		void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo) override;
		void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo) override;
	private:
		bool      mAppPaused;    // �����Ƿ�����ͣ״̬
		bool      mMinimized;    // �����Ƿ���С��
		bool      mMaximized;    // �����Ƿ����
		bool      mResizing;     // �����Ƿ��ڸı��С��״̬
		UINT      m4xMsaaQuality;// 4X MSAA�����ȼ�

		//  D3D11�豸(��4.2.1)��������(��4.2.4)���������/ģ�建���2D����(��4.2.6)��
		//  ��ȾĿ��(��4.2.5)�����/ģ����ͼ(��4.2.6)�����ӿ�(��4.2.8)��
		ID3D11Device* mDevice;
		ID3D11DeviceContext* md3dImmediateContext;
		SwapChainD3D11* mSwapChain;
		Vector<FrameBufferD3D11*> mFrameBuffers;
		Vector<GPUQueue*> mCommandPools;
		Vector<CommandBufferD3D11*> mCommandBuffers;
		Vector<ImageD3D11*> mImages;
		Vector<ImageViewD3D11*> mImageViews;
		Vector<TextureD3D11*> mTextures;
		//ID3D11Texture2D* mDepthStencilBuffer;
		D3D11_VIEWPORT mScreenViewport;

		//  ����ı�������D3DApp���캯�������õġ����ǣ������������������д��Щֵ��

		//  ���ڱ��⡣D3DApp��Ĭ�ϱ�����"D3D11 Application"��
		//std::wstring mMainWndCaption;

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
