#ifndef __MINIKEY_CORE_MKUNIFORMDICE_H__
#define __MINIKEY_CORE_MKUNIFORMDICE_H__

//------------------------------------------------------------------------------------------------//
// uniform 랜덤 생성기
// 난수값의 분포도가 균질
//
// 최소값과 최대값을 설정해 범위내 난수 생성
// ex> (1, 6)이면, 난수값은 (1, 2, 3, 4, 5, 6) 중 하나가 나옴
//
// 랜덤 생성 알고리즘의 세부사항은 "MkCore_MkRandomGenerator.h" 참조
//------------------------------------------------------------------------------------------------//
// ex>
//	unsigned int max = 9; // 난수범위(0 ~ max)
//	unsigned int testCount = 100000; // 테스트 횟수
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

	// 시드 설정(0 ~ 20000)
	void SetSeed(unsigned int seed);

	// 난수 반환값의 범위 설정
	void SetMinMax(unsigned int min, unsigned int max);

	// 의사난수 생성
	unsigned int GenerateRandomNumber(void);

	// 생성자
	MkUniformDice();
	MkUniformDice(unsigned int min, unsigned int max);
	MkUniformDice(unsigned int min, unsigned int max, unsigned int seed);

protected:

	MkRandomGenerator m_RandomGenerator;

	unsigned int m_Min;
	unsigned int m_Max;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkUniformDice)

//------------------------------------------------------------------------------------------------//

#endif
