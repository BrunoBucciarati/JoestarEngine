#pragma once
#include "BoundingBox.h"
#include "Vector3.h"
namespace Joestar
{
	class AABB : public BoundingBox
	{
	public:
		Vector3f min;
		Vector3f max;
		AABB()
		{
			min = FLT_MAX;
			max = -FLT_MAX;
		}

		void Merge(Vector3f p)
		{
			min = Min(min, p);
			max = Max(max, p);
		}

		bool IsValid() const
		{
			return max.x > min.x && max.y > min.y && max.z > min.z;
		}

		Vector3f Center() const
		{
			return 0.5F * (min + max);
		}

		Vector3f Length() const
		{
			return 0.5F * (max - min);
		}

		void Define(Vector3f center, Vector3f length)
		{
			min = center - length;
			max = center + length;
		}
	};
}