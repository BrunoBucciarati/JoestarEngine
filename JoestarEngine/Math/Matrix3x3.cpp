#include "Matrix3x3.h"
#include "Matrix4x4.h"
using namespace std;

namespace Joestar
{
	Matrix3x3f CreateIdentityMatrix3x3f()
	{
		Matrix3x3f temp;
		temp.SetIdentity();
		return temp;
	}

	Matrix3x3f CreateZeroMatrix3x3f()
	{
		Matrix3x3f temp;
		temp.SetZero();
		return temp;
	}

const Matrix3x3f Matrix3x3f::identity = CreateIdentityMatrix3x3f();
const Matrix3x3f Matrix3x3f::zero = CreateZeroMatrix3x3f();


Matrix3x3f& Matrix3x3f::operator = (const Matrix4x4f& other)
{
	m_Data[0] = other.m_Data[0];
	m_Data[1] = other.m_Data[1];
	m_Data[2] = other.m_Data[2];

	m_Data[3] = other.m_Data[4];
	m_Data[4] = other.m_Data[5];
	m_Data[5] = other.m_Data[6];

	m_Data[6] = other.m_Data[8];
	m_Data[7] = other.m_Data[9];
	m_Data[8] = other.m_Data[10];
	return *this;
}

Matrix3x3f::Matrix3x3f(const Matrix4x4f& other)
{
	m_Data[0] = other.m_Data[0];
	m_Data[1] = other.m_Data[1];
	m_Data[2] = other.m_Data[2];

	m_Data[3] = other.m_Data[4];
	m_Data[4] = other.m_Data[5];
	m_Data[5] = other.m_Data[6];

	m_Data[6] = other.m_Data[8];
	m_Data[7] = other.m_Data[9];
	m_Data[8] = other.m_Data[10];
}

Matrix3x3f& Matrix3x3f::SetIdentity()
{
	Get(0, 0) = 1.0F;	Get(0, 1) = 0.0F;	Get(0, 2) = 0.0F;
	Get(1, 0) = 0.0F;	Get(1, 1) = 1.0F;	Get(1, 2) = 0.0F;
	Get(2, 0) = 0.0F;	Get(2, 1) = 0.0F;	Get(2, 2) = 1.0F;
	return *this;
}

Matrix3x3f& Matrix3x3f::SetZero()
{
	Get(0, 0) = 0.0F;	Get(0, 1) = 0.0F;	Get(0, 2) = 0.0F;
	Get(1, 0) = 0.0F;	Get(1, 1) = 0.0F;	Get(1, 2) = 0.0F;
	Get(2, 0) = 0.0F;	Get(2, 1) = 0.0F;	Get(2, 2) = 0.0F;
	return *this;
}

Matrix3x3f& Matrix3x3f::SetOrthoNormalBasis(const Vector3f& inX, const Vector3f& inY, const Vector3f& inZ)
{
	Get(0, 0) = inX[0];	Get(0, 1) = inY[0];	Get(0, 2) = inZ[0];
	Get(1, 0) = inX[1];	Get(1, 1) = inY[1];	Get(1, 2) = inZ[1];
	Get(2, 0) = inX[2];	Get(2, 1) = inY[2];	Get(2, 2) = inZ[2];
	return *this;
}

Matrix3x3f& Matrix3x3f::SetOrthoNormalBasisInverse(const Vector3f& inX, const Vector3f& inY, const Vector3f& inZ)
{
	Get(0, 0) = inX[0];	Get(1, 0) = inY[0];	Get(2, 0) = inZ[0];
	Get(0, 1) = inX[1];	Get(1, 1) = inY[1];	Get(2, 1) = inZ[1];
	Get(0, 2) = inX[2];	Get(1, 2) = inY[2];	Get(2, 2) = inZ[2];
	return *this;
}

Matrix3x3f& Matrix3x3f::SetScale(const Vector3f& inScale)
{
	Get(0, 0) = inScale[0];	Get(0, 1) = 0.0F;			Get(0, 2) = 0.0F;
	Get(1, 0) = 0.0F;			Get(1, 1) = inScale[1];	Get(1, 2) = 0.0F;
	Get(2, 0) = 0.0F;			Get(2, 1) = 0.0F;			Get(2, 2) = inScale[2];
	return *this;
}

bool Matrix3x3f::IsIdentity(float threshold) {
	if (CompareApproximately(Get(0, 0), 1.0f, threshold) && CompareApproximately(Get(0, 1), 0.0f, threshold) && CompareApproximately(Get(0, 2), 0.0f, threshold) &&
		CompareApproximately(Get(1, 0), 0.0f, threshold) && CompareApproximately(Get(1, 1), 1.0f, threshold) && CompareApproximately(Get(1, 2), 0.0f, threshold) &&
		CompareApproximately(Get(2, 0), 0.0f, threshold) && CompareApproximately(Get(2, 1), 0.0f, threshold) && CompareApproximately(Get(2, 2), 1.0f, threshold))
		return true;
	return false;
}


Matrix3x3f& Matrix3x3f::Scale(const Vector3f& inScale)
{
	Get(0, 0) *= inScale[0];
	Get(1, 0) *= inScale[0];
	Get(2, 0) *= inScale[0];

	Get(0, 1) *= inScale[1];
	Get(1, 1) *= inScale[1];
	Get(2, 1) *= inScale[1];

	Get(0, 2) *= inScale[2];
	Get(1, 2) *= inScale[2];
	Get(2, 2) *= inScale[2];
	return *this;
}

float Matrix3x3f::GetDeterminant() const
{
	float fCofactor0 = Get(0, 0) * Get(1, 1) * Get(2, 2);
	float fCofactor1 = Get(0, 1) * Get(1, 2) * Get(2, 0);
	float fCofactor2 = Get(0, 2) * Get(1, 0) * Get(2, 1);

	float fCofactor3 = Get(0, 2) * Get(1, 1) * Get(2, 0);
	float fCofactor4 = Get(0, 1) * Get(1, 0) * Get(2, 2);
	float fCofactor5 = Get(0, 0) * Get(1, 2) * Get(2, 1);

	return fCofactor0 + fCofactor1 + fCofactor2 - fCofactor3 - fCofactor4 - fCofactor5;
}

Matrix3x3f& Matrix3x3f::Transpose()
{
	swap(Get(0, 1), Get(1, 0));
	swap(Get(0, 2), Get(2, 0));
	swap(Get(2, 1), Get(1, 2));
	return *this;
}

Matrix3x3f& Matrix3x3f::operator *= (float f)
{
	for (int i = 0; i < 9; i++)
		m_Data[i] *= f;
	return *this;
}

Matrix3x3f& Matrix3x3f::operator *= (const Matrix3x3f& inM)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		float v[3] = { Get(i, 0), Get(i, 1), Get(i, 2) };
		Get(i, 0) = v[0] * inM.Get(0, 0) + v[1] * inM.Get(1, 0) + v[2] * inM.Get(2, 0);
		Get(i, 1) = v[0] * inM.Get(0, 1) + v[1] * inM.Get(1, 1) + v[2] * inM.Get(2, 1);
		Get(i, 2) = v[0] * inM.Get(0, 2) + v[1] * inM.Get(1, 2) + v[2] * inM.Get(2, 2);
	}
	return *this;
}

Matrix3x3f& Matrix3x3f::operator *= (const Matrix4x4f& inM)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		float v[3] = { Get(i, 0), Get(i, 1), Get(i, 2) };
		Get(i, 0) = v[0] * inM.Get(0, 0) + v[1] * inM.Get(1, 0) + v[2] * inM.Get(2, 0);
		Get(i, 1) = v[0] * inM.Get(0, 1) + v[1] * inM.Get(1, 1) + v[2] * inM.Get(2, 1);
		Get(i, 2) = v[0] * inM.Get(0, 2) + v[1] * inM.Get(1, 2) + v[2] * inM.Get(2, 2);
	}
	return *this;
}

inline void MakePositive(Vector3f& euler)
{
	const float negativeFlip = -0.0001F;
	const float positiveFlip = (PI * 2.0F) - 0.0001F;

	if (euler.x < negativeFlip)
		euler.x += 2.0 * PI;
	else if (euler.x > positiveFlip)
		euler.x -= 2.0 * PI;

	if (euler.y < negativeFlip)
		euler.y += 2.0 * PI;
	else if (euler.y > positiveFlip)
		euler.y -= 2.0 * PI;

	if (euler.z < negativeFlip)
		euler.z += 2.0 * PI;
	else if (euler.z > positiveFlip)
		euler.z -= 2.0 * PI;
}

inline void SanitizeEuler(Vector3f& euler)
{
	MakePositive(euler);
}

void EulerToMatrix(const Vector3f& v, Matrix3x3f& matrix)
{
	float cx = cos(v.x);
	float sx = sin(v.x);
	float cy = cos(v.y);
	float sy = sin(v.y);
	float cz = cos(v.z);
	float sz = sin(v.z);

	matrix.Get(0, 0) = cy * cz + sx * sy * sz;
	matrix.Get(0, 1) = cz * sx * sy - cy * sz;
	matrix.Get(0, 2) = cx * sy;

	matrix.Get(1, 0) = cx * sz;
	matrix.Get(1, 1) = cx * cz;
	matrix.Get(1, 2) = -sx;

	matrix.Get(2, 0) = -cz * sy + cy * sx * sz;
	matrix.Get(2, 1) = cy * cz * sx + sy * sz;
	matrix.Get(2, 2) = cx * cy;
}

void fromToRotation(Vector3f&, Vector3f&, float mtx[3][3]);

Matrix3x3f& Matrix3x3f::SetFromToRotation(const Vector3f& from, const Vector3f& to)
{
	float mtx[3][3];
	Vector3f f = from;
	Vector3f t = to;
	fromToRotation(f, t, mtx);
	Get(0, 0) = mtx[0][0];	Get(0, 1) = mtx[0][1];	Get(0, 2) = mtx[0][2];
	Get(1, 0) = mtx[1][0];	Get(1, 1) = mtx[1][1];	Get(1, 2) = mtx[1][2];
	Get(2, 0) = mtx[2][0];	Get(2, 1) = mtx[2][1];	Get(2, 2) = mtx[2][2];
	return *this;
}

/// This is YXZ euler conversion
bool MatrixToEuler(const Matrix3x3f& matrix, Vector3f& v)
{
	// from http://www.geometrictools.com/Documentation/EulerAngles.pdf
	// YXZ order
	if (matrix.Get(1, 2) < 0.999F) // some fudge for imprecision
	{
		if (matrix.Get(1, 2) > -0.999F) // some fudge for imprecision
		{
			v.x = asin(-matrix.Get(1, 2));
			v.y = atan2(matrix.Get(0, 2), matrix.Get(2, 2));
			v.z = atan2(matrix.Get(1, 0), matrix.Get(1, 1));
			SanitizeEuler(v);
			return true;
		}
		else
		{
			// WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
			v.x = PI * 0.5F;
			v.y = atan2(matrix.Get(0, 1), matrix.Get(0, 0));
			v.z = 0.0F;
			SanitizeEuler(v);

			return false;
		}
	}
	else
	{
		// WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
		v.x = -PI * 0.5F;
		v.y = atan2(-matrix.Get(0, 1), matrix.Get(0, 0));
		v.z = 0.0F;
		SanitizeEuler(v);
		return false;
	}
}

void fromToRotation(Vector3f& from, Vector3f& to, float mtx[3][3])
{
	Vector3f v;
	float e, h;
	v = Cross(from, to);
	e = Dot(from, to);
	if (e > 1.0 - EPSILON)     /* "from" almost or equal to "to"-vector? */
	{
		/* return identity */
		mtx[0][0] = 1.0; mtx[0][1] = 0.0; mtx[0][2] = 0.0;
		mtx[1][0] = 0.0; mtx[1][1] = 1.0; mtx[1][2] = 0.0;
		mtx[2][0] = 0.0; mtx[2][1] = 0.0; mtx[2][2] = 1.0;
	}
	else if (e < -1.0 + EPSILON) /* "from" almost or equal to negated "to"? */
	{
		Vector3f up, left;
		float invlen;
		float fxx, fyy, fzz, fxy, fxz, fyz;
		float uxx, uyy, uzz, uxy, uxz, uyz;
		float lxx, lyy, lzz, lxy, lxz, lyz;
		/* left=CROSS(from, (1,0,0)) */
		left[0] = 0.0; left[1] = from[2]; left[2] = -from[1];
		if (Dot(left, left) < EPSILON) /* was left=CROSS(from,(1,0,0)) a good choice? */
		{
			/* here we now that left = CROSS(from, (1,0,0)) will be a good choice */
			left[0] = -from[2]; left[1] = 0.0; left[2] = from[0];
		}
		/* normalize "left" */
		invlen = 1.0f / sqrt(Dot(left, left));
		left[0] *= invlen;
		left[1] *= invlen;
		left[2] *= invlen;
		up = Cross(left, from);
		/* now we have a coordinate system, i.e., a basis;    */
		/* M=(from, up, left), and we want to rotate to:      */
		/* N=(-from, up, -left). This is done with the matrix:*/
		/* N*M^T where M^T is the transpose of M              */
		fxx = -from[0] * from[0]; fyy = -from[1] * from[1]; fzz = -from[2] * from[2];
		fxy = -from[0] * from[1]; fxz = -from[0] * from[2]; fyz = -from[1] * from[2];

		uxx = up[0] * up[0]; uyy = up[1] * up[1]; uzz = up[2] * up[2];
		uxy = up[0] * up[1]; uxz = up[0] * up[2]; uyz = up[1] * up[2];

		lxx = -left[0] * left[0]; lyy = -left[1] * left[1]; lzz = -left[2] * left[2];
		lxy = -left[0] * left[1]; lxz = -left[0] * left[2]; lyz = -left[1] * left[2];
		/* symmetric matrix */
		mtx[0][0] = fxx + uxx + lxx; mtx[0][1] = fxy + uxy + lxy; mtx[0][2] = fxz + uxz + lxz;
		mtx[1][0] = mtx[0][1];   mtx[1][1] = fyy + uyy + lyy; mtx[1][2] = fyz + uyz + lyz;
		mtx[2][0] = mtx[0][2];   mtx[2][1] = mtx[1][2];   mtx[2][2] = fzz + uzz + lzz;
	}
	else  /* the most common case, unless "from"="to", or "from"=-"to" */
	{
		/* ...otherwise use this hand optimized version (9 mults less) */
		float hvx, hvz, hvxy, hvxz, hvyz;
		h = (1.0f - e) / Dot(v, v);
		hvx = h * v[0];
		hvz = h * v[2];
		hvxy = hvx * v[1];
		hvxz = hvx * v[2];
		hvyz = hvz * v[1];
		mtx[0][0] = e + hvx * v[0]; mtx[0][1] = hvxy - v[2];     mtx[0][2] = hvxz + v[1];
		mtx[1][0] = hvxy + v[2];  mtx[1][1] = e + h * v[1] * v[1]; mtx[1][2] = hvyz - v[0];
		mtx[2][0] = hvxz - v[1];  mtx[2][1] = hvyz + v[0];     mtx[2][2] = e + hvz * v[2];
	}
}

}