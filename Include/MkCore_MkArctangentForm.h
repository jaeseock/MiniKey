#ifndef __MINIKEY_CORE_MKARCTANGENTFORM_H__
#define __MINIKEY_CORE_MKARCTANGENTFORM_H__

//------------------------------------------------------------------------------------------------//
// arctangent graph
// 범위지정을 통해 그래프 형태, 크기 결정
//
// ex> -1.0, 1.0 -> full graph
// ex> 0.0, 1.0 -> 후반부만
// ex> -0.2, 0.2 -> 일반적인 아크탄젠트 그래프 (기본 값)
//------------------------------------------------------------------------------------------------//

class MkArctangentForm
{
public:

	// frontRange (in) : 전반부 대상 범위(-1.0 ~ 0.0)
	// backRange (in) : 후반부 대상 범위(0.0 ~ 1.0)
	bool SetUp(float frontRange = -0.2f, float backRange = 0.2f);

	// x에 해당하는 y를 구함
	// x : 설정 된 그래프의 시작(0.0)과 끝(1.0) 사이의 값. 범위를 넘어설 경우 클램프
	// (out) : 0.0 ~ 1.0 의 그래프 값. 그래프 길이가 0일 경우(frontRange와 backRange간의 차가 0일 경우) 0 리턴
	float GetGraph(float x) const;

	MkArctangentForm() { SetUp(); }

protected:

	float m_FrontRange;
	float m_LengthOfRange;
	float m_BeginWeight;
	float m_EndWeight;
};

#endif
