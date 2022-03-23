#pragma once
#include "FrameBuffer.h"
#include "../Container/Vector.h"
namespace Joestar {
	class SwapChain
	{
	public:
		U32 width;
		U32 height;
		GPUImageView* imageView;
		FrameBuffer* framebuffer;
	};
}