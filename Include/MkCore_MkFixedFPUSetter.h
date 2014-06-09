#ifndef __MINIKEY_CORE_MKFIXEDFPUSETTER_H__
#define __MINIKEY_CORE_MKFIXEDFPUSETTER_H__

//------------------------------------------------------------------------------------------------//
// FPU를 단 밀도 부동소숫점 연산으로 모드 변환
// Intel, AMD간 fpu 단정밀 <-> 배정밀도 변환 차이로 인해 동기화에 문제가 생길 수 있는데 이를 해결함
//
// 조승구(NetKnife) 행님께 영광 있으라!!! (-_-)/
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStackPattern.h"


class MkFixedFPUSetter
{
public:

	// precision을 24 bit로 변경
	void FixPrecision(void);

	// 원 설정 복구
	void Restore(void);

	~MkFixedFPUSetter() { Restore(); }

protected:

	MkStackPattern<unsigned int> m_ControlWord;
};

#endif
