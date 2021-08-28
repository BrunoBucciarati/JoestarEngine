#pragma once
#include "../Base/SubSystem.h"
#include "Log.h"
void* operator new(std::size_t sz) {
	return malloc(sz);
}
namespace Joestar {
	class MemoryManager : public SubSystem {
		REGISTER_SUBSYSTEM(MemoryManager)
	public:
		explicit MemoryManager(EngineContext* ctx) : Super(ctx) {}
	};
}