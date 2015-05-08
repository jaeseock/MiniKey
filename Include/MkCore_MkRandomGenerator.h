#ifndef __MINIKEY_CORE_MKRANDOMGENERATOR_H__
#define __MINIKEY_CORE_MKRANDOMGENERATOR_H__

//------------------------------------------------------------------------------------------------//
// 랜덤 생성기
//
// Uniformly Distributed Random Number 생성 알고리즘
// 완벽한 uniform 난수는 아니지만 상당히 근접한 수치를 보임
// (가장 많이 나온 값과 적게 나온 값의 차이가 만번 테스트시 4%, 10만번 테스트시 0.5% 미만)
//
// 의사난수(seed가 동일하면 동일한 결과가 나옴)
//
// http://blog.yahoo.com/_XJA6C52DU5D4GVLPC7QW3R2EMM/articles/174940
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


class MkRandomGenerator
{
public:

	// 시드 설정(0 ~ 20000)
	void SetSeed(unsigned int seed);

	// 시드를 활용하여 0 이상, 1 미만의 난수를 반환하고 시드 갱신
	double GetRandomValue(void);

	// 0 이상, offset 미만의 난수 반환
	// offset은 2 이상이어야 함
	unsigned int GetRandomNumber(unsigned int offset);

	// 생성자
	MkRandomGenerator();
	MkRandomGenerator(unsigned int seed);

protected:

	// 시간에 따른 시드 얻기
	unsigned int _GetSeedFromTime(void) const;

protected:

	unsigned int m_Seed[3];
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkRandomGenerator)

//------------------------------------------------------------------------------------------------//

#endif
