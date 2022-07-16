#include "Matrix4x4.h"
#include "Matrix3x3.h"

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

	Matrix4x4f& Matrix4x4f::LookAt(const Vector3f& pos, const Vector3f& dir, const Vector3f& right, const Vector3f& up)
	{
		Get(0, 0) = right.x;		Get(0, 1) = right.y;		Get(0, 2) = right.z;		Get(0, 3) = -Dot(pos, right);
		Get(1, 0) = up.x;		Get(1, 1) = up.y;		Get(1, 2) = up.z;		Get(1, 3) = -Dot(pos, up);
		Get(2, 0) = dir.x;		Get(2, 1) = dir.y;		Get(2, 2) = dir.z;		Get(2, 3) = -Dot(pos, dir);
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

		//Get(0, 0) = cotangent / aspect;	Get(0, 1) = 0.0F;      Get(0, 2) = 0.0F;                    Get(0, 3) = 0.0F;
		//Get(1, 0) = 0.0F;               Get(1, 1) = -cotangent; Get(1, 2) = 0.0F;                    Get(1, 3) = 0.0F;
		//Get(2, 0) = 0.0F;               Get(2, 1) = 0.0F;      Get(2, 2) = (zFar + zNear) / deltaZ; Get(2, 3) = 2.0F * zNear * zFar / deltaZ;
		//Get(3, 0) = 0.0F;               Get(3, 1) = 0.0F;      Get(3, 2) = -1.0F;                   Get(3, 3) = 0.0F;

		Get(0, 0) = cotangent / aspect;	Get(0, 1) = 0.0F;      Get(0, 2) = 0.0F;                    Get(0, 3) = 0.0F;
		Get(1, 0) = 0.0F;               Get(1, 1) = -cotangent; Get(1, 2) = 0.0F;                    Get(1, 3) = 0.0F;
		Get(2, 0) = 0.0F;               Get(2, 1) = 0.0F;      Get(2, 2) = -zFar / deltaZ;			Get(2, 3) = zNear * zFar / deltaZ;
		Get(3, 0) = 0.0F;               Get(3, 1) = 0.0F;      Get(3, 2) = 1.0F;                   Get(3, 3) = 0.0F;

		return *this;
	}

	Matrix4x4f& Matrix4x4f::SetFromToRotation(const Vector3f& from, const Vector3f& to) {

		Matrix3x3f mat;
		mat.SetFromToRotation(from, to);
		Get(0, 0) = mat[0];		Get(0, 1) = mat[1];		Get(0, 2) = mat[2];		Get(0, 3) = 0.0;
		Get(1, 0) = mat[3];		Get(1, 1) = mat[4];		Get(1, 2) = mat[5];		Get(1, 3) = 0.0;
		Get(2, 0) = mat[6];		Get(2, 1) = mat[7];		Get(2, 2) = mat[8];		Get(2, 3) = 0.0;
		Get(3, 0) = 0.0;		Get(3, 1) = 0.0;		Get(3, 2) = 0.0;		Get(3, 3) = 1.0;
		return *this;
	}

	Matrix4x4f& Matrix4x4f::SetOrtho(
		float left,
		float right,
		float bottom,
		float top,
		float zNear,
		float zFar)
	{
		SetIdentity();

		float deltax = right - left;
		float deltay = top - bottom;
		float deltaz = zFar - zNear;

		Get(0, 0) = 2.0F / deltax;
		Get(0, 3) = -(right + left) / deltax;
		Get(1, 1) = 2.0F / deltay;
		Get(1, 3) = -(top + bottom) / deltay;
		Get(2, 2) = -1.0F / deltaz;
		Get(2, 3) = -zNear / deltaz;
		return *this;
	}

}