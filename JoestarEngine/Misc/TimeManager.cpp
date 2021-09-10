#include "TimeManager.h"
#include "../IO/Log.h"
namespace Joestar {

	TimeManager::TimeManager(EngineContext* ctx) : Super(ctx), frame(0), lastTime(0.f) {
		QueryPerformanceFrequency(&HiResfrequency);
		frequency = HiResfrequency.QuadPart;
	}
	TimeManager::~TimeManager() {}

	void TimeManager::BeginFrame() {
		QueryPerformanceCounter(&counter);
		beginTick = counter.QuadPart;
		++frame;
		LOG("Frame: %d\n", frame);
	}

	void TimeManager::EndFrame() {
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		endTick = counter.QuadPart;
	}

	float TimeManager::GetElapseTime() {
		U64 dTick = endTick - beginTick;
		U64 usecs =  (dTick * 1000000LL) / frequency;
		return (float)usecs / 1000000.f;
	}
}