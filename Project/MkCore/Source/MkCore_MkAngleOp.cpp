
#include <math.h>
#include <float.h>
#include "MkCore_MkGlobalDefinition.h"
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkVec2.h"
#include "MkCore_MkAngleOp.h"

//------------------------------------------------------------------------------------------------//

float MkAngleOp::Unitize360(float degree)
{
	return MkFloatOp::GetRemainder(degree, 360.f);
}

float MkAngleOp::Unitize180(float degree)
{
	return (MkFloatOp::GetRemainder(degree + 180.f, 360.f) - 180.f);
}

float MkAngleOp::Unitize2PI(float radian)
{
	return MkFloatOp::GetRemainder(radian, MKDEF_PI * 2.f);
}

float MkAngleOp::UnitizePI(float radian)
{
	return (MkFloatOp::GetRemainder(radian + MKDEF_PI, MKDEF_PI * 2.f) - MKDEF_PI);
}

float MkAngleOp::GetAddition(float from, float to)
{
	return Unitize180(to - from);
}

float MkAngleOp::DegreeToRadian(float degree)
{
	return (degree * MKDEF_PI_DIVIDED_BY_180D);
}

float MkAngleOp::RadianToDegree(float radian)
{
	return (radian * MKDEF_180D_DIVIDED_BY_PI);
}

float MkAngleOp::GetRotatationDelta(float radius, float movementDelta)
{
	// movementDelta : 2*PI*r = theta:2*PI >> theta = movementDelta / r;
	return (MkFloatOp::CloseToZero(radius)) ? 0.f : (movementDelta / radius);
}

//------------------------------------------------------------------------------------------------//

float MkAngleOp::Sin(float radian)
{
	return sinf(radian);
}

float MkAngleOp::Cos(float radian)
{
	return cosf(radian);
}

float MkAngleOp::Tan(float radian)
{
	float unitRadian = Unitize2PI(radian);

	// tan의 경우 sin / cos이므로 cos이 0이 되는 0.5*PI와 1.5*PI는 존재하지 않음
	return (MkFloatOp::CloseToNear(unitRadian, MKDEF_PI * 0.5f) || MkFloatOp::CloseToNear(unitRadian, MKDEF_PI * 1.5f)) ?
		FLT_MAX : tanf(unitRadian);
}

float MkAngleOp::ASin(float value)
{
	return asinf(Clamp<float>(value, -1.f, 1.f));
}

float MkAngleOp::ACos(float value)
{
	return acosf(Clamp<float>(value, -1.f, 1.f));
}

float MkAngleOp::ATan(float value)
{
	return atanf(Clamp<float>(value, -1.f, 1.f));
}

//------------------------------------------------------------------------------------------------//

float MkAngleOp::DirectionToRadian(const MkVec2& direction)
{
	float radian = ACos(direction.y);
	return (direction.x < 0.f) ? Unitize2PI(-radian) : radian;
}

MkVec2 MkAngleOp::RadianToDirection(float radian)
{
	return MkVec2(Sin(radian), Cos(radian));
}

//------------------------------------------------------------------------------------------------//
