
#include <Windows.h>
#include <math.h>
#include "MkCore_MkCheck.h"
#include "MkCore_MkRandomGenerator.h"


//------------------------------------------------------------------------------------------------//

void MkRandomGenerator::SetSeed(unsigned int seed)
{
	if (seed > 20000)
		seed = (seed % 20000);

	m_Seed[0] = seed + 673;
	m_Seed[1] = m_Seed[0] + 113;
	m_Seed[2] = m_Seed[0] - 271;
}

float MkRandomGenerator::GetRandomValue(void)
{
	m_Seed[0] = (171 * m_Seed[0]) % 30269;
	m_Seed[1] = (172 * m_Seed[1]) % 30307;
	m_Seed[2] = (170 * m_Seed[2]) % 30323;

	float f0 = static_cast<float>(m_Seed[0]) / 30269.f;
	float f1 = static_cast<float>(m_Seed[1]) / 30307.f;
	float f2 = static_cast<float>(m_Seed[2]) / 30323.f;

	float x = f0 + f1 + f2; // 0.f ~ 3.f
	return (x - floor(x)); // 소수부 반환
}

unsigned int MkRandomGenerator::GetRandomNumber(unsigned int offset)
{
	MK_CHECK(offset >= 2, L"난수 범위가 2 이상이 아님")
		return 0;

	float x = GetRandomValue() * static_cast<float>(offset);
	return static_cast<unsigned int>(x);
}

MkRandomGenerator::MkRandomGenerator()
{
	SetSeed(_GetSeedFromTime());
}

MkRandomGenerator::MkRandomGenerator(unsigned int seed)
{
	SetSeed(seed);
}

unsigned int MkRandomGenerator::_GetSeedFromTime(void) const
{
	return (static_cast<unsigned int>(GetTickCount()) % 20000);
}

//------------------------------------------------------------------------------------------------//
