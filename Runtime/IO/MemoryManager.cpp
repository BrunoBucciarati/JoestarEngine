#include "MemoryManager.h"
#include "Log.h"
void* operator new(std::size_t sz, MemoryType type, int align, const char* file, int line) {
	LOGWARN("line = %d file = %s\n",line, file);
	return malloc(sz);
}

void operator delete(void* p, MemoryType type, int align, const char* file, int line) {
	free(p);
}
//void* operator new(std::size_t sz) {
//	return malloc(sz);
//}

namespace Joestar {
	MemoryManager::~MemoryManager() {}
}