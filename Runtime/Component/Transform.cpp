#include "Transform.h"

namespace Joestar {
	void Transform::Init()
	{
		rotation = Quaternionf::identity();
		translate = Vector3f::Zero;
		scale = Vector3f::One;
		mat = Matrix4x4f::identity;
	}

	Matrix4x4f* Transform::GetAfflineTransform()
	{
		if (dirty) {
			mat.SetPosition(translate);
			dirty = false;
		}
		return &mat;
	}
	Transform::~Transform()
	{

	}
}