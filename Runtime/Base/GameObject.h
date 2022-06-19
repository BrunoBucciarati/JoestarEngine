#pragma once
//#include "Renderer.h"
#define NEW_COMPONENT(CLASS) new CLASS(mContext, this);
#include "../Container/Vector.h"
#include "../Component/Component.h"
#include "../Core/Object.h"
#include "../Component/Transform.h"
namespace Joestar {
#define GetTransform() static_cast<Transform*>(components[0])
	class GameObject : public Object {
		REGISTER_OBJECT(GameObject, Object);
	public:
		explicit GameObject(EngineContext* ctx);

		template<class T>
		T* GetComponent()
		{
			for (auto& comp : components)
			{
				if (comp->GetClassID() == T::GetClassIDStatic())
				{
					return static_cast<T*>(comp);
				}
			}
			T* t = NEW_COMPONENT(T);
			components.Push(t);
			return t;
		}
		template<class T>
		T* HasComponent()
		{
			for (auto& comp : components)
			{
				if (comp->GetClassID() == T::GetClassIDStatic())
				{
					return static_cast<T*>(comp);
				}
			}
			return nullptr;
		}

		Vector3f GetPosition()
		{
			//comp 0 must be transform
			return GetTransform()->GetPosition();
		}

		void SetPosition(const Vector3f& v)
		{
			GetTransform()->SetPosition(v);
		}

		void SetPosition(float x, float y, float z)
		{
			GetTransform()->SetPosition(x, y, z);
		}

		Quaternionf GetRotation()
		{
			//comp 0 must be transform
			return GetTransform()->GetRotation();
		}

		void SetRotation(const Quaternionf& q)
		{
			GetTransform()->SetRotation(q);
		}

		Vector3f GetScale()
		{
			//comp 0 must be transform
			return GetTransform()->GetScale();
		}

		void SetScale(Vector3f& v)
		{
			GetTransform()->SetScale(v);
		}

		void SetScale(F32 s)
		{
			Vector3f scale(s, s, s);
			GetTransform()->SetScale(scale);
		}

		void SetLocalEulerAngles(Vector3f& v) {
			GetTransform()->SetLocalEulerAngles(v);
		}

		Vector3f GetLocalEulerAngles() {
			return GetTransform()->GetLocalEulerAngles();
		}

		Matrix4x4f GetAfflineTransform()
		{
			return GetTransform()->GetAfflineTransform();
		}
	private:
		Vector<Component*> components;
	};
}