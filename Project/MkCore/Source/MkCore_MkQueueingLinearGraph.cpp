
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

	// ���� ������ �پ��� ������ �߰����� clipping�� �ʿ� ����
	if (m_ClippingRange > 0.f)
	{
		m_ClippingRange *= x_weight;
	}
}

//------------------------------------------------------------------------------------------------//

void MkQueueingLinearGraph::_ClipListByRange(float range)
{
	// ���� �ΰ� �̻��� ��쿡�� ����
	if (m_Table.GetSize() < 2)
		return;

	// ���� ����
	float lastKey = m_Table.GetLastKey();
	float limit = lastKey - range;

	// ���� �ϳ��� ���ų� ���� ���� ���� �߰ߵɶ����� ��ȸ
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

	// ���� ���� ������ ���
	unsigned int count = retireList.GetSize();
	if (count > 0)
	{
		float lastX = retireList[count - 1];
		float lastY = m_Table[lastX];

		MK_INDEXING_LOOP(retireList, i)
		{
			m_Table.Erase(retireList[i]);
		}
		
		float nextX = m_Table.GetFirstKey(); // m_ClippingRange�� 0���� Ŭ ��츸 �����ϹǷ� �ּ� �ϳ��� ���� ������ ����
		float nextY = m_Table[nextX];
		float clipY = MkFloatOp::GetLinearPosition(lastX, lastY, limit, nextX, nextY);
		m_Table.Create(limit, clipY);
	}
}

//------------------------------------------------------------------------------------------------//