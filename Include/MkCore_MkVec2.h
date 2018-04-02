#ifndef __MINIKEY_CORE_MKVEC2_H__
#define __MINIKEY_CORE_MKVEC2_H__

//------------------------------------------------------------------------------------------------//
// float ����� vector(x, y)
//------------------------------------------------------------------------------------------------//

#include <math.h>
#include "MkCore_MkContainerDefinition.h"
#include "MkCore_MkFloatOp.h"


class MkVec2
{
public:

	//------------------------------------------------------------------------------------------------//
	// ������
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
	// ������ ������
	//------------------------------------------------------------------------------------------------//

	inline bool operator == (const MkVec2& vec) const { return ((x == vec.x) && (y == vec.y)); }
	inline bool operator != (const MkVec2& vec) const { return ((x != vec.x) || (y != vec.y)); }

	inline MkVec2 operator + (const MkVec2& vec) const { return MkVec2(x + vec.x, y + vec.y); }
	inline MkVec2 operator - (const MkVec2& vec) const { return MkVec2(x - vec.x, y - vec.y); }
	inline MkVec2 operator * (const MkVec2& vec) const { return MkVec2(x * vec.x, y * vec.y); } // ��Ģ�����δ� �������� �ʴ� ���������� ��� ���Ǹ� ���� �߰�
	inline MkVec2 operator * (const float& value) const { return MkVec2(x * value, y * value); }
	inline friend MkVec2 operator * (const float& value, const MkVec2& vec) { return MkVec2(value * vec.x, value * vec.y); }
	inline MkVec2 operator / (const float& value) const { return MkVec2(x / value, y / value); } // divided by zero ����ó�� ����
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
		x /= value; // divided by zero ����ó�� ����
		y /= value;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	// �Ϲ� �޼ҵ�
	//------------------------------------------------------------------------------------------------//

	// �ʱ�ȭ
	inline void Clear(void)
	{
		x = 0.f;
		y = 0.f;
	}

	// ������ ������ ����
	inline float SquaredLength(void) const { return (x * x + y * y); }

	// ���̸� ����
	inline float Length(void) const { return sqrtf(SquaredLength()); }

	// ����
	inline float DotProduct(const MkVec2& vec) const { return (x * vec.x + y * vec.y); }

	// ��������ȭ
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

	// vec�� ���� �������� ����
	inline void CompareAndKeepMin(const MkVec2& vec)
	{
		if (x > vec.x) x = vec.x;
		if (y > vec.y) y = vec.y;
	}

	// vec�� ���� ū���� ����
	inline void CompareAndKeepMax(const MkVec2& vec)
	{
		if (x < vec.x) x = vec.x;
		if (y < vec.y) y = vec.y;
	}

	// �������͸� ����
	inline MkVec2 Perpendicular(void) const { return MkVec2(-y, x); }

	// ����
	inline float CrossProduct(const MkVec2& vec) const { return (x * vec.y - y * vec.x); }

	// normal�� ������ �ݻ纤�͸� ����
	// weight(0 ~ 1) : 1�� �������� ���ݻ�
	inline MkVec2 Reflect(const MkVec2& normal, float weight = 1.f) const { return MkVec2(*this - (DotProduct(normal) * (1.f + weight) * normal)); }

	// grid �� �������� ����
	// ex> (4.8f, -2.3f).SnapToLowerBound(2.f) -> (4.f, -4.f)
	inline void SnapToLowerBound(float grid)
	{
		x = MkFloatOp::SnapToLowerBound(x, grid);
		y = MkFloatOp::SnapToLowerBound(y, grid);
	}

	// grid �� �������� �ø�
	// ex> (4.8f, -2.3f).SnapToUpperBound(2.f) -> (6.f, -2.f)
	inline void SnapToUpperBound(float grid)
	{
		x = MkFloatOp::SnapToUpperBound(x, grid);
		y = MkFloatOp::SnapToUpperBound(y, grid);
	}

	// zero �������� �Ǻ�
	inline bool IsZero(void) const
	{
		return ((x == 0.f) && (y == 0.f));
	}

	// zero ���Ϳ� ������� �Ǻ�
	inline bool CloseToZero(void) const { return (MkFloatOp::CloseToZero(x) && MkFloatOp::CloseToZero(y)); }

	// target ���Ϳ� ������� �Ǻ�
	inline bool CloseToNear(const MkVec2& target) const { return (MkFloatOp::CloseToNear(x, target.x) && MkFloatOp::CloseToNear(y, target.y)); }

public:

	float x, y;

	// ���
	static const MkVec2 Zero;
	static const MkVec2 AxisX;
	static const MkVec2 AxisY;
	static const MkVec2 One;
};

//------------------------------------------------------------------------------------------------//

// ����ũ��
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkVec2)

//------------------------------------------------------------------------------------------------//

#endif
