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
		bool      mAppPaused;    // 程序是否处在暂停状态
		bool      mMinimized;    // 程序是否最小化
		bool      mMaximized;    // 程序是否最大化
		bool      mResizing;     // 程序是否处在改变大小的状态
		UINT      m4xMsaaQuality;// 4X MSAA质量等级

		//  D3D11设备(§4.2.1)，交换链(§4.2.4)，用于深度/模板缓存的2D纹理(§4.2.6)，
		//  渲染目标(§4.2.5)和深度/模板视图(§4.2.6)，和视口(§4.2.8)。
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

		//  下面的变量是在D3DApp构造函数中设置的。但是，你可以在派生类中重写这些值。

		//  窗口标题。D3DApp的默认标题是"D3D11 Application"。
		//std::wstring mMainWndCaption;

		//  Hardware device还是reference device？D3DApp默认使用D3D_DRIVER_TYPE_HARDWARE。
		D3D_DRIVER_TYPE md3dDriverType;
		//  设置为true则使用4XMSAA(§4.1.8)，默认为false。
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
