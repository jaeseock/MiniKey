
#include "MkCore_MkCheck.h"
#include "MkCore_MkUniformDice.h"


//------------------------------------------------------------------------------------------------//

void MkUniformDice::SetSeed(unsigned int seed)
{
	m_RandomGenerator.SetSeed(seed);
}

void MkUniformDice::SetMinMax(unsigned int min, unsigned int max)
{
	MK_CHECK(min <= max, L"MkUniformDice의 난수 최대값보다 최소값이 더 큼")
	{
		min = max;
	}

	m_Min = min;
	m_Max = max;
}

unsigned int MkUniformDice::GenerateRandomNumber(void)
{
	unsigned int addition = 0;
	unsigned int offset = m_Max - m_Min;
	if (offset > 0)
	{
		addition = m_RandomGenerator.GetRandomNumber(offset + 1);
	}
	return (m_Min + addition);
}

MkUniformDice::MkUniformDice()
{
	SetMinMax(0, 1);
}

MkUniformDice::MkUniformDice(unsigned int min, unsigned int max)
{
	SetMinMax(min, max);
}

MkUniformDice::MkUniformDice(unsigned int min, unsigned int max, unsigned int seed) : m_RandomGenerator(seed)
{
	SetMinMax(min, max);
}

//------------------------------------------------------------------------------------------------//
