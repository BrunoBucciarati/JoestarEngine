#pragma once
//#include "Renderer.h"
#define NEW_COMPONENT(CLASS) new CLASS(mContext, this);
#include <vector>
#include "../Component/Component.h"
#include "Object.h"
#include "../Component/Transform.h"
namespace Joestar {
#define GetTransform() static_cast<Transform*>(components[0])
	class GameObject : public Object {
		REGISTER_OBJECT(GameObject, Object);
	public:
		explicit GameObject(EngineContext* ctx);

		template<class T>
		T* GetComponent() {
			for (auto& comp : components) {
				if (comp->GetClassID() == T::GetClassIDStatic()) {
					return static_cast<T*>(comp);
				}
			}
			T* t = NEW_COMPONENT(T);
			components.push_back(t);
			return t;
		}
		template<class T>
		T* HasComponent() {
			for (auto& comp : components) {
				if (comp->GetClassID() == T::GetClassIDStatic()) {
					return static_cast<T*>(comp);
				}
			}
			return nullptr;
		}

		Vector3f GetPosition() {
			//comp 0 must be transform
			return GetTransform()->GetPosition();
		}

		void SetPosition(Vector3f& v) {
			GetTransform()->SetPosition(v);
		}

		Quaternionf GetRotation() {
			//comp 0 must be transform
			return GetTransform()->GetRotation();
		}

		void SetRotation(Quaternionf& q) {
			GetTransform()->SetRotation(q);
		}

		Vector3f GetScale() {
			//comp 0 must be transform
			return GetTransform()->GetScale();
		}

		void SetScale(Vector3f& v) {
			GetTransform()->SetScale(v);
		}

		void SetLocalEulerAngles(Vector3f& v) {
			GetTransform()->SetLocalEulerAngles(v);
		}

		Vector3f GetLocalEulerAngles() {
			return GetTransform()->GetLocalEulerAngles();
		}

		Matrix4x4f& GetAfflineTransform() {
			return GetTransform()->GetAfflineTransform();
		}
	private:
		std::vector<Component*> components;
	};
}