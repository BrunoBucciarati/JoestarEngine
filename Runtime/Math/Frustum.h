#pragma once
#include "Plane.h"
#include "Matrix4x4.h"
#include "AABB.h"
namespace Joestar
{
	enum FrustumPlane
	{
		PLANE_LEFT = 0,
		PLANE_RIGHT,
		PLANE_BOTTOM,
		PLANE_TOP,
		PLANE_NEAR,
		PLANE_FAR
	};
	class Frustum
	{
	public:
		Plane planes[6];

		void DefineFromViewProj(const Matrix4x4f& viewProj);

		Intersection IsInside(const AABB& aabb);
	};
}