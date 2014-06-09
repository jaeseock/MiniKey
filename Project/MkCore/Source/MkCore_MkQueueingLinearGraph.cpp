
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkQueueingLinearGraph.h"


//------------------------------------------------------------------------------------------------//

void MkQueueingLinearGraph::SetClippingRange(float clippingRange)
{
	if ((clippingRange > 0.f) && (clippingRange < m_ClippingRange))
	{
		_ClipListByRange(clippingRange);
	}

	m_ClippingRange = clippingRange;
}

void MkQueueingLinearGraph::AddPoint(float x, float y)
{
	if ((!m_Table.Empty()) && (x <= m_Table.GetLastKey()))
		return;
	
	m_Table.Create(x, y);

	if (m_ClippingRange > 0.f)
	{
		_ClipListByRange(m_ClippingRange);
	}
}

void MkQueueingLinearGraph::SetAxisWeight(float x_weight, float y_weight)
{
	MkStandardLinearGraph::SetAxisWeight(x_weight, y_weight);

	// 동일 비율로 줄어들기 때문에 추가적인 clipping은 필요 없음
	if (m_ClippingRange > 0.f)
	{
		m_ClippingRange *= x_weight;
	}
}

//------------------------------------------------------------------------------------------------//

void MkQueueingLinearGraph::_ClipListByRange(float range)
{
	// 값이 두개 이상일 경우에만 진행
	if (m_Table.GetSize() < 2)
		return;

	// 제한 범위
	float lastKey = m_Table.GetLastKey();
	float limit = lastKey - range;

	// 값이 하나만 남거나 범위 안의 값이 발견될때까지 순회
	MkArray<float> retireList(m_Table.GetSize());
	MkMapLooper<float, float> looper(m_Table);
	MK_STL_LOOP(looper)
	{
		float currKey = looper.GetCurrentKey();
		if ((currKey < lastKey) && (currKey < limit))
		{
			retireList.PushBack(currKey);
		}
		else
			break;
	}

	// 삭제 값이 존재할 경우
	unsigned int count = retireList.GetSize();
	if (count > 0)
	{
		float lastX = retireList[count - 1];
		float lastY = m_Table[lastX];

		MK_INDEXING_LOOP(retireList, i)
		{
			m_Table.Erase(retireList[i]);
		}
		
		float nextX = m_Table.GetFirstKey(); // m_ClippingRange가 0보다 클 경우만 진입하므로 최소 하나의 값은 무조건 존재
		float nextY = m_Table[nextX];
		float clipY = MkFloatOp::GetLinearPosition(lastX, lastY, limit, nextX, nextY);
		m_Table.Create(limit, clipY);
	}
}

//------------------------------------------------------------------------------------------------//