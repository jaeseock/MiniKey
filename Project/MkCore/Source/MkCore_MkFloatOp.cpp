
#include <math.h>
#include "MkCore_MkGlobalDefinition.h"
#include "MkCore_MkFloatOp.h"

//------------------------------------------------------------------------------------------------//

float MkFloatOp::SnapToLowerBound(float source, float grid)
{
	return (floor(source / grid) * grid);
}

float MkFloatOp::SnapToUpperBound(float source, float grid)
{
	return (ceil(source / grid) * grid);
}

float MkFloatOp::GetRemainder(float source, float divider)
{
	return (source - SnapToLowerBound(source, divider));
}

float MkFloatOp::RoundingOff(float source)
{
	return floor(source + 0.5f);
}

int MkFloatOp::FloatToInt(float source)
{
	int i;
	static const float round_toward_m_i = -0.5f;
	__asm
	{
		fld      source
		fadd     st, st (0)
		fadd     round_toward_m_i
		fistp    i
		sar      i, 1
	}
	return i;
}

bool MkFloatOp::IsBetween(float minRange, float source, float maxRange)
{
	return ((source >= minRange) && (source <= maxRange));
}

float MkFloatOp::GetLinearPosition(float minRange, float source, float maxRange)
{
	return (source - minRange) / (maxRange - minRange);
}

float MkFloatOp::GetLinearPosition(float lastX, float lastY, float currX, float nextX, float nextY)
{
	float offset = GetLinearPosition(lastX, currX, nextX);
	return ((nextY - lastY) * offset + lastY);
}

bool MkFloatOp::CloseToNear(float source, float target)
{
	return IsBetween(-MKDEF_FLOAT_EPSILON + target, source, MKDEF_FLOAT_EPSILON + target);
}

bool MkFloatOp::CloseToZero(float source)
{
	return CloseToNear(source, 0.f);
}

float MkFloatOp::LinearInterpolate(float a, float b, float fraction)
{
	return (a - fraction * (b - a)); // == (a * (1.f - fraction) + b * fraction)
}

float MkFloatOp::CosineInterpolate(float a, float b, float fraction)
{
	float f = (1.f - cos(fraction * MKDEF_PI)) * 0.5f;
	return LinearInterpolate(a, b, f);
}

float MkFloatOp::GenerateRandomNumber(int seed)
{
	seed = (seed << 13) ^ seed;
	return static_cast<float>(1.0 - (double)((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

void MkFloatOp::ConvertSecondsToClockTime(float seconds, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int& millisec)
{
	unsigned int secs = static_cast<unsigned int>(FloatToInt(seconds));
	hour = secs / 3600;
	secs = secs % 3600;
	minute = secs / 60;
	second = secs % 60;
	millisec = static_cast<unsigned int>(FloatToInt(seconds * 1000.f)) % 1000;
}

//------------------------------------------------------------------------------------------------//

