#pragma once
#include "RenderTarget.h"
namespace Joestar
{
	class FrameBuffer : public GPUResource
	{
	public:
		RenderTarget colorAttachment[4];
		RenderTarget depthStencil;
	};
}