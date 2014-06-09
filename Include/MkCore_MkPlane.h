#ifndef __MINIKEY_CORE_MKPLANE_H__
#define __MINIKEY_CORE_MKPLANE_H__

//------------------------------------------------------------------------------------------------//
// 3d plane
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSphere.h"


class MkPlane
{
public:
	
	MkPlane() {}

	MkPlane(const MkVec3& normalIn, float dIn)
	{
		normal = normalIn;
		d = dIn;
	}

	MkPlane(const MkPlane& plane)
	{
		normal = plane.normal;
		d = plane.d;
	}

	inline MkPlane& operator = (const MkPlane& plane)
	{
		normal = plane.normal;
		d = plane.d;
		return *this;
	}

	inline bool operator == (const MkPlane& plane) const { return ((normal == plane.normal) && (d == plane.d)); }
	inline bool operator != (const MkPlane& plane) const { return ((normal != plane.normal) || (d != plane.d)); }

	// 세 점을 지나는 평면을 생성
	inline bool SetUp(const MkVec3& v0, const MkVec3& v1, const MkVec3& v2, bool clockwise = false)
	{
		if (v0.CloseToNear(v1) || v0.CloseToNear(v2))
			return false;

		MkVec3 p01 = v1 - v0;
		MkVec3 p02 = v2 - v0;
		normal = (clockwise) ? p02.CrossProduct(p01) : p01.CrossProduct(p02);
		normal.Normalise();
		d = -(normal.DotProduct(v1)); // D = -N dot P
		return true;
	}

	// point와 plane 사이의 거리를 구함
	inline float GetLength(const MkVec3& point) const { return (normal.DotProduct(point) + d); }

	// sphere와 plane간의 관계를 구함
	// 1 : sphere가 plane의 앞면에 위치
	// 0 : sphere가 plane과 겹쳐 있음
	// -1 : sphere가 plane의 뒷면에 위치
	inline int CheckIntersection(const MkSphere& sphere) const
	{
		float length = GetLength(sphere.center);
		if (length > sphere.radius) // positive side
			return 1;
		if (length < -sphere.radius) // negative side
			return -1;
		return 0;
	}

	// sphere와 plane간의 관계를 구하고 겹쳐 있을 경우 접면(plane으로 잘려진 sphere의 단면)의 중심점을 intersectionPoint에 반환
	inline int CheckIntersection(const MkSphere& sphere, MkVec3& intersectionPoint) const
	{
		float length = GetLength(sphere.center);
		if (length > sphere.radius) // positive side
			return 1;
		if (length < -sphere.radius) // negative side
			return -1;

		intersectionPoint = sphere.center - normal * length;
		return 0;
	}

public:

	MkVec3 normal;
	float d;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkPlane)

//------------------------------------------------------------------------------------------------//

#endif
