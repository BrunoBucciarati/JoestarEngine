#include "Vector2.h"

namespace Joestar {
	const float		Vector2f::epsilon = 0.00001F;
	const float		Vector2f::infinity = std::numeric_limits<float>::infinity();
	const Vector2f	Vector2f::infinityVec = Vector2f(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

	const Vector2f	Vector2f::zero = Vector2f(0, 0);
	const Vector2f	Vector2f::xAxis = Vector2f(1, 0);
	const Vector2f	Vector2f::yAxis = Vector2f(0, 1);
}