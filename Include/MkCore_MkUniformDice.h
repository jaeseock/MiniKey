#ifndef __MINIKEY_CORE_MKUNIFORMDICE_H__
#define __MINIKEY_CORE_MKUNIFORMDICE_H__

//------------------------------------------------------------------------------------------------//
// uniform ���� ������
// �������� �������� ����
//
// �ּҰ��� �ִ밪�� ������ ������ ���� ����
// ex> (1, 6)�̸�, �������� (1, 2, 3, 4, 5, 6) �� �ϳ��� ����
//
// ���� ���� �˰����� ���λ����� "MkCore_MkRandomGenerator.h" ����
//------------------------------------------------------------------------------------------------//
// ex>
//	unsigned int max = 9; // ��������(0 ~ max)
//	unsigned int testCount = 100000; // �׽�Ʈ Ƚ��
//	unsigned int bufferSize = max + 1;
//	unsigned int totalCount = bufferSize * testCount;
//
//	MkArray<unsigned int> hitCount;
//	hitCount.Fill(bufferSize, 0);
//
//	MkUniformDice dice(0, max);
//	for (unsigned int i=0; i<totalCount; ++i)
//	{
//		int n = dice.GenerateRandomNumber();
//		++hitCount[n];
//	}
//
//	for (unsigned int i=0; i<bufferSize; ++i)
//	{
//		wprintf(L" - %d : %d hits\n", i, hitCount[i]);
//	}
//
//	hitCount.SortInAscendingOrder();
//	unsigned int lowest = hitCount[0];
//	hitCount.SortInDescendingOrder();
//	unsigned int highest = hitCount[0];
//	float diff = static_cast<float>(highest - lowest) * 100.f / static_cast<float>(highest);
//
//	wprintf(L"---------------------------------\n");
//	wprintf(L" lowest : %d\n", lowest);
//	wprintf(L" highest : %d\n", highest);
//	wprintf(L" difference : %f %%\n", diff);
//	wprintf(L"---------------------------------\n");
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRandomGenerator.h"


class MkUniformDice
{
public:

	// �õ� ����(0 ~ 20000)
	void SetSeed(unsigned int seed);

	// ���� ��ȯ���� ���� ����
	void SetMinMax(unsigned int min, unsigned int max);

	// �ǻ糭�� ����
	unsigned int GenerateRandomNumber(void);

	// ������
	MkUniformDice();
	MkUniformDice(unsigned int min, unsigned int max);
	MkUniformDice(unsigned int min, unsigned int max, unsigned int seed);

protected:

	MkRandomGenerator m_RandomGenerator;

	unsigned int m_Min;
	unsigned int m_Max;
};

//------------------------------------------------------------------------------------------------//

// ����ũ��
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkUniformDice)

//------------------------------------------------------------------------------------------------//

#endif
