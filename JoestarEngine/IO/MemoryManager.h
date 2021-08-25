#pragma once
#include "../Base/SubSystem.h"
#include "Log.h"
//int frameCount = 0;
void* operator new(std::size_t sz) {
	//if (sz > 20000) {
	//	frameCount++;
	//	LOGWARN("size: %d, frameCount: %d\n", sz, frameCount)
	//	//if (frameCount > 500)
	//	//	int t = 0;
	//}
	return malloc(sz);
}
namespace Joestar {
	class MemoryManager : public SubSystem {
		REGISTER_SUBSYSTEM(MemoryManager)
	public:
		explicit MemoryManager(EngineContext* ctx) : Super(ctx) {}
	};
}