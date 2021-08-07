#pragma once
#include "Mesh.h"
#include "../Thread/RenderThread.h"
#include "../Base/SubSystem.h"
#include "../Base/EngineContext.h"
namespace Joestar {
	class Graphics : public SubSystem {
		REGISTER_OBJECT(Graphics, SubSystem)
	public:
		explicit Graphics(EngineContext* context);
		void Init();
		virtual void DrawTriangle() {}
		void MainLoop();
		virtual void DrawMesh(Mesh* mesh) {}

	private:
		RenderThread* renderThread;
	};
}