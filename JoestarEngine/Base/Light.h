#pragma once
#include "../Base/Object.h"
#include "../Math/Vector3.h"
namespace Joestar {
	enum LightType {
		DIRECTIONAL_LIGHT = 0,
		POINT_LIGHT,
		SPOT_LIGHT
	};
	class Light {
		REGISTER_OBJECT(Light, Object)
	public:
		virtual void Init() {}
		Vector3f& GetDirection() { return direction; }
		Vector3f& GetPosition() { return position; }
		float GetIntensity() { return intensity; }
		Vector3f& GetColor() { return lightColor; }
		LightType& GetType() { return lightType; }
	protected:
		float intensity = 1.0f;
		LightType lightType;
		Vector3f direction = Vector3f::zero;
		Vector3f lightColor = Vector3f::one;
		Vector3f position = Vector3f::zero;
	};

	class DirectionalLight : Light {
		REGISTER_OBJECT(DirectionalLight, Light)
	public:
		void Init();
	};

	class PointLight : Light {
		REGISTER_OBJECT(PointLight, Light)
	public:
		void Init();
	};

	class SpotLight : Light {
		REGISTER_OBJECT(SpotLight, Light)
	public:
		void Init();
	};
}