#pragma once
#include "FrameBuffer.h"
#include "../Container/Vector.h"
namespace Joestar {
	class SwapChain
	{
	public:
		bool IsReady()
		{
			return width != 0;
		}
		U32 width{ 0 };
		U32 height{ 0 };
		GPUImageView* imageView;
		FrameBuffer* framebuffer;
	};
}