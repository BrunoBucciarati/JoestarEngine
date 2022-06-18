#pragma once
#include "../Component/Component.h"
#include "../Base/GameObject.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix4x4.h"
#include "../Graphics/Material.h"
#include "../Graphics/Mesh.h"
namespace Joestar {
	enum LightType {
		DIRECTIONAL_LIGHT = 0,
		POINT_LIGHT,
		SPOT_LIGHT
	};
	class Light : public Component
	{
		REGISTER_OBJECT(Light, Component)
	public:
		explicit Light(EngineContext* ctx, GameObject* go) : Super(ctx, go)
		{
		}
		virtual void Init() {}
		void SetDirection(Vector3f& dir)
		{
			Quaternionf q;
			q.FromLookRotation(dir);
			mGameObject->SetRotation(q);
		}
		void SetDirection(float x, float y, float z)
		{
			Vector3f v(x, y, z);
			SetDirection(v);
		}
		Vector3f GetPosition()
		{
			return mGameObject->GetPosition();
		}
		void SetPosition(Vector3f& pos) {
			mGameObject->SetPosition(pos);
		}
		void SetPosition(float x, float y, float z)
		{
			mGameObject->SetPosition(x, y, z);
		}
		float GetIntensity()
		{
			return mIntensity;
		}
		void SetIntensity(float i)
		{
			mIntensity = i; 
			mFinalColor = i * mColor;
		}
		Vector3f& GetColor()
		{
			return mColor;
		}
		Vector3f& GetIntensityMixColor()
		{
			return mFinalColor;
		}
		void SetColor(Vector3f& c)
		{
			mColor = c;
			mFinalColor = mIntensity * mColor;
		}
		void SetColor(float x, float y, float z)
		{
			mColor.Set(x, y, z);
			mFinalColor = mIntensity * mColor;
		}
		LightType& GetType()
		{
			return mLightType;
		}
		Quaternionf GetWorldRotation()
		{
			return mGameObject->GetRotation();
		}
	protected:
		float mIntensity = 1.0f;
		LightType mLightType;
		Vector3f mColor = Vector3f::One;
		Vector3f mFinalColor = Vector3f::One;
	};

	class DirectionalLight : public Light {
		REGISTER_OBJECT(DirectionalLight, Light)
		explicit DirectionalLight(EngineContext* ctx, GameObject* go) : Super(ctx, go)
		{
			mLightType = DIRECTIONAL_LIGHT;
		}
	public:
		void Init();
	};

	class PointLight : public Light {
		REGISTER_OBJECT(PointLight, Light)
		explicit PointLight(EngineContext* ctx, GameObject* go) : Super(ctx, go)
		{
			mLightType = POINT_LIGHT;
		}
	public:
		void Init();
	};

	class SpotLight : public Light {
		REGISTER_OBJECT(SpotLight, Light)
		explicit SpotLight(EngineContext* ctx, GameObject* go) : Super(ctx, go)
		{
			mLightType = SPOT_LIGHT;
		}
	public:
		void Init();
	};

	class LightBatch : public Object
	{
		REGISTER_OBJECT(LightBatch, Object)
		explicit LightBatch(EngineContext* ctx);
		void SetLights(Vector<Light*>& ls);
		//InstanceBuffer* GetInstanceBuffer() { return instanceBuffer; }
		Material* GetMaterial() { return mat; }
		Mesh* GetMesh() { return mesh; }
		Matrix4x4f& GetModelMatrix() { return modelMatrix; }
	private:
		//InstanceBuffer* instanceBuffer;
		Vector<Light*> lights;
		Material* mat;
		Mesh* mesh;
		Matrix4x4f modelMatrix;
	};
}