#include "Vector3.h"

namespace Joestar {
	const Vector3f	Vector3f::Zero = Vector3f(0, 0, 0);
	const Vector3f	Vector3f::One = Vector3f(1.0F, 1.0F, 1.0F);
	const Vector3f	Vector3f::Right = Vector3f(1.0F, 0.0F, 0.0F);
	const Vector3f	Vector3f::Front = Vector3f(0.0F, 0.0F, 1.0F);
	const Vector3f	Vector3f::Up = Vector3f(0.0F, 1.0F, 0.0F);
	const Vector3f	Vector3f::Left = Vector3f(-1.0F, 0.0F, 0.0F);
}