#include "Matrix4x4.h"

namespace Joestar {
	Matrix4x4f CreateIdentityMatrix4x4f() {
		Matrix4x4f tmp; tmp.SetIdentity(); return tmp;
	}
	const Matrix4x4f Matrix4x4f::identity = CreateIdentityMatrix4x4f();

	Matrix4x4f& Matrix4x4f::SetIdentity()
	{
		Get(0, 0) = 1.0;	Get(0, 1) = 0.0;	Get(0, 2) = 0.0;	Get(0, 3) = 0.0;
		Get(1, 0) = 0.0;	Get(1, 1) = 1.0;	Get(1, 2) = 0.0;	Get(1, 3) = 0.0;
		Get(2, 0) = 0.0;	Get(2, 1) = 0.0;	Get(2, 2) = 1.0;	Get(2, 3) = 0.0;
		Get(3, 0) = 0.0;	Get(3, 1) = 0.0;	Get(3, 2) = 0.0;	Get(3, 3) = 1.0;
		return *this;
	}

	Matrix4x4f::Matrix4x4f(const float data[16])
	{
		for (int i = 0; i < 16; i++)
			m_Data[i] = data[i];
	}

	Matrix4x4f& Matrix4x4f::LookAt(const Vector3f& pos, const Vector3f& target, const Vector3f& up) {
		Vector3f dir = Normalize(target - pos);
		Vector3f right = Normalize(Cross(dir, up));
		Vector3f up1 = Normalize(Cross(right, dir));

		Get(0, 0) = right.x;		Get(0, 1) = up1.x;		Get(0, 2) = dir.x;		Get(0, 3) = -Dot(pos, right);;
		Get(1, 0) = right.y;		Get(1, 1) = up1.y;		Get(1, 2) = dir.y;		Get(1, 3) = -Dot(pos, up1);
		Get(2, 0) = -right.z;		Get(2, 1) = -up1.z;		Get(2, 2) = -dir.z;		Get(2, 3) = Dot(pos, dir);
		Get(3, 0) = 0.0;			Get(3, 1) = 0.0;		Get(3, 2) = 0.0;		Get(3, 3) = 1.0;

		return *this;
	}

	Matrix4x4f& Matrix4x4f::SetPerspective(
		float fovy,
		float aspect,
		float zNear,
		float zFar)
	{
		float cotangent, deltaZ;
		float radians = Deg2Rad(fovy / 2.0f);
		cotangent = cos(radians) / sin(radians);
		deltaZ = zNear - zFar;

		Get(0, 0) = cotangent / aspect;	Get(0, 1) = 0.0F;      Get(0, 2) = 0.0F;                    Get(0, 3) = 0.0F;
		Get(1, 0) = 0.0F;               Get(1, 1) = cotangent; Get(1, 2) = 0.0F;                    Get(1, 3) = 0.0F;
		Get(2, 0) = 0.0F;               Get(2, 1) = 0.0F;      Get(2, 2) = (zFar + zNear) / deltaZ; Get(2, 3) = 2.0F * zNear * zFar / deltaZ;
		Get(3, 0) = 0.0F;               Get(3, 1) = 0.0F;      Get(3, 2) = -1.0F;                   Get(3, 3) = 0.0F;

		return *this;
	}
}