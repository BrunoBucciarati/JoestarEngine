#pragma once
#include "Thread.h"
namespace Joestar {
	class RenderThread : public Thread {
	public:
		virtual void InitRenderContext() {}
		virtual void DrawFrame() {}
	};
}