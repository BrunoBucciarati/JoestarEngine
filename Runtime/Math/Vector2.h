#pragma once
#ifndef VECTOR2_H
#define VECTOR2_H

#include <algorithm>
#include "MathDefs.h"
#include "FloatMath.h"

namespace Joestar {
	class Vector2f
	{
	public:
		float x, y;

		Vector2f() : x(0.f), y(0.f) {}
		Vector2f(float inX, float inY) { x = inX; y = inY; }
		explicit Vector2f(const float* p) { x = p[0]; y = p[1]; }

		void Set(float inX, float inY) { x = inX; y = inY; }

		float* GetPtr() { return &x; }
		const float* GetPtr()const { return &x; }
		float& operator[] (int i) {return (&x)[i]; }
		const float& operator[] (int i)const { return (&x)[i]; }

		Vector2f& operator += (const Vector2f& inV) { x += inV.x; y += inV.y; return *this; }
		Vector2f& operator -= (const Vector2f& inV) { x -= inV.x; y -= inV.y; return *this; }
		Vector2f& operator *= (const float s) { x *= s; y *= s; return *this; }
		Vector2f& operator /= (const float s) { x /= s; y /= s; return *this; }
		bool operator == (const Vector2f& v)const { return x == v.x && y == v.y; }
		bool operator != (const Vector2f& v)const { return x != v.x || y != v.y; }


		Vector2f operator - () const { return Vector2f(-x, -y); }

		Vector2f& Scale(const Vector2f& inV) { x *= inV.x; y *= inV.y; return *this; }

		static const float		epsilon;
		static const float		infinity;
		static const Vector2f	infinityVec;
		static const Vector2f	zero;
		static const Vector2f	xAxis;
		static const Vector2f	yAxis;
	};

	inline Vector2f Scale(const Vector2f& lhs, const Vector2f& rhs) { return Vector2f(lhs.x * rhs.x, lhs.y * rhs.y); }

	inline Vector2f operator + (const Vector2f& lhs, const Vector2f& rhs) { return Vector2f(lhs.x + rhs.x, lhs.y + rhs.y); }
	inline Vector2f operator - (const Vector2f& lhs, const Vector2f& rhs) { return Vector2f(lhs.x - rhs.x, lhs.y - rhs.y); }
	inline float Dot(const Vector2f& lhs, const Vector2f& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }

	inline float SqrMagnitude(const Vector2f& inV) { return Dot(inV, inV); }
	inline float Magnitude(const Vector2f& inV) { return Sqrt(Dot(inV, inV)); }

	inline float Angle(const Vector2f& lhs, const Vector2f& rhs) { return acos(Min(1.0f, Max(-1.0f, Dot(lhs, rhs) / (Magnitude(lhs) * Magnitude(rhs))))); }

	inline Vector2f operator * (const Vector2f& inV, float s) { return Vector2f(inV.x * s, inV.y * s); }
	inline Vector2f operator * (const float s, const Vector2f& inV) { return Vector2f(inV.x * s, inV.y * s); }
	inline Vector2f operator / (const Vector2f& inV, float s) { Vector2f temp(inV); temp /= s; return temp; }
	inline Vector2f Inverse(const Vector2f& inVec) { return Vector2f(1.0F / inVec.x, 1.0F / inVec.y); }

	// Normalizes a vector, asserts if the vector can be normalized
	inline Vector2f Normalize(const Vector2f& inV) { return inV / Magnitude(inV); }
	// Normalizes a vector, returns default vector if it can't be normalized
	inline Vector2f NormalizeSafe(const Vector2f& inV, const Vector2f& defaultV = Vector2f::zero);

	inline Vector2f Lerp(const Vector2f& from, const Vector2f& to, float t) { return to * t + from * (1.0f - t); }

	// Returns a vector with the smaller of every component from v0 and v1
	inline Vector2f Min(const Vector2f& lhs, const Vector2f& rhs) { return Vector2f(Min(lhs.x, rhs.x), Min(lhs.y, rhs.y)); }
	// Returns a vector with the larger  of every component from v0 and v1
	inline Vector2f Max(const Vector2f& lhs, const Vector2f& rhs) { return Vector2f(Max(lhs.x, rhs.x), Max(lhs.y, rhs.y)); }

	bool CompareApproximately(const Vector2f& inV0, const Vector2f& inV1, float inMaxDist = Vector2f::epsilon);

	inline bool CompareApproximately(const Vector2f& inV0, const Vector2f& inV1, float inMaxDist)
	{
		return SqrMagnitude(inV1 - inV0) < inMaxDist * inMaxDist;
	}

	inline bool IsNormalized(const Vector2f& vec, float epsilon = Vector2f::epsilon)
	{
		return Equals(SqrMagnitude(vec), 1.0F, epsilon);
	}

	/// Returns the abs of every component of the vector
	inline Vector2f Abs(const Vector2f& v) { return Vector2f(Abs(v.x), Abs(v.y)); }

	inline bool IsFinite(const Vector2f& f)
	{
		return IsFinite(f.x) & IsFinite(f.y);
	}


	inline Vector2f NormalizeSafe(const Vector2f& inV, const Vector2f& defaultV)
	{
		float mag = Magnitude(inV);
		if (mag > Vector2f::epsilon)
			return inV / Magnitude(inV);
		else
			return defaultV;
	}

}
#endif