#pragma once
#include <thread>
namespace Joestar {
	class Thread {
	public:
		Thread();
		virtual void ThreadFunc() {}
	protected:
		std::thread* mThread;
	};
}