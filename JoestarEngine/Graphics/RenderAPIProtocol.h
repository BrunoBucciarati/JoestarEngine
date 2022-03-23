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
		virtual GPUResourceHandle CreateSwapChain(GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;
		virtual GPUResourceHandle CreateMainCommandBuffers(U32 num = 1) = 0;
		virtual GPUResourceHandle CreateCommandBuffers(GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;
		virtual GPUResourceHandle CreateSyncObjects(GPUResourceCreateInfo& createInfo, U32 num = 1) = 0;

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