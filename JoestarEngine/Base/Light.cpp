#include "Light.h"
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
}