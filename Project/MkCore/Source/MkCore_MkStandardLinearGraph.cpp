
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkStandardLinearGraph.h"


//------------------------------------------------------------------------------------------------//

void MkStandardLinearGraph::AddPoint(float x, float y)
{
	m_Table.Create(x, y);
}

void MkStandardLinearGraph::AddPointList(MkMap<float, float>& inputList)
{
	MkMapLooper<float, float> looper(inputList);
	MK_STL_LOOP(looper)
	{
		m_Table.Create(looper.GetCurrentKey(), looper.GetCurrentField());
	}
}

void MkStandardLinearGraph::SetAxisWeight(float x_weight, float y_weight)
{
	MkMap<float, float> buffer = m_Table;
	m_Table.Clear();

	MkMapLooper<float, float> looper(buffer);
	MK_STL_LOOP(looper)
	{
		m_Table.Create(looper.GetCurrentKey() * x_weight, looper.GetCurrentField() * y_weight);
	}
	buffer.Clear();
}

void MkStandardLinearGraph::Clear(void)
{
	m_Table.Clear();
}

float MkStandardLinearGraph::GetLastX(void) const
{
	return (m_Table.Empty()) ? 0.f : m_Table.GetLastKey();
}

float MkStandardLinearGraph::GetValue(float x, bool useCosineInterpolation) const
{
	if (m_Table.Empty())
		return 0.f;

	float lowerBound, upperBound;
	m_Table.GetBoundKey(x, lowerBound, upperBound);
	if (lowerBound == upperBound)
	{
		return m_Table[lowerBound]; // 키가 한개이거나 최대/최소 범위를 넘은 경우
	}
	
	float fraction = (x - lowerBound) / (upperBound - lowerBound);
	return (useCosineInterpolation) ?
		MkFloatOp::CosineInterpolate(m_Table[lowerBound], m_Table[upperBound], fraction) :
		MkFloatOp::LinearInterpolate(m_Table[lowerBound], m_Table[upperBound], fraction);
}

//------------------------------------------------------------------------------------------------//
