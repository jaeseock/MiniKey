#ifndef __MINIKEY_CORE_MKSTANDARDLINEARGRAPH_H__
#define __MINIKEY_CORE_MKSTANDARDLINEARGRAPH_H__

//------------------------------------------------------------------------------------------------//
// 정적인 Line 형태의 2D 그래프
// 초기화는 구간값(x, y), 혹은 구간값들의 리스트
// x축 값을 넣어 선형/코사인 보간된 y 값을 추출
// 삽입 비용이 큰 대신 값 참조가 효율적임
//
// ex>
//	MkStandardLinearGraph lg;
//	lg.AddPoint(1, 10.f);
//	lg.AddPoint(2, 20.f);
//	lg.AddPoint(3, 30.f);
//	lg.AddPoint(4, 40.f);
//
//	float val = lg.GetValue(2.3f);
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"


class MkStandardLinearGraph
{
public:

	// 구간값(x, y)을 더함
	virtual void AddPoint(float x, float y);

	// 구간값(x, y) 리스트를 더함
	void AddPointList(MkMap<float, float>& inputList);

	// x, y축의 값을 일정 비율로 변경
	virtual void SetAxisWeight(float x_weight = 1.f, float y_weight = 1.f);

	// 해제
	void Clear(void);

	// x축의 마지막 값을 구함
	// 값 리스트가 비었을 경우 0.f 반환
	float GetLastX(void) const;

	// 입력된 x축의 값에 따라 보간된 y값을 반환
	// 값 리스트가 비었을 경우 0.f 반환
	// useCosineInterpolation : true일 경우 코사인 보간된 부드러운 값을 반환, false이면 선형보간 값을 반환
	float GetValue(float x, bool useCosineInterpolation = false) const;

	// empty
	inline bool Empty(void) const { return m_Table.Empty(); }

protected:

	MkMap<float, float> m_Table;
};

#endif
