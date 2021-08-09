#pragma once
#include <math.h>
#include <stdint.h>

namespace Joestar {
	const float PI = 3.1415926535893238;
	inline float InvSqrt(float p) { return 1.0F / sqrt(p); }
	inline float Sqrt(float p) { return sqrt(p); }
	inline float Abs(float x) { return x > 0 ? x : -x; }
	inline float Min(float x, float y) { return x < y ? x : y; }
	inline float Max(float x, float y) { return x > y ? x : y; }
	inline bool CompareApproximately(float f0, float f1, float epsilon = 0.000001F)
	{
		float dist = (f0 - f1);
		dist = Abs(dist);
		return dist < epsilon;
	}
	inline bool IsFinite(const float& value)
	{
		// Returns false if value is NaN or +/- infinity
		uint32_t intval = *reinterpret_cast<const uint32_t*>(&value);
		return (intval & 0x7f800000) != 0x7f800000;
	}
	inline float Deg2Rad(float deg) {
		return deg * PI / 180;
	}
}