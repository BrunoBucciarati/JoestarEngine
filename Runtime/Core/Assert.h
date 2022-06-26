#pragma once

#ifdef _WIN64
#define DEBUG_BREAK _wassert(_CRT_WIDE(__FILE__), _CRT_WIDE(__FILE__), (unsigned)(__LINE__))
#else
#define DEBUG_BREAK _asm { int 3 }
#endif

#define JOESTAR_ASSERT(condition) \
	do \
	{ \
		if (!condition)\
			DEBUG_BREAK;\
	}