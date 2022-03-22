#pragma once
#include "../Core/SubSystem.h"
#include <windows.h>
#include <mmsystem.h>

namespace Joestar {
	class TimeManager : public SubSystem {
		REGISTER_SUBSYSTEM(TimeManager)
	public:
		explicit TimeManager(EngineContext* ctx);
		//float Update();
		void BeginFrame();
		void EndFrame();
		float GetElapseTime();
		U32 GetFrame() { return frame; }
	private:
		U32 frame;
		float lastTime;
		float dt;
		U64 beginTick;
		U64 endTick;
		U64 frequency;
		LARGE_INTEGER counter;
		LARGE_INTEGER HiResfrequency;
	};
}