#pragma once
#include <thread>
#include "../Core/Object.h"
namespace Joestar {
	class Thread : public Object {
		REGISTER_OBJECT(Thread, Object);
	public:
		explicit Thread(EngineContext* ctx);
		virtual void ThreadFunc() {}
	protected:
		std::thread* mThread;
	};
}