#pragma once
#include "Thread.h"
#include "../Graphics/RenderCommand.h"
#include "../Container/Vector.h"
namespace Joestar {
	class RenderThread : public Thread {
	public:
		RenderThread(Vector<GFXCommandBuffer*>& cmdBufs, Vector<GFXCommandBuffer*>& comCmdBufs) : cmdBuffers(cmdBufs),
			computeCmdBuffers(comCmdBufs)
		{}
		//virtual void InitRenderContext() {}
		//virtual void DrawFrame(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx) {}
		virtual void DrawFrame(GFXCommandBuffer* cmdBuffer) {}
		virtual void DispatchCompute(GFXCommandBuffer* cmdBuffer) {}
	protected:
		Vector<GFXCommandBuffer*> cmdBuffers;
		Vector<GFXCommandBuffer*> computeCmdBuffers;

		bool bInit{ false };
		bool bExit{ false };
		U32 frameIndex = 0;
	};
}