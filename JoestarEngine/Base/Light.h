#pragma once
#include "../Base/Object.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix4x4.h"
#include "../Graphics/VertexData.h"
#include "../Graphics/Material.h"
#include "../Graphics/Mesh.h"
namespace Joestar {
	enum LightType {
		DIRECTIONAL_LIGHT = 0,
		POINT_LIGHT,
		SPOT_LIGHT
	};
	class Light : public Object {
		REGISTER_OBJECT(Light, Object)
	public:
		explicit Light(EngineContext* ctx) : Super(ctx), mat(Matrix4x4f::identity) {
			position.Set(0.f, 0.f, 0.f);
		}
		virtual void Init() {}
		Vector3f& GetDirection() { return direction; }
		void SetDirection(Vector3f& dir) { direction = dir; UpdateMatrix(); }
		void SetDirection(float x, float y, float z) { direction.Set(x, y, z); UpdateMatrix();}
		Vector3f& GetPosition() { return position; }
		void SetPosition(Vector3f& pos) { position = pos;  UpdateMatrix();}
		void SetPosition(float x, float y, float z) { position.Set(x, y, z); UpdateMatrix();}
		float GetIntensity() { return intensity; }
		void UpdateMatrix();
		Matrix4x4f& GetModelMatrix() { return mat; }
		void SetIntensity(float i) { intensity = i; }
		Vector3f& GetColor() { return lightColor; }
		void SetColor(Vector3f& c) { lightColor = c; }
		void SetColor(float x, float y, float z) { lightColor.Set(x, y, z); }
		LightType& GetType() { return lightType; }
	protected:
		float intensity = 1.0f;
		LightType lightType;
		Vector3f direction = Vector3f::zero;
		Vector3f lightColor = Vector3f::one;
		Vector3f position = Vector3f::zero;
		Matrix4x4f mat;
	};

	class DirectionalLight : public Light {
		REGISTER_OBJECT(DirectionalLight, Light)
		explicit DirectionalLight(EngineContext* ctx) : Super(ctx) {
			lightType = DIRECTIONAL_LIGHT;
		}
	public:
		void Init();
	};

	class PointLight : public Light {
		REGISTER_OBJECT(PointLight, Light)
		explicit PointLight(EngineContext* ctx) : Super(ctx) {
			lightType = POINT_LIGHT;
		}
	public:
		void Init();
	};

	class SpotLight : public Light {
		REGISTER_OBJECT(SpotLight, Light)
		explicit SpotLight(EngineContext* ctx) : Super(ctx) {
			lightType = SPOT_LIGHT;
		}
	public:
		void Init();
	};

	class LightBatch : public Object {
		REGISTER_OBJECT(LightBatch, Object)
		explicit LightBatch(EngineContext* ctx);
		void SetLights(std::vector<Light*>& ls);
		InstanceBuffer* GetInstanceBuffer() { return instanceBuffer; }
		Material* GetMaterial() { return mat; }
		Mesh* GetMesh() { return mesh; }
		Matrix4x4f& GetModelMatrix() { return modelMatrix; }
	private:
		InstanceBuffer* instanceBuffer;
		std::vector<Light*> lights;
		Material* mat;
		Mesh* mesh;
		Matrix4x4f modelMatrix;
	};
}