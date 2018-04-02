#ifndef __MINIKEY_CORE_MKVEC3_H__
#define __MINIKEY_CORE_MKVEC3_H__

//------------------------------------------------------------------------------------------------//
// float 기반의 vector(x, y, z)
//------------------------------------------------------------------------------------------------//

#include <math.h>
#include "MkCore_MkContainerDefinition.h"
#include "MkCore_MkFloatOp.h"


class MkVec3
{
public:

	enum eAxis
	{
		eNone = -1,
		ePositiveX,
		ePositiveY,
		ePositiveZ,
		eNegativeX,
		eNegativeY,
		eNegativeZ
	};

public:

	//------------------------------------------------------------------------------------------------//
	// 생성자
	//------------------------------------------------------------------------------------------------//

	inline MkVec3() { Clear(); }

	inline MkVec3(const float& fx, const float& fy, const float& fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

	inline MkVec3(const double& fx, const double& fy, const double& fz)
	{
		x = static_cast<float>(fx);
		y = static_cast<float>(fy);
		z = static_cast<float>(fz);
	}

	inline MkVec3(const MkVec3& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}

	inline MkVec3& operator = (const MkVec3& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	// 연산자 재정의
	//------------------------------------------------------------------------------------------------//

	inline bool operator == (const MkVec3& vec) const { return ((x == vec.x) && (y == vec.y) && (z == vec.z)); }
	inline bool operator != (const MkVec3& vec) const { return ((x != vec.x) || (y != vec.y) || (z != vec.z)); }

	inline MkVec3 operator + (const MkVec3& vec) const { return MkVec3(x + vec.x, y + vec.y, z + vec.z); }
	inline MkVec3 operator - (const MkVec3& vec) const { return MkVec3(x - vec.x, y - vec.y, z - vec.z); }
	inline MkVec3 operator * (const MkVec3& vec) const { return MkVec3(x * vec.x, y * vec.y, z * vec.z); } // 원칙적으로는 존재하지 않는 공식이지만 계산 편의를 위해 추가
	inline MkVec3 operator * (const float& value) const { return MkVec3(x * value, y * value, z * value); }
	inline friend MkVec3 operator * (const float& value, const MkVec3& vec) { return MkVec3(value * vec.x, value * vec.y, value * vec.z); }
	inline MkVec3 operator / (const float& value) const { return MkVec3(x / value, y / value, z / value); }
	inline MkVec3 operator - () const { return MkVec3(-x, -y, -z); }

	inline MkVec3& operator += (const MkVec3& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}

	inline MkVec3& operator -= (const MkVec3& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}

	inline MkVec3& operator *= (const float& value)
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	inline MkVec3& operator /= (const float& value)
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	// 일반 메소드
	//------------------------------------------------------------------------------------------------//

	// 초기화
	inline void Clear(void)
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	// 길이의 제곱을 구함
	inline float SquaredLength(void) const { return (x * x + y * y + z * z); }

	// 길이를 구함
	inline float Length(void) const { return sqrtf(SquaredLength()); }

	// 내적
	inline float DotProduct(const MkVec3& vec) const { return (x * vec.x + y * vec.y + z * vec.z); }

	// 단위벡터화
	inline float Normalize(void)
	{
		float length = Length();
		if (length != 0.f)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return length;
	}

	// vec과 비교해 작은쪽을 저장
	inline void CompareAndKeepMin(const MkVec3& vec)
	{
		if (x > vec.x) x = vec.x;
		if (y > vec.y) y = vec.y;
		if (z > vec.z) z = vec.z;
	}

	// vec과 비교해 큰쪽을 저장
	inline void CompareAndKeepMax(const MkVec3& vec)
	{
		if (x < vec.x) x = vec.x;
		if (y < vec.y) y = vec.y;
		if (z < vec.z) z = vec.z;
	}

	// 외적
	inline MkVec3 CrossProduct(const MkVec3& vec) const
	{
		MkVec3 tmp;
		tmp.x = y * vec.z - z * vec.y;
		tmp.y = z * vec.x - x * vec.z;
		tmp.z = x * vec.y - y * vec.x;
		return tmp;
	}

	// 진행벡터라 가정하고 업벡터와 함께 우/좌방향 벡터를 만듬(좌표계에 따라 다름)
	inline MkVec3 GetPivotVector(const MkVec3& upVec) const
	{
		MkVec3 tmp = CrossProduct(upVec);
		tmp.Normalize();
		return tmp;
	}

	// 가장 큰 축을 결정
	inline eAxis GetGreatestAxis(void) const
	{
		float ax = fabsf(x);
		float ay = fabsf(y);
		float az = fabsf(z);
		if ((ax >= ay) && (ax >= az)) return (x >= 0.f) ? ePositiveX : eNegativeX;
		if ((ay >= ax) && (ay >= az)) return (y >= 0.f) ? ePositiveY : eNegativeY;
		if ((az >= ax) && (az >= ay)) return (z >= 0.f) ? ePositiveZ : eNegativeZ;
		return eNone;
	}

	// 직교벡터를 구함
	inline MkVec3 Perpendicular(void) const
	{
		MkVec3 perp, a;
		switch (GetGreatestAxis())
		{
		case ePositiveX: a.y = 1.f; break;
		case ePositiveY: a.z = 1.f; break;
		case ePositiveZ: a.x = 1.f; break;
		case eNegativeX: a.y = -1.f; break;
		case eNegativeY: a.z = -1.f; break;
		case eNegativeZ: a.x = -1.f; break;
		}
		perp = CrossProduct(a);
		if (perp.SquaredLength() != 0.f)
		{
			perp = CrossProduct(perp);
		}
		perp.Normalize();
		return perp;
	}

	// normal을 축으로 반사벡터를 구함
	// weight(0 ~ 1) : 1에 가까울수록 정반사
	inline MkVec3 Reflect(const MkVec3& normal, float weight = 1.f) const
	{
		return MkVec3(*this - (DotProduct(normal) * (1.f + weight) * normal));
	}

	// grid 를 기준으로 내림
	// ex> (4.8f, -2.3f, 1.f).SnapToLowerBound(2.f) -> (4.f, -4.f, 0.f)
	inline void SnapToLowerBound(float grid)
	{
		x = MkFloatOp::SnapToLowerBound(x, grid);
		y = MkFloatOp::SnapToLowerBound(y, grid);
		z = MkFloatOp::SnapToLowerBound(z, grid);
	}

	// grid 를 기준으로 올림
	// ex> (4.8f, -2.3f, 1.f).SnapToUpperBound(2.f) -> (6.f, -2.f, 2.f)
	inline void SnapToUpperBound(float grid)
	{
		x = MkFloatOp::SnapToUpperBound(x, grid);
		y = MkFloatOp::SnapToUpperBound(y, grid);
		z = MkFloatOp::SnapToUpperBound(z, grid);
	}

	// zero 벡터인지 판별
	inline bool IsZero(void) const
	{
		return ((x == 0.f) && (y == 0.f) && (z == 0.f));
	}

	// zero 벡터에 가까운지 판별
	inline bool CloseToZero(void) const
	{
		return (MkFloatOp::CloseToZero(x) && MkFloatOp::CloseToZero(y) && MkFloatOp::CloseToZero(z));
	}

	// target 벡터에 가까운지 판별
	inline bool CloseToNear(const MkVec3& target) const
	{
		return (MkFloatOp::CloseToNear(x, target.x) && MkFloatOp::CloseToNear(y, target.y) && MkFloatOp::CloseToNear(z, target.z));
	}

public:

	float x, y, z;

	// 상수
	static const MkVec3 Zero;
	static const MkVec3 AxisX;
	static const MkVec3 AxisY;
	static const MkVec3 AxisZ;
	static const MkVec3 One;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkVec3)

//------------------------------------------------------------------------------------------------//

#endif
