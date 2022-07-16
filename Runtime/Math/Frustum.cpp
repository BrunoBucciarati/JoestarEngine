#include "Frustum.h"
#include "Vector3.h"
#include "../Platform/Platform.h"

namespace Joestar {
	void Frustum::DefineFromViewProj(const Matrix4x4f& viewProj)
	{
		planes[PLANE_LEFT].Define(Vector3f(viewProj[3] + viewProj[0], viewProj[4] + viewProj[7], viewProj[8] + viewProj[11]),
			viewProj[12] + viewProj[15]);
		planes[PLANE_RIGHT].Define(Vector3f(viewProj[3] - viewProj[0], -viewProj[4] + viewProj[7], -viewProj[8] + viewProj[11]),
			-viewProj[12] + viewProj[15]);
		planes[PLANE_BOTTOM].Define(Vector3f(viewProj[3] + viewProj[1], viewProj[5] + viewProj[7], viewProj[9] + viewProj[11]),
			viewProj[13] + viewProj[15]);
		planes[PLANE_TOP].Define(Vector3f(viewProj[3] - viewProj[1], -viewProj[5] + viewProj[7], -viewProj[9] + viewProj[11]),
			-viewProj[13] + viewProj[15]);
		planes[PLANE_NEAR].Define(Vector3f(viewProj[2], viewProj[6], viewProj[10]),
			viewProj[14]);
		planes[PLANE_FAR].Define(Vector3f(viewProj[3] - viewProj[1], -viewProj[6] + viewProj[7], -viewProj[10] + viewProj[11]),
			-viewProj[14] + viewProj[15]);
	}


	Intersection Frustum::IsInside(const AABB& aabb)
	{
		Vector3f center = aabb.Center();
		Vector3f edge = center - aabb.min;
		bool allInside = true;
		for (U32 i = 0; i < 6; ++i)
		{
			float dist = Dot(planes[i].normal, center) + planes[i].d;
			float absDist = Dot(planes[i].GetAbsNormal(), edge);
			if (dist < -absDist)
			{
				return Intersection::OUTSIDE;
			}
			else if (dist < absDist)
			{
				allInside = false;
			}
		}
		return allInside ? Intersection::INSIDE : Intersection::INTERSECT;
	}
}