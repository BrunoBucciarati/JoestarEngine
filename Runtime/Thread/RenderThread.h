#pragma once
#include "Thread.h"
#include "../Graphics/GraphicDefines.h"
#include "../Graphics/RenderAPIProtocol.h"
#include "../Container/Vector.h"
#include "../Container/Ptr.h"
namespace Joestar {
	class GFXCommandList;
	class RenderThread : public Thread
	{
	public:
		REGISTER_OBJECT(RenderThread, Thread);
		explicit RenderThread(EngineContext* ctx) : Super(ctx)
		{}
		void SetGFXCommandList(GFXCommandList** start);
		void InitContext();
		virtual void ThreadFunc();
		void ExecuteGFXCommand(U32 command, GFXCommandList* cmdList);
	protected:
		UniquePtr<RenderAPIProtocol> mProtocol;
		GFXCommandList* mCmdList[MAX_CMDLISTS_IN_FLIGHT]{nullptr};

		bool bInit{ false };
		bool bExit{ false };
		U32 frameIndex = 0;
	};
}