#pragma once
#include "../Core/Platform.h"
namespace Joestar
{
	class Window;

	class RenderAPIProtocol
	{
	public:
		virtual void CreateDevice() = 0;
		virtual void CreateSwapChain() = 0;
		virtual void CreateCommandBuffers() = 0;
		virtual void CreateSyncObjects() = 0;

		void SetWindow(Window* w);
	protected:
		U32 msaaSamples;
		Window* window;
	};
}