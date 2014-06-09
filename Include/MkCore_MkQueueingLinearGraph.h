#ifndef __MINIKEY_CORE_MKQUEUEINGLINEARGRAPH_H__
#define __MINIKEY_CORE_MKQUEUEINGLINEARGRAPH_H__

//------------------------------------------------------------------------------------------------//
// 최신 값만 순차적으로 추가되는 Line 형태의 2D 그래프
// 추가되는 구간값(x, y)의 x는 항상 이전 x보다 커야 함
// x축 값을 넣어 선형/코사인 보간된 y 값을 추출
//
// 최신 x축 값을 기준으로 범위를 지정해 필요 없는 이전 데이터 자동 삭제하고 경계면에 보간된 값을 넣음
// ex>
//	리스트에 (1.f, 0.f), (2.f, 1.f), (4.f, 3.f), (4.5f, 2.f)가 들어 있고 범위가 5.f면,
//	새로운 (8.f, 2.5f)가 추가될 시 (1.f, 0.f), (2.f, 1.f)값은 삭제 되고 (3.f, 2.f)가 삽입 됨
//	또 (10.f, 0.f)가 추가되면 (3.f, 2.f), (4.f, 3.f), (4.5f, 2.f)가 삭제되고 (5.f, 2.1428571f)가 삽입 됨
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStandardLinearGraph.h"


class MkQueueingLinearGraph : public MkStandardLinearGraph
{
public:

	// 자동 삭제될 영역을 설정
	// 0.f 이하이면 자동 삭제 기능 사용 안 함
	void SetClippingRange(float clippingRange);
 
	// 구간값(x, y)을 더함
	// 반환값은 추가 성공여부
	// (NOTE) x는 항상 이전 더해진 값의 x보다 커야 함
	virtual void AddPoint(float x, float y);

	// x, y축의 값을 일정 비율로 변경
	// 자동 삭제 범위가 존재 할 경우 x_weight에 맞추어 재설정됨
	virtual void SetAxisWeight(float x_weight = 1.f, float y_weight = 1.f);

	MkQueueingLinearGraph() : MkStandardLinearGraph() { m_ClippingRange = 0.f; }

protected:

	void _ClipListByRange(float range);

protected:

	float m_ClippingRange;
};

#endif
