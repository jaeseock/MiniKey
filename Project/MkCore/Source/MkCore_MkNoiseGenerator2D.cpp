
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkNoiseGenerator2D.h"


//------------------------------------------------------------------------------------------------//

void MkNoiseGenerator2D::SetUp(float persistence, int numberOfOctaves)
{
	m_Persistence = persistence;
	m_NumberOfOctaves = numberOfOctaves;
}

float MkNoiseGenerator2D::GetNoise(float x, float y)
{
	float total = 0.f;
	int n = m_NumberOfOctaves - 1;

	for (int i=0; i<n; ++i)
	{
		float frequency = static_cast<float>(i * 2);
		float amplitude = m_Persistence * static_cast<float>(i);

		total += _InterpolatedNoise(x * frequency, y * frequency) * amplitude;
	}

	return total;
}

float MkNoiseGenerator2D::_MakeRandom(int x, int y)
{
	return MkFloatOp::GenerateRandomNumber(x + y * 57);
}

float MkNoiseGenerator2D::_SmoothedNoise(int x, int y)
{
	int mx = x - 1;
	int px = x + 1;
	int my = y - 1;
	int py = y + 1;
	float corners = (_MakeRandom(mx, my) + _MakeRandom(px, my) + _MakeRandom(mx, py) + _MakeRandom(px, py)) * 0.0625f; // / 16
	float sides = (_MakeRandom(mx, y) + _MakeRandom(px, y) + _MakeRandom(x, my) + _MakeRandom(x, py)) * 0.125f; // / 8
	float center = _MakeRandom(x, y) * 0.25f; // / 4
	return (corners + sides + center);
}

float MkNoiseGenerator2D::_InterpolatedNoise(float x, float y)
{
	int int_x = MkFloatOp::FloatToInt(x);
	float fraction_x = MkFloatOp::GetRemainder(x, 1.f);

	int int_y = MkFloatOp::FloatToInt(y);
	float fraction_y = MkFloatOp::GetRemainder(y, 1.f);

	int px = int_x + 1;
	int py = int_y + 1;

	float v1 = _SmoothedNoise(int_x, int_y);
	float v2 = _SmoothedNoise(px, int_y);
	float v3 = _SmoothedNoise(int_x, py);
	float v4 = _SmoothedNoise(px, py);

	float i1 = MkFloatOp::LinearInterpolate(v1 , v2 , fraction_x);
	float i2 = MkFloatOp::LinearInterpolate(v3 , v4 , fraction_x);

	return MkFloatOp::CosineInterpolate(i1, i2, fraction_y);
}

//------------------------------------------------------------------------------------------------//
