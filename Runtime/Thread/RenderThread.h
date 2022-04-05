#pragma once
#include "Thread.h"
#include "../Graphics/RenderCommand.h"
#include "../Graphics/GraphicDefines.h"
#include "../Graphics/RenderAPIProtocol.h"
#include "../Container/Vector.h"
#include "../Container/Ptr.h"
namespace Joestar {
	class GFXCommandList;
	class RenderThread : public Thread {
	public:
		REGISTER_OBJECT(RenderThread, Thread);
		explicit RenderThread(EngineContext* ctx, Vector<GFXCommandBuffer*>& cmdBufs, Vector<GFXCommandBuffer*>& comCmdBufs) : Super(ctx),
			cmdBuffers(cmdBufs),
			computeCmdBuffers(comCmdBufs)
		{}
		void SetGFXCommandList(GFXCommandList** start);
		virtual void DrawFrame(GFXCommandBuffer* cmdBuffer) {}
		virtual void DispatchCompute(GFXCommandBuffer* cmdBuffer) {}
		void InitContext();
		virtual void ThreadFunc();
		void ExecuteGFXCommand(U32 command, GFXCommandList* cmdList);
	protected:
		Vector<GFXCommandBuffer*> cmdBuffers;
		Vector<GFXCommandBuffer*> computeCmdBuffers;
		UniquePtr<RenderAPIProtocol> mProtocol;
		GFXCommandList* mCmdList[MAX_CMDLISTS_IN_FLIGHT]{nullptr};

		bool bInit{ false };
		bool bExit{ false };
		U32 frameIndex = 0;
	};
}