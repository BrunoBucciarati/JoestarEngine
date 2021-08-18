#pragma once
#include <vector>
#include "GameObject.h"
#include "Camera.h"

namespace Joestar {
	class Scene {
	public:
		Scene();
		std::vector<GameObject> gameObjects;
		void RenderScene();
	private:
		Camera camera;
	};
}