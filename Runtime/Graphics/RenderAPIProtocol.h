#pragma once
#include "../Platform/Platform.h"
#include "GPUResource.h"
#include "GPUCreateInfos.h"
#include "../Container/Vector.h"
namespace Joestar
{
	class Window;

	class RenderAPIProtocol
	{
	public:
		//Protocol Interfaces
		virtual void CreateDevice() = 0;
		virtual void CreateSwapChain(GPUSwapChainCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateMainCommandBuffers(U32 num = 1) = 0;
		virtual void CreateCommandBuffers(GPUResourceHandle handle, GPUCommandBufferCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateSyncObjects(U32 num = 1) = 0;
		virtual void CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo) = 0;
		virtual void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo) = 0;
		virtual void CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo) = 0;
		virtual void CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo) = 0;
		virtual void CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo) = 0;
		virtual void CreateRenderPass(GPUResourceHandle handle, GPURenderPassCreateInfo& createInfo) = 0;
		virtual void CreateDescriptorPool(U32 num = 1) = 0;
		virtual void CreateGraphicsPipelineState(GPUResourceHandle handle, GPUGraphicsPipelineStateCreateInfo& createInfo) = 0;
		virtual void CreateComputePipelineState(GPUResourceHandle handle, GPUComputePipelineStateCreateInfo& createInfo) = 0;
		///Uniform Functions
		void CreateMemory(GPUResourceHandle handle, U32 size, U8* data);
		void CreateColorBlendState(GPUResourceHandle handle, GPUColorBlendStateCreateInfo& createInfo);
		void CreateDepthStencilState(GPUResourceHandle handle, GPUDepthStencilStateCreateInfo& createInfo);
		void CreateRasterizationState(GPUResourceHandle handle, GPURasterizationStateCreateInfo& createInfo);
		void CreateMultiSampleState(GPUResourceHandle handle, GPUMultiSampleStateCreateInfo& createInfo);

		void SetWindow(Window* w);
	protected:
		U32 maxMsaaSamples;
		U32 maxBindings{16};
		U32 maxUniformBuffers{ 1024 };
		Window* window;
		GPUResourceHandle gCommandBufferHandle{0};
		GPUResourceHandle gPipelineStateHandle{0};
		Vector<GPUMemory> mMemories;
		Vector<GPUColorBlendStateCreateInfo> mColorBlendStates;
		Vector<GPUDepthStencilStateCreateInfo> mDepthStencilStates;
		Vector<GPURasterizationStateCreateInfo> mRasterizationStates;
		Vector<GPUMultiSampleStateCreateInfo> mMultiSampleStates;
	};
}