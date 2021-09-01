#pragma once
#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "Vector3.h"
#include "Vector4.h"

namespace Joestar {
	class Matrix3x3f;
	class Quaternionf;

	/// Matrices in unity are column major.
	class Matrix4x4f
	{
	public:
		float m_Data[16];

		Matrix4x4f() {}
		Matrix4x4f(const Matrix3x3f& other);
		explicit Matrix4x4f(const float data[16]);

		bool operator == (const Matrix4x4f& v) const {
			for (int i = 0; i < 16; i++)
				if (m_Data[i] != v[i])
					return false;
			return true;
		}

		float& Get(int row, int column) { return m_Data[row + (column * 4)]; }
		const float& Get(int row, int column)const { return m_Data[row + (column * 4)]; }
		float* GetPtr() { return m_Data; }
		const float* GetPtr()const { return m_Data; }

		float operator [] (int index) const { return m_Data[index]; }
		float& operator [] (int index) { return m_Data[index]; }

		Matrix4x4f& operator *= (const Matrix4x4f& inM);

		Matrix4x4f& operator = (const Matrix3x3f& m);

		Vector3f MultiplyVector3(const Vector3f& inV) const;
		void MultiplyVector3(const Vector3f& inV, Vector3f& output) const;
		Vector4f MultiplyVector4(const Vector4f& inV) const;
		bool PerspectiveMultiplyVector3(const Vector3f& inV, Vector3f& output) const;
		Vector3f MultiplyPoint3(const Vector3f& inV) const;
		void MultiplyPoint3(const Vector3f& inV, Vector3f& output) const;
		bool PerspectiveMultiplyPoint3(const Vector3f& inV, Vector3f& output) const;
		Vector3f InverseMultiplyPoint3Affine(const Vector3f& inV) const;
		Vector3f InverseMultiplyVector3Affine(const Vector3f& inV) const;

		Matrix4x4f& LookAt(const Vector3f& pos, const Vector3f& target, const Vector3f& up);

		Matrix4x4f& SetIdentity();
		Matrix4x4f& SetPerspective(float fovy, float aspect, float zNear, float zFar);

		Vector3f GetAxisX() const;
		Vector3f GetAxisY() const;
		Vector3f GetAxisZ() const;
		Vector3f GetPosition() const;
		Vector4f GetRow(int row) const;
		Vector4f GetColumn(int col) const;
		// these set only these components of the matrix, everything else is untouched!
		void SetAxisX(const Vector3f& v);
		void SetAxisY(const Vector3f& v);
		void SetAxisZ(const Vector3f& v);
		void SetPosition(const Vector3f& v);
		void SetRow(int row, const Vector4f& v);
		void SetColumn(int col, const Vector4f& v);
		void SetScale(float s);
		void SetScale(float x, float y, float z);

		Matrix4x4f& SetFromToRotation(const Vector3f& from, const Vector3f& to);

		static const Matrix4x4f identity;
	};


	inline Vector3f Matrix4x4f::GetAxisX() const {
		return Vector3f(Get(0, 0), Get(1, 0), Get(2, 0));
	}
	inline Vector3f Matrix4x4f::GetAxisY() const {
		return Vector3f(Get(0, 1), Get(1, 1), Get(2, 1));
	}
	inline Vector3f Matrix4x4f::GetAxisZ() const {
		return Vector3f(Get(0, 2), Get(1, 2), Get(2, 2));
	}
	inline Vector3f Matrix4x4f::GetPosition() const {
		return Vector3f(Get(0, 3), Get(1, 3), Get(2, 3));
	}
	inline Vector4f Matrix4x4f::GetRow(int row) const {
		return Vector4f(Get(row, 0), Get(row, 1), Get(row, 2), Get(row, 3));
	}
	inline Vector4f Matrix4x4f::GetColumn(int col) const {
		return Vector4f(Get(0, col), Get(1, col), Get(2, col), Get(3, col));
	}
	inline void Matrix4x4f::SetAxisX(const Vector3f& v) {
		Get(0, 0) = v.x; Get(1, 0) = v.y; Get(2, 0) = v.z;
	}
	inline void Matrix4x4f::SetAxisY(const Vector3f& v) {
		Get(0, 1) = v.x; Get(1, 1) = v.y; Get(2, 1) = v.z;
	}
	inline void Matrix4x4f::SetAxisZ(const Vector3f& v) {
		Get(0, 2) = v.x; Get(1, 2) = v.y; Get(2, 2) = v.z;
	}
	inline void Matrix4x4f::SetPosition(const Vector3f& v) {
		Get(0, 3) = v.x; Get(1, 3) = v.y; Get(2, 3) = v.z;
	}
	inline void Matrix4x4f::SetRow(int row, const Vector4f& v) {
		Get(row, 0) = v.x; Get(row, 1) = v.y; Get(row, 2) = v.z; Get(row, 3) = v.w;
	}
	inline void Matrix4x4f::SetColumn(int col, const Vector4f& v) {
		Get(0, col) = v.x; Get(1, col) = v.y; Get(2, col) = v.z; Get(3, col) = v.w;
	}
	inline void Matrix4x4f::SetScale(float s) {
		Get(0, 0) = s; Get(1, 1) = s; Get(2, 2) = s;
	}
	inline void Matrix4x4f::SetScale(float x, float y, float z) {
		Get(0, 0) = x; Get(1, 1) = y; Get(2, 2) = z;
	}


	inline Vector3f Matrix4x4f::MultiplyPoint3(const Vector3f& v) const
	{
		Vector3f res;
		res.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z + m_Data[12];
		res.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z + m_Data[13];
		res.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z + m_Data[14];
		return res;
	}

	inline void Matrix4x4f::MultiplyPoint3(const Vector3f& v, Vector3f& output) const
	{
		output.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z + m_Data[12];
		output.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z + m_Data[13];
		output.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z + m_Data[14];
	}


	inline Vector3f Matrix4x4f::MultiplyVector3(const Vector3f& v) const
	{
		Vector3f res;
		res.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z;
		res.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z;
		res.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z;
		return res;
	}


	inline Vector4f Matrix4x4f::MultiplyVector4(const Vector4f& v) const
	{
		Vector4f res;
		res.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z + m_Data[12] * v.w;
		res.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z + m_Data[13] * v.w;
		res.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z + m_Data[14] * v.w;
		res.w = m_Data[3] * v.x + m_Data[7] * v.y + m_Data[11] * v.z + m_Data[15] * v.w;
		return res;
	}

	inline void Matrix4x4f::MultiplyVector3(const Vector3f& v, Vector3f& output) const
	{
		output.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z;
		output.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z;
		output.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z;
	}


	inline bool Matrix4x4f::PerspectiveMultiplyPoint3(const Vector3f& v, Vector3f& output) const
	{
		Vector3f res;
		float w;
		res.x = Get(0, 0) * v.x + Get(0, 1) * v.y + Get(0, 2) * v.z + Get(0, 3);
		res.y = Get(1, 0) * v.x + Get(1, 1) * v.y + Get(1, 2) * v.z + Get(1, 3);
		res.z = Get(2, 0) * v.x + Get(2, 1) * v.y + Get(2, 2) * v.z + Get(2, 3);
		w = Get(3, 0) * v.x + Get(3, 1) * v.y + Get(3, 2) * v.z + Get(3, 3);
		if (Abs(w) > 1.0e-7f)
		{
			float invW = 1.0f / w;
			output.x = res.x * invW;
			output.y = res.y * invW;
			output.z = res.z * invW;
			return true;
		}
		else
		{
			output.x = 0.0f;
			output.y = 0.0f;
			output.z = 0.0f;
			return false;
		}
	}

	inline bool Matrix4x4f::PerspectiveMultiplyVector3(const Vector3f& v, Vector3f& output) const
	{
		Vector3f res;
		float w;
		res.x = Get(0, 0) * v.x + Get(0, 1) * v.y + Get(0, 2) * v.z;
		res.y = Get(1, 0) * v.x + Get(1, 1) * v.y + Get(1, 2) * v.z;
		res.z = Get(2, 0) * v.x + Get(2, 1) * v.y + Get(2, 2) * v.z;
		w = Get(3, 0) * v.x + Get(3, 1) * v.y + Get(3, 2) * v.z;
		if (Abs(w) > 1.0e-7f)
		{
			float invW = 1.0f / w;
			output.x = res.x * invW;
			output.y = res.y * invW;
			output.z = res.z * invW;
			return true;
		}
		else
		{
			output.x = 0.0f;
			output.y = 0.0f;
			output.z = 0.0f;
			return false;
		}
	}

	inline Vector3f Matrix4x4f::InverseMultiplyPoint3Affine(const Vector3f& inV) const
	{
		Vector3f v(inV.x - Get(0, 3), inV.y - Get(1, 3), inV.z - Get(2, 3));
		Vector3f res;
		res.x = Get(0, 0) * v.x + Get(1, 0) * v.y + Get(2, 0) * v.z;
		res.y = Get(0, 1) * v.x + Get(1, 1) * v.y + Get(2, 1) * v.z;
		res.z = Get(0, 2) * v.x + Get(1, 2) * v.y + Get(2, 2) * v.z;
		return res;
	}

	inline Vector3f Matrix4x4f::InverseMultiplyVector3Affine(const Vector3f& v) const
	{
		Vector3f res;
		res.x = Get(0, 0) * v.x + Get(1, 0) * v.y + Get(2, 0) * v.z;
		res.y = Get(0, 1) * v.x + Get(1, 1) * v.y + Get(2, 1) * v.z;
		res.z = Get(0, 2) * v.x + Get(1, 2) * v.y + Get(2, 2) * v.z;
		return res;
	}

	inline bool IsFinite(const Matrix4x4f& f)
	{
		return
			IsFinite(f.m_Data[0]) & IsFinite(f.m_Data[1]) & IsFinite(f.m_Data[2]) &
			IsFinite(f.m_Data[4]) & IsFinite(f.m_Data[5]) & IsFinite(f.m_Data[6]) &
			IsFinite(f.m_Data[8]) & IsFinite(f.m_Data[9]) & IsFinite(f.m_Data[10]) &
			IsFinite(f.m_Data[12]) & IsFinite(f.m_Data[13]) & IsFinite(f.m_Data[14]) & IsFinite(f.m_Data[15]);
	}
}
#endif
