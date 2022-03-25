#pragma once
namespace Joestar {
	/// Return the smaller of two values.
	template <class T, class U>
	inline T Min(T lhs, U rhs) { return lhs < rhs ? lhs : rhs; }
	/// Return the smaller of three values.
	template <class T, class U, class V>
	inline T Min3(T a, U b, V c) { return Min(Min(a, b), c); }

	/// Return the larger of two values.
	template <class T, class U>
	inline T Max(T lhs, U rhs) { return lhs > rhs ? lhs : rhs; }

	/// Return the larger of three values.
	template <class T, class U, class V>
	inline T Max3(T a, U b, V c) { return Max(Max(a, b), c); }

	/// Return absolute value of a value
	template <class T>
	inline T Abs(T value) { return value >= 0.0 ? value : -value; }

	/// Return the sign of a float (-1, 0 or 1.)
	template <class T>
	inline T Sign(T value) { return value > 0.0 ? 1.0 : (value < 0.0 ? -1.0 : 0.0); }

	/// Return square root of X.
	template <class T> inline T Sqrt(T x) { return sqrt(x); }

	/// Check whether two floating point values are equal within accuracy.
	template <class T>
	inline bool Equals(T lhs, T rhs) { return lhs + std::numeric_limits<T>::epsilon() >= rhs && lhs - std::numeric_limits<T>::epsilon() <= rhs; }
	/// Check whether two floating point values are equal within accuracy.
	template <class T>
	inline bool Equals(T lhs, T rhs, T tolerance) { return lhs + tolerance >= rhs && lhs - tolerance <= rhs; }
}