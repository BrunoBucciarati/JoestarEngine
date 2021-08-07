#pragma once
#include <thread>
namespace Joestar {
	class Thread {
	public:
		Thread();
	private:
		std::thread mThread;
	};
}