#pragma once
#include "../Core/Platform.h"
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
		virtual void CreateCommandBuffers(GPUResourceHandle handle, GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateSyncObjects(U32 num = 1) = 0;
		virtual void CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo) = 0;
		virtual void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo) = 0;
		virtual void CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo) = 0;
		virtual void CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo) = 0;
		virtual void CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo) = 0;
		virtual void CreateMemory(GPUResourceHandle handle, U32 size, U8* data) = 0;

		///Uniform Functions
		void CreateMemory(GPUResourceHandle handle, U32 size, U8* data);
		//virtual void CreateTexture(GPUResourceHandle handle, GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;

		void SetWindow(Window* w);
	protected:
		U32 msaaSamples;
		Window* window;
		GPUResourceHandle gCommandBufferHandle{0};
		GPUResourceHandle gPipelineStateHandle{0};
		Vector<GPUMemory> mMemories;
		//Vector<GPUResourceHandle> mCommandBufferHandles;
		//Vector<GPUResourceHandle> mPipelineStateHandles;
	};
}