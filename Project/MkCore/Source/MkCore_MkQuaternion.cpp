
#include "MkCore_MkQuaternion.h"


//------------------------------------------------------------------------------------------------//

MkVec3 MkQuaternion::operator* (const MkVec3& vec) const
{
	MkVec3 uv, uuv;
	MkVec3 qvec(x, y, z);
	uv = qvec.CrossProduct(vec);
	uuv = qvec.CrossProduct(uv);
	uv *= (2.f * w);
	uuv *= 2.f;
	return (vec + uv + uuv);
	//return (vec - uv + uuv); // CCW
}

void MkQuaternion::FromMat3(const MkMat3& mat)
{
	float trace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
	if (trace > 0.f)
	{
		// |w| > 1/2, may as well choose w > 1/2
		float root = sqrt(trace + 1.0f); // 2w
		w = 0.5f * root;
		root = 0.5f / root; // 1/(4w)
		x = (mat.m[2][1] - mat.m[1][2]) * root;
		y = (mat.m[0][2] - mat.m[2][0]) * root;
		z = (mat.m[1][0] - mat.m[0][1]) * root;
	}
	else
	{
		// |w| <= 1/2
		static int s_next[3] = {1, 2, 0};

		int i = 0;
		if (mat.m[1][1] > mat.m[0][0]) i = 1;
		if (mat.m[2][2] > mat.m[i][i]) i = 2;
		int j = s_next[i];
		int k = s_next[j];

		float root = sqrt(mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + 1.f);
		float* apkQuat[3] = {&x, &y, &z};
		*apkQuat[i] = 0.5f * root;
		root = 0.5f / root;
		w = (mat.m[k][j] - mat.m[j][k]) * root;
		*apkQuat[j] = (mat.m[j][i] + mat.m[i][j]) * root;
		*apkQuat[k] = (mat.m[k][i] + mat.m[i][k]) * root;
	}
}

void MkQuaternion::ToMat3(MkMat3& mat) const
{
	float tx  = x + x;
	float ty  = y + y;
	float tz  = z + z;
	float twx = tx * w;
	float twy = ty * w;
	float twz = tz * w;
	float txx = tx * x;
	float txy = ty * x;
	float txz = tz * x;
	float tyy = ty * y;
	float tyz = tz * y;
	float tzz = tz * z;

	mat.m[0][0] = 1.f - (tyy + tzz);
	mat.m[0][1] = txy - twz;
	mat.m[0][2] = txz + twy;
	mat.m[1][0] = txy + twz;
	mat.m[1][1] = 1.f - (txx + tzz);
	mat.m[1][2] = tyz - twx;
	mat.m[2][0] = txz - twy;
	mat.m[2][1] = tyz + twx;
	mat.m[2][2] = 1.f - (txx + tyy);
}

void MkQuaternion::FromAxisRadian(const MkVec3& axis, float radian)
{
	// q = cos(A/2) + sin(A/2) * (x*i + y*j + z*k)
	float halfAngle = 0.5f * radian;
	//float halfAngle = -0.5f * radian; // CCW
	float s = MkAngleOp::Sin(halfAngle);
	w = MkAngleOp::Cos(halfAngle);
	x = s * axis.x;
	y = s * axis.y;
	z = s * axis.z;
}

void MkQuaternion::ToAxisRadian(MkVec3& axis, float& radian) const
{
	float sqrLength = x*x + y*y + z*z;
	if (sqrLength > 0.f)
	{
		radian = 2.f * MkAngleOp::ACos(w);
		//radian = -2.f * MkAngleOp::ACos(w); // CCW
		float invLength = 1.f / sqrt(sqrLength);
		axis.x = x * invLength;
		axis.y = y * invLength;
		axis.z = z * invLength;
	}
	else
	{
		radian = 0.f;
		axis.x = 1.f;
		axis.y = 0.f;
		axis.z = 0.f;
	}
}

MkVec3 MkQuaternion::GetAxisX(void) const
{
	float ty = 2.f * y;
	float tz = 2.f * z;
	float twy = ty * w;
	float twz = tz * w;
	float txy = ty * x;
	float txz = tz * x;
	float tyy = ty * y;
	float tzz = tz * z;
	return MkVec3(1.f - (tyy + tzz), txy + twz, txz - twy);
}

MkVec3 MkQuaternion::GetAxisY(void) const
{
	float tx = 2.f * x;
	float ty = 2.f * y;
	float tz = 2.f * z;
	float twx = tx * w;
	float twz = tz * w;
	float txx = tx * x;
	float txy = ty * x;
	float tyz = tz * y;
	float tzz = tz * z;
	return MkVec3(txy - twz, 1.f - (txx + tzz), tyz + twx);
}

MkVec3 MkQuaternion::GetAxisZ(void) const
{
	float tx = 2.f * x;
	float ty = 2.f * y;
	float tz = 2.f * z;
	float twx = tx * w;
	float twy = ty * w;
	float txx = tx * x;
	float txz = tz * x;
	float tyy = ty * y;
	float tyz = tz * y;
	return MkVec3(txz + twy, tyz - twx, 1.f - (txx + tyy));
}

//------------------------------------------------------------------------------------------------//

MkQuaternion MkQuaternion::Slerp(float t, const MkQuaternion& startQ, const MkQuaternion& endQ, bool shortestPath)
{
	float dotValue = startQ.DotProduct(endQ);
	MkQuaternion tmpQ;
	if ((dotValue < 0.f) && shortestPath)
	{
		dotValue = -dotValue;
		tmpQ = -endQ;
	}
	else
	{
		tmpQ = endQ;
	}

	if (abs(dotValue) < 1.f)
	{
		float sinValue = sqrt(1.f - dotValue * dotValue);
		float angle = atan2(sinValue, dotValue);
		float invSin = 1.f / sinValue;
		float coeff0 = sin((1.f - t) * angle) * invSin;
		float coeff1 = sin(t * angle) * invSin;
		return (coeff0 * startQ + coeff1 * tmpQ);
	}

	MkQuaternion liQ = (1.f - t) * startQ + t * tmpQ;
	liQ.Normalize();
	return liQ;
}

MkQuaternion MkQuaternion::Nlerp(float t, const MkQuaternion& startQ, const MkQuaternion& endQ, bool shortestPath)
{
	MkQuaternion result;
	if ((startQ.DotProduct(endQ) < 0.f) && shortestPath)
	{
		result = startQ + t * ((-endQ) - startQ);
	}
	else
	{
		result = startQ + t * (endQ - startQ);
	}
	result.Normalize();
	return result;
}


//------------------------------------------------------------------------------------------------//
