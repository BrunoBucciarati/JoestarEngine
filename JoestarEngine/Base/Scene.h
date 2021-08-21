#pragma once
#include <vector>
#include "GameObject.h"
#include "Camera.h"
#include "SubSystem.h"

namespace Joestar {
	class Scene : public SubSystem {
		REGISTER_SUBSYSTEM(Scene);
	public:
		explicit Scene(EngineContext* ctx);
		std::vector<GameObject*> gameObjects;
		void RenderScene();
		void Update();
	private:
		Camera camera;
	};
}