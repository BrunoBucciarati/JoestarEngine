#pragma once
#include "../Core/Platform.h"
#include "GPUResource.h"
#include "GPUCreateInfos.h"
namespace Joestar
{
	class Window;

	class RenderAPIProtocol
	{
	public:
		virtual void CreateDevice() = 0;
		virtual void CreateSwapChain(GPUSwapChainCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateMainCommandBuffers(U32 num = 1) = 0;
		virtual void CreateCommandBuffers(GPUResourceHandle handle, GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateSyncObjects(U32 num = 1) = 0;
		virtual void CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo) = 0;
		virtual void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo) = 0;
		//virtual void CreateTexture(GPUResourceHandle handle, GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;

		void SetWindow(Window* w);
	protected:
		U32 msaaSamples;
		Window* window;
		GPUResourceHandle gCommandBufferHandle{0};
		GPUResourceHandle gPipelineStateHandle{0};
		//Vector<GPUResourceHandle> mCommandBufferHandles;
		//Vector<GPUResourceHandle> mPipelineStateHandles;
	};
}