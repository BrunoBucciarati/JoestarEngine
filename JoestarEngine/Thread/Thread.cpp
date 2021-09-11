#include "Thread.h"
namespace Joestar {
	static void EntryPoint(Thread* t) {
		t->ThreadFunc();
	}
	Thread::Thread() {
		mThread = new std::thread(&EntryPoint, this);
	};
}