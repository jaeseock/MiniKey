#ifndef __MINIKEY_CORE_MKVEC2_H__
#define __MINIKEY_CORE_MKVEC2_H__

//------------------------------------------------------------------------------------------------//
// float 기반의 vector(x, y)
//------------------------------------------------------------------------------------------------//

#include <math.h>
#include "MkCore_MkContainerDefinition.h"
#include "MkCore_MkFloatOp.h"


class MkVec2
{
public:

	//------------------------------------------------------------------------------------------------//
	// 생성자
	//------------------------------------------------------------------------------------------------//

	inline MkVec2() { Clear(); }

	inline MkVec2(const float& fx, const float& fy)
	{
		x = fx;
		y = fy;
	}

	inline MkVec2(const double& fx, const double& fy)
	{
		x = static_cast<float>(fx);
		y = static_cast<float>(fy);
	}

	inline MkVec2(const MkVec2& vec)
	{
		x = vec.x;
		y = vec.y;
	}

	inline MkVec2& operator = (const MkVec2& vec)
	{
		x = vec.x;
		y = vec.y;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	// 연산자 재정의
	//------------------------------------------------------------------------------------------------//

	inline bool operator == (const MkVec2& vec) const { return ((x == vec.x) && (y == vec.y)); }
	inline bool operator != (const MkVec2& vec) const { return ((x != vec.x) || (y != vec.y)); }

	inline MkVec2 operator + (const MkVec2& vec) const { return MkVec2(x + vec.x, y + vec.y); }
	inline MkVec2 operator - (const MkVec2& vec) const { return MkVec2(x - vec.x, y - vec.y); }
	inline MkVec2 operator * (const MkVec2& vec) const { return MkVec2(x * vec.x, y * vec.y); } // 원칙적으로는 존재하지 않는 공식이지만 계산 편의를 위해 추가
	inline MkVec2 operator * (const float& value) const { return MkVec2(x * value, y * value); }
	inline friend MkVec2 operator * (const float& value, const MkVec2& vec) { return MkVec2(value * vec.x, value * vec.y); }
	inline MkVec2 operator / (const float& value) const { return MkVec2(x / value, y / value); } // divided by zero 예외처리 없음
	inline MkVec2 operator - () const { return MkVec2(-x, -y); }

	inline MkVec2& operator += (const MkVec2& vec)
	{
		x += vec.x;
		y += vec.y;
		return *this;
	}

	inline MkVec2& operator -= (const MkVec2& vec)
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	}

	inline MkVec2& operator *= (const float& value)
	{
		x *= value;
		y *= value;
		return *this;
	}

	inline MkVec2& operator /= (const float& value)
	{
		x /= value; // divided by zero 예외처리 없음
		y /= value;
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
	}

	// 길이의 제곱을 구함
	inline float SquaredLength(void) const { return (x * x + y * y); }

	// 길이를 구함
	inline float Length(void) const { return sqrtf(SquaredLength()); }

	// 내적
	inline float DotProduct(const MkVec2& vec) const { return (x * vec.x + y * vec.y); }

	// 단위벡터화
	inline float Normalize(void)
	{
		float length = Length();
		if (length != 0.f)
		{
			x /= length;
			y /= length;
		}
		return length;
	}

	// vec과 비교해 작은쪽을 저장
	inline void CompareAndKeepMin(const MkVec2& vec)
	{
		if (x > vec.x) x = vec.x;
		if (y > vec.y) y = vec.y;
	}

	// vec과 비교해 큰쪽을 저장
	inline void CompareAndKeepMax(const MkVec2& vec)
	{
		if (x < vec.x) x = vec.x;
		if (y < vec.y) y = vec.y;
	}

	// 직교벡터를 구함
	inline MkVec2 Perpendicular(void) const { return MkVec2(-y, x); }

	// 외적
	inline float CrossProduct(const MkVec2& vec) const { return (x * vec.y - y * vec.x); }

	// normal을 축으로 반사벡터를 구함
	// weight(0 ~ 1) : 1에 가까울수록 정반사
	inline MkVec2 Reflect(const MkVec2& normal, float weight = 1.f) const { return MkVec2(*this - (DotProduct(normal) * (1.f + weight) * normal)); }

	// grid 를 기준으로 내림
	// ex> (4.8f, -2.3f).SnapToLowerBound(2.f) -> (4.f, -4.f)
	inline void SnapToLowerBound(float grid)
	{
		x = MkFloatOp::SnapToLowerBound(x, grid);
		y = MkFloatOp::SnapToLowerBound(y, grid);
	}

	// grid 를 기준으로 올림
	// ex> (4.8f, -2.3f).SnapToUpperBound(2.f) -> (6.f, -2.f)
	inline void SnapToUpperBound(float grid)
	{
		x = MkFloatOp::SnapToUpperBound(x, grid);
		y = MkFloatOp::SnapToUpperBound(y, grid);
	}

	// zero 벡터인지 판별
	inline bool IsZero(void) const
	{
		return ((x == 0.f) && (y == 0.f));
	}

	// zero 벡터에 가까운지 판별
	inline bool CloseToZero(void) const { return (MkFloatOp::CloseToZero(x) && MkFloatOp::CloseToZero(y)); }

	// target 벡터에 가까운지 판별
	inline bool CloseToNear(const MkVec2& target) const { return (MkFloatOp::CloseToNear(x, target.x) && MkFloatOp::CloseToNear(y, target.y)); }

public:

	float x, y;

	// 상수
	static const MkVec2 Zero;
	static const MkVec2 AxisX;
	static const MkVec2 AxisY;
	static const MkVec2 One;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkVec2)

//------------------------------------------------------------------------------------------------//

#endif
