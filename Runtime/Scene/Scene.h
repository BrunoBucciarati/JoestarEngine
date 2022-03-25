#pragma once
#include "../Base/GameObject.h"
#include "../Base/Light.h"
#include "../Graphics/UniformData.h"
#include "../Graphics/FrameBufferDef.h"
#include "../Core/SubSystem.h"
#include "../Graphics/Material.h"
#include "../Graphics/Graphics.h"
#include "../Container/Vector.h"
#include "../Container/HashMap.h"

namespace Joestar {
	class Camera;
	class RenderPass;
	class Scene : public Object {
		REGISTER_OBJECT(Scene, Object);
	public:
		explicit Scene(EngineContext* ctx);
		void PreRenderCompute();
		void RenderScene(Camera* camera);
		void RenderLights();
		void RenderSkybox();
		void Update(float);
		void CreateLights();
		void CreateCompute();
		void RenderShadowMap();
	private:
		Vector<GameObject*> gameObjects;
		Vector<Light*> lights;
		Light* mainLight;
		GameObject* selection;
		Material* skyboxMat;
		Material* lightMat;
		LightBatch* lightBatch;
		//ComputeBuffer* shComputeBuffer;
		Shader* shComputeShader;
		Shader* shadowShader;
		Camera* shadowCam;
		FrameBufferDef* shadowMapFB;
		ComputeSHConsts computeSHConsts;
		Texture* shCube;
		LightBlocks lightBlocks;
		Graphics* mGraphics;
		SharedPtr<RenderPass> mMainRenderPass;
	};
}