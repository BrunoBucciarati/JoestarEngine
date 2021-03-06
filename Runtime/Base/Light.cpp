#include "Light.h"
#include "../Graphics/ProceduralMesh.h"
#include "../Container/Vector.h"
namespace Joestar {
	//void Light::UpdateMatrix()
	//{
	//	if (direction != Vector3f::Zero)
	//		mat.SetFromToRotation(Vector3f::Front, direction);
	//	mat.SetPosition(position);

	//	//light should be small
	//	mat.SetScale(0.2);
	//}
	Light::~Light() {}
	void DirectionalLight::Init() {
	}
	DirectionalLight::~DirectionalLight(){}
	void PointLight::Init() {
	}
	PointLight::~PointLight() {}
	void SpotLight::Init() {
	}
	SpotLight::~SpotLight() {}

	LightBatch::LightBatch(EngineContext* ctx) : Super(ctx) {
		mat = NEW_OBJECT(Material);
		Shader* shader = NEW_OBJECT(Shader);
		shader->SetShader("light");
		//mat->SetShader(shader);

		mesh = GetSubsystem<ProceduralMesh>()->GetUVSphere();

		modelMatrix.SetIdentity();
	}

	LightBatch::~LightBatch() {

	}

	void LightBatch::SetLights(Vector<Light*>& ls) {
		lights = ls;
		//instanceBuffer = new InstanceBuffer;
		//instanceBuffer->SetCount(ls.Size());
		//U32 flag = mat->GetShader()->GetInstanceAttributeFlag();
		//instanceBuffer->PrepareInstanceData(flag);

		//U8* buffer = instanceBuffer->GetBuffer();
		////main light is dir light
		//for (int i = 0; i < ls.Size(); ++i) {
		//	instanceBuffer->SetElementData((U8*)&ls[i]->GetPosition(), i);
		//}
	}
}