#pragma once
#include "../Core/Platform.h"
#include "GPUResource.h"
namespace Joestar
{
	class Window;

	class RenderAPIProtocol
	{
	public:
		virtual void CreateDevice() = 0;
		virtual void CreateSwapChain(GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateMainCommandBuffers(U32 num = 1) = 0;
		virtual void CreateCommandBuffers(GPUResourceHandle handle, GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateSyncObjects(GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;

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