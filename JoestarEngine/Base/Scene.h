#pragma once
#include <vector>
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "../Graphics/UniformData.h"
#include "SubSystem.h"
#include "../Graphics/Material.h"

namespace Joestar {
	class Scene : public SubSystem {
		REGISTER_SUBSYSTEM(Scene);
	public:
		explicit Scene(EngineContext* ctx);
		void PreRenderCompute();
		void RenderScene();
		void RenderLights();
		void RenderSkybox();
		void Update(float);
		void CreateLights();
		void CreateCompute();
	private:
		std::vector<GameObject*> gameObjects;
		std::vector<Light*> lights;
		Light* mainLight;
		Camera camera;
		GameObject* selection;
		Material* skyboxMat;
		Material* lightMat;
		LightBatch* lightBatch;
		ComputeBuffer* shComputeBuffer;
		Shader* shComputeShader;
		ComputeSHConsts computeSHConsts;
		Texture* shCube;
		LightBlocks lightBlocks;
	};
}