#pragma once
#include "Thread.h"
#include "../Graphics/RenderCommand.h"
#include <vector>
namespace Joestar {
	class RenderThread : public Thread {
	public:
		//virtual void InitRenderContext() {}
		//virtual void DrawFrame(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx) {}
		virtual void DrawFrame(GFXCommandBuffer* cmdBuffer) {}
		virtual void DispatchCompute(GFXCommandBuffer* cmdBuffer) {}
	};
}