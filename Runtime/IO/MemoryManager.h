#pragma once
#include "../Core/SubSystem.h"
#include "Log.h"

#ifndef JOESTAR_MEMORY_H
#define JOESTAR_MEMORY_H

enum MemoryType {
	MEMORY_TEMP = 0,
	MEMORY_GFX_STRUCT = 1,
	MEMORY_STRING = 2,
	MEMORY_CONTAINER = 3,
	MEMORY_GFX_MEMORY = 4,
	MEMORY_TEXTURE = 5,
	MEMORY_FILE = 6,
	MEMORY_TERRAIN = 7
};

enum MemoryAlignment {
	kDefaultMemoryAlignment = sizeof(void*)
};

#if NDEBUG
#define JOJO_NEW(data, type)\
	new (type, kDefaultMemoryAlignment, nullptr, 0) data
#define JOJO_DELETE(data, type) \
	delete (type, kDefaultMemoryAlignment,  nullptr, 0) data
#else
#define JOJO_NEW(data, type)\
	new data
#define JOJO_PLACEMENT_NEW(data, place, type)\
	new (place) data
#define JOJO_NEW_ARRAY(data, sz, type)\
	new data[sz]
	//new (type, kDefaultMemoryAlignment, __FILE__, __LINE__) data
#define JOJO_DELETE(p, type) \
	delete p
#define JOJO_DELETE_ARRAY(p, type) \
	delete[] p
#endif

//void* operator new(std::size_t sz, MemoryType type, int align, const char* file, int line);
//void operator delete(void* p, MemoryType type, int align, const char* file, int line);
//void* operator new(std::size_t sz);

namespace Joestar {
	class MemoryManager : public SubSystem {
		REGISTER_SUBSYSTEM(MemoryManager)
	public:
		explicit MemoryManager(EngineContext* ctx) : Super(ctx) {}
	};
}
#endif