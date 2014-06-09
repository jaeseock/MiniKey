#ifndef __MINIKEY_CORE_MKQUATERNION_H__
#define __MINIKEY_CORE_MKQUATERNION_H__

//------------------------------------------------------------------------------------------------//
// 사원수
//------------------------------------------------------------------------------------------------//

#include <math.h>
#include "MkCore_MkMat3.h"


class MkQuaternion
{
public:

	//------------------------------------------------------------------------------------------------//

	inline MkQuaternion() { SetIdentity(); }

	inline MkQuaternion(float fw, float fx, float fy, float fz)
	{
		w = fw;
		x = fx;
		y = fy;
		z = fz;
	}

	inline MkQuaternion(const MkQuaternion& q)
	{
		w = q.x;
		x = q.x;
		y = q.y;
		z = q.z;
	}

	inline MkQuaternion(const MkMat3& mat) { FromMat3(mat); }

	inline MkQuaternion(const MkVec3& axis, float radian) { FromAxisRadian(axis, radian); }

	inline MkQuaternion(const MkVec3& x_axis, const MkVec3& y_axis, const MkVec3& z_axis) { FromAxis(x_axis, y_axis, z_axis); }

	//------------------------------------------------------------------------------------------------//

	inline MkQuaternion& operator = (const MkQuaternion& q)
	{
		w = q.x;
		x = q.x;
		y = q.y;
		z = q.z;
		return *this;
	}

	inline MkQuaternion& operator = (const MkMat3& mat)
	{
		FromMat3(mat);
		return *this;
	}

	//------------------------------------------------------------------------------------------------//

	inline bool operator == (const MkQuaternion& q) const
	{
		return ((w == q.w) && (x == q.x) && (y == q.y) && (z == q.z));
	}

	inline bool operator != (const MkQuaternion& q) const
	{
		return ((w != q.w) || (x != q.x) || (y != q.y) || (z != q.z));
	}

	inline MkQuaternion operator + (const MkQuaternion& q) const
	{
		return MkQuaternion(w + q.w, x + q.x, y + q.y, z + q.z);
	}

	inline MkQuaternion operator - (const MkQuaternion& q) const
	{
		return MkQuaternion(w - q.w, x - q.x, y - q.y, z - q.z);
	}

	MkVec3 operator * (const MkVec3& vec) const;

	inline MkQuaternion operator * (const MkQuaternion& q) const
	{
		return MkQuaternion
			(
			w * q.w - x * q.x - y * q.y - z * q.z,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x
			);
	}

	inline MkQuaternion operator * (const float& scalar) const
	{
		return MkQuaternion(scalar * w, scalar * x, scalar * y, scalar * z);
	}

	inline friend MkQuaternion operator * (const float& scalar, const MkQuaternion& q)
	{
		return MkQuaternion(scalar * q.w, scalar * q.x, scalar * q.y, scalar * q.z);
	}

	inline MkQuaternion operator / (const float& scalar) const
	{
		float invScalar = 1.f / scalar;
		return MkQuaternion(w * invScalar, x * invScalar, y * invScalar, z * invScalar); // divided by zero 예외처리 없음
	}

	inline MkQuaternion operator - () const
	{
		return MkQuaternion(-w, -x, -y, -z);
	}

	inline MkQuaternion& operator += (const MkQuaternion& q)
	{
		w += q.w;
		x += q.x;
		y += q.y;
		z += q.z;
		return *this;
	}

	inline MkQuaternion& operator -= (const MkQuaternion& q)
	{
		w -= q.w;
		x -= q.x;
		y -= q.y;
		z -= q.z;
		return *this;
	}

	inline MkQuaternion& operator *= (const MkQuaternion& q)
	{
		float tw = w * q.w - x * q.x - y * q.y - z * q.z;
		float tx = w * q.x + x * q.w + y * q.z - z * q.y;
		float ty = w * q.y + y * q.w + z * q.x - x * q.z;
		float tz = w * q.z + z * q.w + x * q.y - y * q.x;
		w = tw;
		x = tx;
		y = ty;
		z = tz;
		return *this;
	}

	inline MkQuaternion& operator *= (const float& scalar)
	{
		w *= scalar;
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	inline MkQuaternion& operator /= (const float& scalar)
	{
		float invScalar = 1.f / scalar;
		w *= invScalar;
		x *= invScalar;
		y *= invScalar;
		z *= invScalar;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	
	inline void SetIdentity(void)
	{
		w = 1.f;
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	// MkMat3 변환
	void FromMat3(const MkMat3& mat);
	void ToMat3(MkMat3& mat) const;

	inline operator MkMat3()
	{
		MkMat3 mat;
		ToMat3(mat);
		return mat;
	}

	// axis & radian(angle) 변환
	void FromAxisRadian(const MkVec3& axis, float radian);
	void ToAxisRadian(MkVec3& axis, float& radian) const;

	inline void FromRotationX(float radian) { FromAxisRadian(MkVec3::AxisX, radian); }
	inline void FromRotationY(float radian) { FromAxisRadian(MkVec3::AxisY, radian); }
	inline void FromRotationZ(float radian) { FromAxisRadian(MkVec3::AxisZ, radian); }

	// x, y, z axis 변환
	inline void FromAxis(const MkVec3& x_axis, const MkVec3& y_axis, const MkVec3& z_axis)
	{
		MkMat3 mat;
		mat.FromAxis(x_axis, y_axis, z_axis);
		FromMat3(mat);
	}

	inline void GetAxis(MkVec3& x_axis, MkVec3& y_axis, MkVec3& z_axis) const
	{
		MkMat3 mat;
		ToMat3(mat);
		mat.GetAxis(x_axis, y_axis, z_axis);
	}

	// local axis 반환
	MkVec3 GetAxisX(void) const;
	MkVec3 GetAxisY(void) const;
	MkVec3 GetAxisZ(void) const;

	// 길이의 제곱을 구함
	inline float SquaredLength(void) const
	{
		return (w * w + x * x + y * y + z * z);
	}

	// 길이를 구함
	inline float Length(void) const
	{
		return sqrt(SquaredLength());
	}

	// 내적
	inline float DotProduct(const MkQuaternion& q) const
	{
		return (w * q.w + x * q.x + y * q.y + z * q.z);
	}

	// 단위화
	inline float Normalise(void)
	{
		float tlength = Length();
		if (tlength != 0.f)
		{
			float invLength = 1.f / tlength;
			w *= invLength;
			x *= invLength;
			y *= invLength;
			z *= invLength;
		}
		return tlength;
	}

	// 역수
	inline MkQuaternion Inverse(void) const
	{
		float norm = SquaredLength();
		if (norm > 0.f)
		{
			float invNorm = 1.f / norm;
			return MkQuaternion(w * invNorm, -x * invNorm, -y * invNorm, -z * invNorm);
		}
		return Zero; // error
	}

	//------------------------------------------------------------------------------------------------//
	
	// 구면 선형 보간 (spherical linear interpolation)
	static MkQuaternion Slerp(float t, const MkQuaternion& startQ, const MkQuaternion& endQ, bool shortestPath = false);

	// 선형 보간 (normalised linear interpolation)
	static MkQuaternion Nlerp(float t, const MkQuaternion& startQ, const MkQuaternion& endQ, bool shortestPath = false);

	//------------------------------------------------------------------------------------------------//

public:

	float w, x, y, z;

	// 상수
	static const MkQuaternion Zero;
	static const MkQuaternion Identity;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkQuaternion)

//------------------------------------------------------------------------------------------------//

#endif
