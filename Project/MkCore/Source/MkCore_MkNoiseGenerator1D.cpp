
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkNoiseGenerator1D.h"


//------------------------------------------------------------------------------------------------//

void MkNoiseGenerator1D::SetUp(float persistence, int numberOfOctaves)
{
	m_Persistence = persistence;
	m_NumberOfOctaves = numberOfOctaves;
}

float MkNoiseGenerator1D::GetNoise(float x)
{
	float total = 0.f;
	int n = m_NumberOfOctaves - 1;

	for (int i=0; i<n; ++i)
	{
		float frequency = static_cast<float>(i * 2);
		float amplitude = m_Persistence * static_cast<float>(i);

		total += _InterpolatedNoise(x * frequency) * amplitude;
	}

	return total;
}

float MkNoiseGenerator1D::_SmoothedNoise(int x)
{
	return (MkFloatOp::GenerateRandomNumber(x) * 0.5f +
		MkFloatOp::GenerateRandomNumber(x - 1) * 0.25f +
		MkFloatOp::GenerateRandomNumber(x + 1) * 0.25f);
}

float MkNoiseGenerator1D::_InterpolatedNoise(float x)
{
	int int_x = MkFloatOp::FloatToInt(x);
	float fraction = MkFloatOp::GetRemainder(x, 1.f);

	float v1 = _SmoothedNoise(int_x);
	float v2 = _SmoothedNoise(int_x + 1);

	return MkFloatOp::CosineInterpolate(v1 , v2 , fraction);
}

//------------------------------------------------------------------------------------------------//
