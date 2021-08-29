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
		Matrix4x4f& GetAfflineTransform() { return mat;  }
		void SetPosition(Vector3f& v) { translate = v; }
		void SetScale(Vector3f& v) { scale = v; }
		void SetRotation(Quaternionf& q) { rotation = q; }
		void SetLocalEulerAngles(Vector3f& euler) {
			rotation = EulerToQuaternion(euler);
		}
		
	private:
		Matrix4x4f mat;
		Vector3f translate;
		Vector3f scale;
		Quaternionf rotation;
		bool matDirty = false;
	};
}