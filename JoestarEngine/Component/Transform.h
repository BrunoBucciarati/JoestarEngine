#pragma once
#include "Component.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Quaternion.h"

namespace Joestar {
	class Transform : public Component {
		REGISTER_COMPONENT(Transform)
	public:
		void Init();
		Vector3f& GetPosition() { return translate; }
		Vector3f& GetScale() { return scale; }
		Vector3f GetLocalEulerAngles() { return QuaternionToEuler(rotation); }
		Quaternionf& GetRotation() { return rotation; }
		Matrix4x4f& GetAfflineTransform();
		void SetPosition(Vector3f& v) {
			translate = v;
			dirty = true;
		}
		void SetPosition(float x, float y, float z) {
			translate.Set(x, y, z);
			dirty = true;
		}
		void SetScale(Vector3f& v) { scale = v; dirty = true;}
		void SetRotation(Quaternionf& q) { rotation = q; dirty = true;}
		void SetLocalEulerAngles(Vector3f& euler) {
			rotation = EulerToQuaternion(euler);
			dirty = true;
		}
		
	private:
		Matrix4x4f mat;
		Vector3f translate;
		Vector3f scale;
		Quaternionf rotation;
		bool dirty = false;
	};
}