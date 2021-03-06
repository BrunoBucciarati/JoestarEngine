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
		void CreateSampler(GPUResourceHandle handle, GPUSamplerCreateInfo& createInfo) override;
		void CreateTexture(GPUResourceHandle handle, GPUTextureCreateInfo& createInfo) override;
		void CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo) override;
		void CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo) override;
		void CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo) override;
		void SetUniformBuffer(GPUResourceHandle handle, U8* data, U32 size) override;
		void CreateRenderPass(GPUResourceHandle handle, GPURenderPassCreateInfo& createInfo) override;
		void CreateDescriptorPool(U32 num = 1) override;
		void CreateShader(GPUResourceHandle handle, GPUShaderCreateInfo& createInfo) override;
		void CreateGraphicsPipelineState(GPUResourceHandle handle, GPUGraphicsPipelineStateCreateInfo& createInfo) override;
		void CreateComputePipelineState(GPUResourceHandle handle, GPUComputePipelineStateCreateInfo& createInfo) override;
		void QueueSubmit(GPUResourceHandle handle) override;
		void Present() override;
		//virtual void CreateDescriptorSetLayout(GPUResourceHandle handle, PODVector<GPUDescriptorSetLayoutBinding>& bindings) {};
		//virtual void CreateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsCreateInfo& createInfo) = 0;
		//virtual void UpdateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsUpdateInfo& updateInfo) = 0;

		//override for D3D
		void CreateColorBlendState(GPUResourceHandle handle, GPUColorBlendStateCreateInfo& createInfo) override;
		void CreateDepthStencilState(GPUResourceHandle handle, GPUDepthStencilStateCreateInfo& createInfo) override;
		void CreateRasterizationState(GPUResourceHandle handle, GPURasterizationStateCreateInfo& createInfo) override;

		//command buffer APIs
		void CBBegin(GPUResourceHandle handle) {};
		void CBEnd(GPUResourceHandle handle) {};
		void CBBeginRenderPass(GPUResourceHandle handle, RenderPassBeginInfo&);
		void CBEndRenderPass(GPUResourceHandle handle, GPUResourceHandle);
		void CBBindGraphicsPipeline(GPUResourceHandle handle, GPUResourceHandle);
		void CBBindComputePipeline(GPUResourceHandle handle, GPUResourceHandle);
		void CBBindIndexBuffer(GPUResourceHandle handle, GPUResourceHandle);
		void CBBindVertexBuffer(GPUResourceHandle handle, GPUResourceHandle, U32 = 0);
		void CBBindDescriptorSets(GPUResourceHandle handle, GPUResourceHandle, GPUResourceHandle, U32 = 0);
		void CBDraw(GPUResourceHandle handle, U32 count) override;
		void CBDrawIndexed(GPUResourceHandle handle, U32 count, U32 indexStart = 0, U32 vertStart = 0) override;
		void CBSetViewport(GPUResourceHandle handle, const Viewport& vp) override;
		//not for D3D11
		void CBPushConstants(GPUResourceHandle handle, GPUResourceHandle) override {}
		void CBCopyBuffer(GPUResourceHandle handle, CopyBufferType type, GPUResourceHandle) override {};
		void CBCopyBufferToImage(GPUResourceHandle handle, GPUResourceHandle, ImageLayout) override {};
		void CBTransitionImageLayout(GPUResourceHandle handle, GPUResourceHandle, ImageLayout, ImageLayout, U32) override {};
		void CBSubmit(GPUResourceHandle handle) override {};

		DepthStencilStateD3D11* GetDepthStencilState(GPUResourceHandle handle)
		{
			return mDepthStencilStates[handle];
		}

		ColorBlendStateD3D11* GetColorBlendState(GPUResourceHandle handle)
		{
			return mColorBlendStates[handle];
		}

		RasterizationStateD3D11* GetRasterizationState(GPUResourceHandle handle)
		{
			return mRasterizationStates[handle];
		}

		VertexBufferD3D11* GetVertexBuffer(GPUResourceHandle handle)
		{
			return mVertexBuffers[handle];
		}

		ShaderD3D11* GetShader(GPUResourceHandle handle)
		{
			return mShaders[handle];
		}

		RenderPassD3D11* GetRenderPass(GPUResourceHandle handle)
		{
			return mRenderPasses[handle];
		}

		FrameBufferD3D11* GetFrameBuffer(GPUResourceHandle handle)
		{
			return mFrameBuffers[handle];
		}

		TextureD3D11* GetTexture(GPUResourceHandle handle)
		{
			return mTextures[handle];
		}

		UniformBufferD3D11* GetUniformBuffer(GPUResourceHandle handle)
		{
			return mUniformBuffers[handle];
		}
	private:
		bool      mAppPaused;    // 程序是否处在暂停状态
		bool      mMinimized;    // 程序是否最小化
		bool      mMaximized;    // 程序是否最大化
		bool      mResizing;     // 程序是否处在改变大小的状态
		UINT      m4xMsaaQuality;// 4X MSAA质量等级

		//  D3D11设备(§4.2.1)，交换链(§4.2.4)，用于深度/模板缓存的2D纹理(§4.2.6)，
		//  渲染目标(§4.2.5)和深度/模板视图(§4.2.6)，和视口(§4.2.8)。
		ID3D11Device* mDevice;
		ID3D11DeviceContext* mImmediateContext;
		SwapChainD3D11* mSwapChain;
		Vector<FrameBufferD3D11*> mFrameBuffers;
		Vector<GPUQueue*> mCommandPools;
		Vector<CommandBufferD3D11*> mCommandBuffers;
		Vector<ImageD3D11*> mImages;
		Vector<ImageViewD3D11*> mImageViews;
		Vector<SamplerD3D11*> mSamplers;
		Vector<TextureD3D11*> mTextures;
		Vector<IndexBufferD3D11*> mIndexBuffers;
		Vector<VertexBufferD3D11*> mVertexBuffers;
		Vector<UniformBufferD3D11*> mUniformBuffers;
		Vector<RenderPassD3D11*> mRenderPasses;
		Vector<ColorBlendStateD3D11*> mColorBlendStates;
		Vector<DepthStencilStateD3D11*> mDepthStencilStates;
		Vector<RasterizationStateD3D11*> mRasterizationStates;
		Vector<GraphicsPipelineStateD3D11*> mGraphicsPipelineStates;
		Vector<ComputePipelineStateD3D11*> mComputePipelineStates;
		Vector<ShaderD3D11*> mShaders;
		D3D11_VIEWPORT mScreenViewport;

		//  下面的变量是在D3DApp构造函数中设置的。但是，你可以在派生类中重写这些值。

		//  窗口标题。D3DApp的默认标题是"D3D11 Application"。
		//std::wstring mMainWndCaption;

		//  Hardware device还是reference device？D3DApp默认使用D3D_DRIVER_TYPE_HARDWARE。
		D3D_DRIVER_TYPE md3dDriverType;
		//  设置为true则使用4XMSAA(§4.1.8)，默认为false。
		bool mEnable4xMsaa;
	};
}
