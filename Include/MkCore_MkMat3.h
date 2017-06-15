#ifndef __MINIKEY_CORE_MKMAT3_H__
#define __MINIKEY_CORE_MKMAT3_H__

//------------------------------------------------------------------------------------------------//
// 3X3 매트릭스
// MkVec3 기반
// 행우선(row-major)
//
// * 오른손좌표계-열우선(OpenGL, Ogre, etc...),
// * 오른손좌표계-행우선(MiniKey, XNA, etc...),
// * 왼손좌표계-행우선(DirectX),
// -> 좌표계-행렬구성방식이 플랫폼마다 다르다는 점을 유의 할 것
//
// 회전행렬의 경우 행렬의 열 벡터들이 정규직교집합이므로 전치행렬이 곧 역행렬이 된다는 특장점이 존재
//------------------------------------------------------------------------------------------------//

#include <memory.h>
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkAngleOp.h"


class MkMat3
{
public:

	//------------------------------------------------------------------------------------------------//
	
	inline MkMat3() { SetIdentity(); }
	inline MkMat3(const MkMat3& mat) { *this = mat; }
	inline MkMat3(const MkVec3& x_axis, const MkVec3& y_axis, const MkVec3& z_axis) { FromAxis(x_axis, y_axis, z_axis); }

	inline MkMat3& operator = (const MkMat3& mat)
	{
		memcpy_s(m, 9 * sizeof(float), mat.m, 9 * sizeof(float));
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	
	inline bool operator == (const MkMat3& mat) const { return (memcmp(m, mat.m, 9 * sizeof(float)) == 0); }
	inline bool operator != (const MkMat3& mat) const { return !operator==(mat); }

	inline MkMat3 operator + (const MkMat3& mat) const
	{
		MkMat3 tmp;
		tmp.m[0][0] = m[0][0] + mat.m[0][0];
		tmp.m[0][1] = m[0][1] + mat.m[0][1];
		tmp.m[0][2] = m[0][2] + mat.m[0][2];
		tmp.m[1][0] = m[1][0] + mat.m[1][0];
		tmp.m[1][1] = m[1][1] + mat.m[1][1];
		tmp.m[1][2] = m[1][2] + mat.m[1][2];
		tmp.m[2][0] = m[2][0] + mat.m[2][0];
		tmp.m[2][1] = m[2][1] + mat.m[2][1];
		tmp.m[2][2] = m[2][2] + mat.m[2][2];
		return tmp;
	}

	inline MkMat3 operator - (const MkMat3& mat) const
	{
		MkMat3 tmp;
		tmp.m[0][0] = m[0][0] - mat.m[0][0];
		tmp.m[0][1] = m[0][1] - mat.m[0][1];
		tmp.m[0][2] = m[0][2] - mat.m[0][2];
		tmp.m[1][0] = m[1][0] - mat.m[1][0];
		tmp.m[1][1] = m[1][1] - mat.m[1][1];
		tmp.m[1][2] = m[1][2] - mat.m[1][2];
		tmp.m[2][0] = m[2][0] - mat.m[2][0];
		tmp.m[2][1] = m[2][1] - mat.m[2][1];
		tmp.m[2][2] = m[2][2] - mat.m[2][2];
		return tmp;
	}

	inline MkMat3 operator * (const MkMat3& mat) const
	{
		MkMat3 tmp;
		tmp.m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0];
		tmp.m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1];
		tmp.m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2];
		tmp.m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0];
		tmp.m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1];
		tmp.m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2];
		tmp.m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0];
		tmp.m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1];
		tmp.m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2];
		return tmp;
	}

	inline friend MkMat3 operator * (const float& value, const MkMat3& mat)
	{
		MkMat3 tmp;
		tmp.m[0][0] = mat.m[0][0] * value;
		tmp.m[0][1] = mat.m[0][1] * value;
		tmp.m[0][2] = mat.m[0][2] * value;
		tmp.m[1][0] = mat.m[1][0] * value;
		tmp.m[1][1] = mat.m[1][1] * value;
		tmp.m[1][2] = mat.m[1][2] * value;
		tmp.m[2][0] = mat.m[2][0] * value;
		tmp.m[2][1] = mat.m[2][1] * value;
		tmp.m[2][2] = mat.m[2][2] * value;
		return tmp;
	}

	inline friend MkVec3 operator * (const MkVec3& vec, const MkMat3& mat)
	{
		MkVec3 tmp;
		tmp.x = mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z;
		tmp.y = mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z;
		tmp.z = mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z;
		return tmp;
	}

	inline MkMat3 operator - () const
	{
		MkMat3 tmp;
		tmp.m[0][0] = -m[0][0];
		tmp.m[0][1] = -m[0][1];
		tmp.m[0][2] = -m[0][2];
		tmp.m[1][0] = -m[1][0];
		tmp.m[1][1] = -m[1][1];
		tmp.m[1][2] = -m[1][2];
		tmp.m[2][0] = -m[2][0];
		tmp.m[2][1] = -m[2][1];
		tmp.m[2][2] = -m[2][2];
		return tmp;
	}

	inline MkMat3& operator += (const MkMat3& mat)
	{
		m[0][0] += mat.m[0][0];
		m[0][1] += mat.m[0][1];
		m[0][2] += mat.m[0][2];
		m[1][0] += mat.m[1][0];
		m[1][1] += mat.m[1][1];
		m[1][2] += mat.m[1][2];
		m[2][0] += mat.m[2][0];
		m[2][1] += mat.m[2][1];
		m[2][2] += mat.m[2][2];
		return *this;
	}

	inline MkMat3& operator -= (const MkMat3& mat)
	{
		m[0][0] -= mat.m[0][0];
		m[0][1] -= mat.m[0][1];
		m[0][2] -= mat.m[0][2];
		m[1][0] -= mat.m[1][0];
		m[1][1] -= mat.m[1][1];
		m[1][2] -= mat.m[1][2];
		m[2][0] -= mat.m[2][0];
		m[2][1] -= mat.m[2][1];
		m[2][2] -= mat.m[2][2];
		return *this;
	}

	inline MkMat3& operator *= (const MkMat3& mat)
	{
		*this = operator*(mat);
		return *this;
	}

	//------------------------------------------------------------------------------------------------//

	// 정규화
	inline void SetIdentity(void)
	{
		m[0][0] = m[1][1] = m[2][2] = 1.f;
		m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1] = 0.f;
	}

	// 전치행렬
	// 세 축이 직교하는 행렬의 경우 전치행렬이 곧 역행렬이 됨
	inline void Transpose(void)
	{
		float tmp;
		tmp = m[0][1];
		m[0][1] = m[1][0];
		m[1][0] = tmp;
		tmp = m[0][2];
		m[0][2] = m[2][0];			
		m[2][0] = tmp;
		tmp = m[1][2];
		m[1][2] = m[2][1];
		m[2][1] = tmp;
	}

	// 전치행렬 반환
	inline MkMat3 GetTranspose(void) const
	{
		MkMat3 tmp;
		tmp.m[0][0] = m[0][0];
		tmp.m[0][1] = m[1][0];
		tmp.m[0][2] = m[2][0];
		tmp.m[1][0] = m[0][1];
		tmp.m[1][1] = m[1][1];
		tmp.m[1][2] = m[2][1];
		tmp.m[2][0] = m[0][2];
		tmp.m[2][1] = m[1][2];
		tmp.m[2][2] = m[2][2];
		return tmp;
	}

	// 역행렬
	inline void Inverse(void)
	{
		float a = m[0][0];
		float b = m[0][1];
		float c = m[0][2];
		float d = m[1][0];
		float e = m[1][1];
		float f = m[1][2];
		float g = m[2][0];
		float h = m[2][1];
		float i = m[2][2];
		float norm = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
		if (norm != 0.f)
		{
			float invNorm = 1.f / norm;
			m[0][0] = (e * i - f * h) * invNorm;
			m[0][1] = (c * h - b * i) * invNorm;
			m[0][2] = (b * f - c * e) * invNorm;
			m[1][0] = (f * g - d * i) * invNorm;
			m[1][1] = (a * i - c * g) * invNorm;
			m[1][2] = (c * d - a * f) * invNorm;
			m[2][0] = (d * h - e * g) * invNorm;
			m[2][1] = (b * g - a * h) * invNorm;
			m[2][2] = (a * e - b * d) * invNorm;
		}
	}

	// 역행렬 반환
	inline MkMat3 GetInverse(void) const
	{
		MkMat3 tmp = *this;
		tmp.Inverse();
		return tmp;
	}

	// 행 반환
	inline void GetRow(int index, float& row_0, float& row_1, float& row_2) const
	{
		row_0 = m[index][0];
		row_1 = m[index][1];
		row_2 = m[index][2];
	}

	inline void GetRow(int index, MkVec3& row) const { GetRow(index, row.x, row.y, row.z); }

	inline MkVec3 GetRow(int index) const
	{
		MkVec3 r;
		GetRow(index, r.x, r.y, r.z);
		return r;
	}

	// 행으로부터 초기화
	inline void SetRow(int index, float row_0, float row_1, float row_2)
	{
		m[index][0] = row_0;
		m[index][1] = row_1;
		m[index][2] = row_2;
	}

	inline void SetRow(int index, const MkVec3& row) { SetRow(index, row.x, row.y, row.z); }

	// 열 반환
	inline void GetCol(int index, float& col_0, float& col_1, float& col_2) const
	{
		col_0 = m[0][index];
		col_1 = m[1][index];
		col_2 = m[2][index];
	}

	inline void GetCol(int index, MkVec3& col) const { GetCol(index, col.x, col.y, col.z); }

	inline MkVec3 GetCol(int index) const
	{
		MkVec3 c;
		GetCol(index, c.x, c.y, c.z);
		return c;
	}

	// 열로부터 초기화
	inline void SetCol(int index, float col_0, float col_1, float col_2)
	{
		m[0][index] = col_0;
		m[1][index] = col_1;
		m[2][index] = col_2;
	}

	inline void SetCol(int index, const MkVec3& col) { SetCol(index, col.x, col.y, col.z); }

	// x축 회전
	inline void FromRotationX(float radian)
	{
		float s = MkAngleOp::Sin(radian);
		float c = MkAngleOp::Cos(radian);
		m[0][0] = 1.f;
		m[1][1] = m[2][2] = c;
		m[0][1] = m[0][2] = m[1][0] = m[2][0] = 0.f;
		m[1][2] = s;
		m[2][1] = -s;
	}

	// y축 회전
	inline void FromRotationY(float radian)
	{
		float s = MkAngleOp::Sin(radian);
		float c = MkAngleOp::Cos(radian);
		m[1][1] = 1.f;
		m[0][0] = m[2][2] = c;
		m[0][1] = m[1][0] = m[1][2] = m[2][1] = 0.f;
		m[0][2] = -s;
		m[2][0] = s;
	}

	// z축 회전
	inline void FromRotationZ(float radian)
	{
		float s = MkAngleOp::Sin(radian);
		float c = MkAngleOp::Cos(radian);
		m[2][2] = 1.f;
		m[0][0] = m[1][1] = c;
		m[0][2] = m[1][2] = m[2][0] = m[2][1] = 0.f;
		m[0][1] = s;
		m[1][0] = -s;
	}

	// xyz 오일러각 회전
	inline void FromRotationXYZ(const MkVec3& angle)
	{
		float sx = MkAngleOp::Sin(angle.x);
		float cx = MkAngleOp::Cos(angle.x);
		float sy = MkAngleOp::Sin(angle.y);
		float cy = MkAngleOp::Cos(angle.y);
		float sz = MkAngleOp::Sin(angle.z);
		float cz = MkAngleOp::Cos(angle.z);

		m[0][0] = cy * cz;
		m[0][1] = cy * sz;
		m[0][2] = -sy;
		m[1][0] = sx * sy * cz - cx * sz;
		m[1][1] = sx * sy * sz + cx * cz;
		m[1][2] = sx * cy;
		m[2][0] = cx * sy * cz + sx * sz;
		m[2][1] = cx * sy * sz - sx * cz;
		m[2][2] = cx * cy;
	}

	// 특정 axis 단위벡터를 축으로 회전
	inline void FromAxisRadian(const MkVec3& axis, float radian)
	{
		float st = MkAngleOp::Sin(radian);
		float ct = MkAngleOp::Cos(radian);
		float vt = 1.f - ct;
		const float& kx = axis.x;
		const float& ky = axis.y;
		const float& kz = axis.z;
		float xsin = kx * st;
		float ysin = ky * st;
		float zsin = kz * st;
		float x2 = kx * kx;
		float y2 = ky * ky;
		float z2 = kz * kz;
		float xym = kx * ky * vt;
		float xzm = kx * kz * vt;
		float yzm = ky * kz * vt;

		m[0][0] = x2 * vt + ct;
		m[0][1] = xym + zsin;
		m[0][2] = xzm - ysin;
		m[1][0] = xym - zsin;
		m[1][1] = y2 * vt + ct;
		m[1][2] = yzm + xsin;
		m[2][0] = xzm + ysin;
		m[2][1] = yzm - xsin;
		m[2][2] = z2 * vt + ct;
	}
	
	// 직교하는 오른손좌표계의 x, y, z 단위벡터를 축으로 한 회전행렬
	inline void FromAxis(const MkVec3& x_axis, const MkVec3& y_axis, const MkVec3& z_axis)
	{
		SetRow(0, x_axis);
		SetRow(1, y_axis);
		SetRow(2, z_axis);
	}

	// 축으로 지정된 단위벡터 반환
	inline void GetAxis(MkVec3& x_axis, MkVec3& y_axis, MkVec3& z_axis) const
	{
		x_axis = GetRow(0);
		y_axis = GetRow(1);
		z_axis = GetRow(2);
	}

	// 각 축을 이루는 단위벡터 반환
	inline MkVec3 GetAxisX(void) const { return GetRow(0); }
	inline MkVec3 GetAxisY(void) const { return GetRow(1); }
	inline MkVec3 GetAxisZ(void) const { return GetRow(2); }

	// src 단위벡터를 dest 단위벡터로 만드는 회전행렬을 구함
	inline bool FromSourceAndDestination(const MkVec3& src, const MkVec3& dest)
	{
		MkVec3 fdest;
		float addition;
		if (src.DotProduct(dest) >= 0.f)
		{
			fdest = dest;
			addition = 0.f;
		}
		else
		{
			fdest = -dest;
			addition = MKDEF_PI;
		}

		MkVec3 pivotDir = fdest.CrossProduct(src);
		if (pivotDir.Normalize() == 0.f)
			return false;
		
		MkVec3 localPivot = src.GetPivotVector(pivotDir);
		MkVec2 localDir;
		localDir.x = localPivot.DotProduct(dest);
		localDir.y = src.DotProduct(dest);
		localDir.Normalize();

		FromAxisRadian(pivotDir, MkAngleOp::DirectionToRadian(localDir) + addition);
		//FromAxisRadian(-pivotDir, MkAngleOp::DirectionToRadian(localDir) + addition); // CCW

		return true;
	}

	//------------------------------------------------------------------------------------------------//

public:

	//	m[0][0], m[0][1], m[0][2]	//
	//	m[1][0], m[1][1], m[1][2]	//
	//	m[2][0], m[2][1], m[2][2]	//

	float m[3][3];

	static const MkMat3 Identity;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkMat3)

//------------------------------------------------------------------------------------------------//

#endif

