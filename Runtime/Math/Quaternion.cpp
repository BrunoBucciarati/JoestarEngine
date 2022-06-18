#include "Quaternion.h"
#include "../Platform/Platform.h"

namespace Joestar {

    bool Quaternionf::FromLookRotation(const Vector3f& direction, const Vector3f& up)
    {
        Quaternionf ret;
        Vector3f forward = Normalize(direction);

        Vector3f v = Cross(forward, up); 
        // If direction & up are parallel and crossproduct becomes zero, use FromRotationTo() fallback
        if (v.LengthSquared() >= M_EPSILON)
        {
            v.Normalize();
            Vector3f up = Cross(v, forward);
            Vector3f right = Cross(up, forward);    
            ret.FromAxes(right, up, forward);
        }
        else
            ret.FromRotationTo(Vector3f::Front, forward);

        if (!ret.IsNaN())
        {
            (*this) = ret;
            return true;
        }
        else
            return false;
    }


    void Quaternionf::FromAxes(const Vector3f& xAxis, const Vector3f& yAxis, const Vector3f& zAxis)
    {
        Matrix3x3f matrix(
            xAxis.x, yAxis.x, zAxis.x,
            xAxis.y, yAxis.y, zAxis.y,
            xAxis.z, yAxis.z, zAxis.z
        );

        FromRotationMatrix(matrix);
    }

    void Quaternionf::FromRotationMatrix(const Matrix3x3f& matrix)
    {
        float t = matrix.Get(0, 0) + matrix.Get(1, 1) + matrix.Get(2, 2);

        if (t > 0.0f)
        {
            float invS = 0.5f / Sqrtf(1.0f + t);

            x = (matrix.Get(2,1) - matrix.Get(1, 2)) * invS;
            y = (matrix.Get(0,2) - matrix.Get(2, 0)) * invS;
            z = (matrix.Get(1,0) - matrix.Get(0,1)) * invS;
            w = 0.25f / invS;
        }
        else
        {
            if (matrix.Get(0,0) > matrix.Get(1,1)&& matrix.Get(0,0) > matrix.Get(2,2))
            {
                float invS = 0.5f / sqrtf(1.0f + matrix.Get(0, 0) - matrix.Get(1,1) - matrix.Get(2,2));

                x = 0.25f / invS;
                y = (matrix.Get(0,1) + matrix.Get(1,0)) * invS;
                z = (matrix.Get(2,0) + matrix.Get(0,2)) * invS;
                w = (matrix.Get(2,1) - matrix.Get(1,2)) * invS;
            }
            else if (matrix.Get(1,1) > matrix.Get(2,2))
            {
                float invS = 0.5f / sqrtf(1.0f + matrix.Get(1,1) - matrix.Get(0, 0) - matrix.Get(2,2));

                x = (matrix.Get(0,1) + matrix.Get(1,0)) * invS;
                y = 0.25f / invS;
                z = (matrix.Get(1,2) + matrix.Get(2,1)) * invS;
                w = (matrix.Get(0,2) - matrix.Get(2,0)) * invS;
            }
            else
            {
                float invS = 0.5f / sqrtf(1.0f + matrix.Get(2,2) - matrix.Get(0, 0) - matrix.Get(1,1));

                x = (matrix.Get(0,2) + matrix.Get(2,0)) * invS;
                y = (matrix.Get(1,2) + matrix.Get(2,1)) * invS;
                z = 0.25f / invS;
                w = (matrix.Get(1,0) - matrix.Get(0,1)) * invS;
            }
        }
    }



    void Quaternionf::FromRotationTo(const Vector3f& start, const Vector3f& end)
    {
        Vector3f normStart = Normalize(start);
        Vector3f normEnd = end.Normalized();
        float d = Dot(normStart, normEnd);

        if (d > -1.0f + M_EPSILON)
        {
            Vector3f c = Cross(normStart, normEnd);
            float s = sqrtf((1.0f + d) * 2.0f);
            float invS = 1.0f / s;

            x = c.x * invS;
            y = c.y * invS;
            z = c.z * invS;
            w = 0.5f * s;
        }
        else
        {
            Vector3f axis = Cross(Vector3f::Right, normStart);
            if (axis.Length() < M_EPSILON)
                axis = Cross(Vector3f::Up, normStart);

            AxisAngleToQuaternion(axis, 180.f);
        }
    }

    Matrix3x3f Quaternionf::RotationMatrix() const
    {
        return Matrix3x3f(
            1.0f - 2.0f * y * y - 2.0f * z * z,
            2.0f * x * y - 2.0f * w * z,
            2.0f * x * z + 2.0f * w * y,
            2.0f * x * y + 2.0f * w * z,
            1.0f - 2.0f * x * x - 2.0f * z * z,
            2.0f * y * z - 2.0f * w * x,
            2.0f * x * z - 2.0f * w * y,
            2.0f * y * z + 2.0f * w * x,
            1.0f - 2.0f * x * x - 2.0f * y * y
        );
    }
}