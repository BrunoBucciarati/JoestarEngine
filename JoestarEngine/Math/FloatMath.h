#pragma once
#include <math.h>
#include <stdint.h>

namespace Joestar {
	const float PI = 3.1415926535893238;
	const float EPSILON = 1e-6;
	inline float InvSqrt(float p) { return 1.0F / sqrt(p); }
	inline float Sqrt(float p) { return sqrt(p); }
	inline float Abs(float x) { return x > 0 ? x : -x; }
	inline float Dot3(float* x, float* y) {
		return  x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
	}
	inline float* Cross3(float* lhs, float* rhs) {
		  float ret[] = { lhs[1] * rhs[2] - lhs[2] * rhs[1],
			lhs[2] * rhs[0] - lhs[0] * rhs[2],
			lhs[0] * rhs[1] - lhs[1] * rhs[0] };
		  return ret;
	}
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
	inline float Floor(float f) {
		return floor(f);
	}
	inline float Log2(float f) {
		return log2(f);
	}
	inline float Sin(float angle) {
		return sin(angle);
	}
	inline float Cos(float angle) {
		return cos(angle);
	}
}