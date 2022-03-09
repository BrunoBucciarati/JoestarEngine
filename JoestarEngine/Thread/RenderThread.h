#pragma once
#include "Thread.h"
#include "../Graphics/RenderCommand.h"
#include <vector>
namespace Joestar {
	class RenderThread : public Thread {
	public:
		RenderThread(std::vector<GFXCommandBuffer*> cmdBufs, std::vector<GFXCommandBuffer*> comCmdBufs) : cmdBuffers(cmdBufs),
			computeCmdBuffers(comCmdBufs)
		{}
		//virtual void InitRenderContext() {}
		//virtual void DrawFrame(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx) {}
		virtual void DrawFrame(GFXCommandBuffer* cmdBuffer) {}
		virtual void DispatchCompute(GFXCommandBuffer* cmdBuffer) {}
	protected:
		std::vector<GFXCommandBuffer*> cmdBuffers;
		std::vector<GFXCommandBuffer*> computeCmdBuffers;

		bool bInit{ false };
		bool bExit{ false };
		U32 frameIndex = 0;
	};
}