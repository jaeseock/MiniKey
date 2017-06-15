#ifndef __MINIKEY_CORE_MKSPHERE_H__
#define __MINIKEY_CORE_MKSPHERE_H__

//------------------------------------------------------------------------------------------------//
// 3d sphere
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkVec3.h"


class MkSphere
{
public:
	
	inline MkSphere() {	SetUp(MkVec3::Zero, 0.f); }
	inline MkSphere(const MkVec3& centerPosition, float sphereRadius) { SetUp(centerPosition, sphereRadius); }
	inline MkSphere(const MkSphere& sphere) { SetUp(sphere.center, sphere.radius); }

	inline MkSphere& operator = (const MkSphere& sphere)
	{
		SetUp(sphere.center, sphere.radius);
		return *this;
	}

	inline bool operator == (const MkSphere& sphere) const { return ((center == sphere.center) && (radius == sphere.radius)); }
	inline bool operator != (const MkSphere& sphere) const { return ((center != sphere.center) || (radius != sphere.radius)); }

	// 초기화
	inline void SetUp(const MkVec3& centerPosition, float sphereRadius)
	{
		center = centerPosition;
		radius = sphereRadius;
	}

	// 자신과 sphere를 모두 포괄하는 구를 반환
	inline MkSphere GetUnion(const MkSphere& sphere) const
	{
		MkSphere result;
		if (center == sphere.center)
		{
			result.center = center;
			result.radius = GetMax<float>(radius, sphere.radius);
		}
		else
		{
			MkVec3 thisToTarget = sphere.center - center;
			float distance = thisToTarget.Normalize();

			if ((distance + sphere.radius) <= radius) // sphere가 자신에게 포함됨
			{
				result = *this;
			}
			else if ((distance + radius) <= sphere.radius) // 자신이 sphere에게 포함됨
			{
				result = sphere;
			}
			else
			{
				float diameter = distance + radius + sphere.radius;
				result.radius = diameter * 0.5f;
				float thisToNewCenterLength = result.radius - radius;
				result.center = center + thisToTarget * thisToNewCenterLength;
			}
		}
		return result;
	}

	// 자신 내부에 pt가 속하는지 여부 반환
	inline bool CheckIntersection(const MkVec3& pt) const
	{
		MkVec3 thisToPt = pt - center;
		return (thisToPt.Length() <= radius);
	}

	// (sphere와 자신과의 거리 / 반지름의 합) 반환
	// 1 이하면 교차중이고 초과면 접하지 않음
	inline float GetDistanceRatio(const MkSphere& sphere) const
	{
		MkVec3 thisToTarget = sphere.center - center;
		float distance = thisToTarget.Length();
		return (distance / (radius + sphere.radius));
	}

	// sphere가 자신과 겹치거나 포함되어 있는지 여부 반환
	inline bool CheckIntersection(const MkSphere& sphere) const { return (GetDistanceRatio(sphere) <= 1.f); }

public:

	MkVec3 center;
	float radius;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkSphere)

//------------------------------------------------------------------------------------------------//

#endif
