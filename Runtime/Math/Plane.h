#pragma once
#include "Vector3.h"
namespace Joestar
{
	class Plane
	{
	public:
		Vector3f normal;
		float d;
		//define from a normal and a point
		void Define(const Vector3f& nor, const Vector3f& point)
		{
			normal = nor.Normalized();
			d = -Dot(normal, point);
		}
		void Define(const Vector3f& nor, float d1)
		{
			normal = nor.Normalized();
			d = d1;
		}
		Vector3f GetAbsNormal()
		{
			return Vector3f(Abs(normal.x), Abs(normal.y), Abs(normal.z));
		}
	};
}