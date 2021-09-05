#include "MemoryManager.h"
void* operator new(std::size_t sz, MemoryType type, int align, const char* file, int line) {
	return malloc(sz);
}

void operator delete(void* p, MemoryType type, int align, const char* file, int line) {
	free(p);
}
void* operator new(std::size_t sz) {
	return malloc(sz);
}