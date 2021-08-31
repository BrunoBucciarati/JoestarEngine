#include "Light.h"
namespace Joestar {
	void DirectionalLight::Init() {
		lightType = DIRECTIONAL_LIGHT;
	}
	void PointLight::Init() {
		lightType = POINT_LIGHT;
	}
	void SpotLight::Init() {
		lightType = SPOT_LIGHT;
	}
}