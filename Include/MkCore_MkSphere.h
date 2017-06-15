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

	// �ʱ�ȭ
	inline void SetUp(const MkVec3& centerPosition, float sphereRadius)
	{
		center = centerPosition;
		radius = sphereRadius;
	}

	// �ڽŰ� sphere�� ��� �����ϴ� ���� ��ȯ
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

			if ((distance + sphere.radius) <= radius) // sphere�� �ڽſ��� ���Ե�
			{
				result = *this;
			}
			else if ((distance + radius) <= sphere.radius) // �ڽ��� sphere���� ���Ե�
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

	// �ڽ� ���ο� pt�� ���ϴ��� ���� ��ȯ
	inline bool CheckIntersection(const MkVec3& pt) const
	{
		MkVec3 thisToPt = pt - center;
		return (thisToPt.Length() <= radius);
	}

	// (sphere�� �ڽŰ��� �Ÿ� / �������� ��) ��ȯ
	// 1 ���ϸ� �������̰� �ʰ��� ������ ����
	inline float GetDistanceRatio(const MkSphere& sphere) const
	{
		MkVec3 thisToTarget = sphere.center - center;
		float distance = thisToTarget.Length();
		return (distance / (radius + sphere.radius));
	}

	// sphere�� �ڽŰ� ��ġ�ų� ���ԵǾ� �ִ��� ���� ��ȯ
	inline bool CheckIntersection(const MkSphere& sphere) const { return (GetDistanceRatio(sphere) <= 1.f); }

public:

	MkVec3 center;
	float radius;
};

//------------------------------------------------------------------------------------------------//

// ����ũ��
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkSphere)

//------------------------------------------------------------------------------------------------//

#endif
