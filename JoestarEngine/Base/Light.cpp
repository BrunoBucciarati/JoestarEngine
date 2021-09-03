#include "Light.h"
#include "../Graphics/ProceduralMesh.h"
namespace Joestar {
	void Light::UpdateMatrix() {
		if (direction != Vector3f::zero)
			mat.SetFromToRotation(Vector3f(0, 0, 1), direction);
		mat.SetPosition(position);

		//light should be small
		mat.SetScale(0.2);
	}
	void DirectionalLight::Init() {
	}
	void PointLight::Init() {
	}
	void SpotLight::Init() {
	}

	LightBatch::LightBatch(EngineContext* ctx) : Super(ctx) {
		mat = NEW_OBJECT(Material);
		Shader* shader = NEW_OBJECT(Shader);
		shader->SetName("light");
		mat->SetShader(shader);

		mesh = GetSubsystem<ProceduralMesh>()->GetUVSphere();

		modelMatrix.SetIdentity();
		modelMatrix.SetScale(0.2);
	}

	void LightBatch::SetLights(std::vector<Light*>& ls) {
		lights = ls;
		instanceBuffer = new InstanceBuffer;
		instanceBuffer->SetCount(ls.size());
		U32 flag = mat->GetShader()->GetInstanceAttributeFlag();
		instanceBuffer->PrepareInstanceData(flag);

		U8* buffer = instanceBuffer->GetBuffer();
		//main light is dir light
		for (int i = 0; i < ls.size(); ++i) {
			instanceBuffer->SetElementData((U8*)&ls[i]->GetPosition(), i);
		}
	}
}