#include "Thread.h"
namespace Joestar {
	static void EntryPoint(Thread* t)
	{
		t->ThreadFunc();
	}
	Thread::Thread(EngineContext* ctx) : Super(ctx)
	{
		mThread = new std::thread(&EntryPoint, this);
	};
	Thread::~Thread()
	{
		delete mThread;
	}
}