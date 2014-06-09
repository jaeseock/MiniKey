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

	// �� ���� ������ ����� ����
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

	// point�� plane ������ �Ÿ��� ����
	inline float GetLength(const MkVec3& point) const { return (normal.DotProduct(point) + d); }

	// sphere�� plane���� ���踦 ����
	// 1 : sphere�� plane�� �ո鿡 ��ġ
	// 0 : sphere�� plane�� ���� ����
	// -1 : sphere�� plane�� �޸鿡 ��ġ
	inline int CheckIntersection(const MkSphere& sphere) const
	{
		float length = GetLength(sphere.center);
		if (length > sphere.radius) // positive side
			return 1;
		if (length < -sphere.radius) // negative side
			return -1;
		return 0;
	}

	// sphere�� plane���� ���踦 ���ϰ� ���� ���� ��� ����(plane���� �߷��� sphere�� �ܸ�)�� �߽����� intersectionPoint�� ��ȯ
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

// ����ũ��
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkPlane)

//------------------------------------------------------------------------------------------------//

#endif
