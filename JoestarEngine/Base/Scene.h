#pragma once
#include <vector>
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "../Graphics/UniformData.h"
#include "../Graphics/FrameBufferDef.h"
#include "SubSystem.h"
#include "../Graphics/Material.h"
#include "../Graphics/Graphics.h"
#include "../Container/Vector.h"
#include "../Container/HashMap.h"

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
		void RenderShadowMap();
	private:
		std::vector<GameObject*> gameObjects;
		std::vector<Light*> lights;
		Light* mainLight;
		Camera* camera;
		GameObject* selection;
		Material* skyboxMat;
		Material* lightMat;
		LightBatch* lightBatch;
		ComputeBuffer* shComputeBuffer;
		Shader* shComputeShader;
		Shader* shadowShader;
		Camera* shadowCam;
		FrameBufferDef* shadowMapFB;
		ComputeSHConsts computeSHConsts;
		Texture* shCube;
		LightBlocks lightBlocks;
		Graphics* graphics;
		Vector<GameObject*> tests;
		HashSet<int> testSet;
		HashMap<int, GameObject*> testMap;
	};
}