#pragma once
#include "Thread.h"
#include "../Graphics/RenderCommand.h"
#include "../Graphics/RenderAPIProtocol.h"
#include "../Container/Vector.h"
#include "../Container/Ptr.h"
namespace Joestar {
	class RenderThread : public Thread {
	public:
		REGISTER_OBJECT(RenderThread, Thread);
		explicit RenderThread(EngineContext* ctx, Vector<GFXCommandBuffer*>& cmdBufs, Vector<GFXCommandBuffer*>& comCmdBufs) : Super(ctx),
			cmdBuffers(cmdBufs),
			computeCmdBuffers(comCmdBufs)
		{
		}
		virtual void DrawFrame(GFXCommandBuffer* cmdBuffer) {}
		virtual void DispatchCompute(GFXCommandBuffer* cmdBuffer) {}
		void InitContext();
		virtual void ThreadFunc();
	protected:
		Vector<GFXCommandBuffer*> cmdBuffers;
		Vector<GFXCommandBuffer*> computeCmdBuffers;
		UniquePtr<RenderAPIProtocol> mProtocol;

		bool bInit{ false };
		bool bExit{ false };
		U32 frameIndex = 0;
	};
}