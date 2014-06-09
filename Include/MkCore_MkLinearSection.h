#ifndef __MINIKEY_CORE_MKLINEARSECTION_H__
#define __MINIKEY_CORE_MKLINEARSECTION_H__


//------------------------------------------------------------------------------------------------//
// position, size로 표시되는 선형 구간
// (NOTE) 정수의 경우 overflow를 고려하지 않음
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalFunction.h"


template <class DataType>
class MkLinearSection
{
public:

	// 값 설정
	virtual void SetUp(const DataType& position, const DataType& size)
	{
		m_Position = position;
		m_Size = size;
	}

	// 위치 반환
	inline const DataType& GetPosition(void) const { return m_Position; }

	// 크기 반환
	inline const DataType& GetSize(void) const { return m_Size; }

	// 종료 위치 반환
	inline DataType GetEndPosition(void) const { return (m_Position + m_Size); }

	// 교집합 존재여부 반환
	inline bool CheckIntersection(const MkLinearSection<DataType>& section) const
	{
		DataType srcEnd = GetEndPosition();
		DataType destEnd = section.GetEndPosition();

		return (CheckInclusion<DataType>(m_Position, section.GetPosition(), srcEnd) ||
			CheckInclusion<DataType>(section.GetPosition(), m_Position, destEnd));
	} 

	// 교집합 구간을 반환
	// 반환된 MkLinearSection의 size가 0보다 클 경우 교차 발생
	inline MkLinearSection<DataType> GetIntersection(const MkLinearSection<DataType>& section) const
	{
		DataType srcEnd = GetEndPosition();
		DataType destEnd = section.GetEndPosition();

		MkLinearSection<DataType> result;
		if (CheckInclusion<DataType>(m_Position, section.GetPosition(), srcEnd))
		{
			result.SetUp(section.GetPosition(), CheckInclusion<DataType>(m_Position, destEnd, srcEnd) ? section.GetSize() : (srcEnd - section.GetPosition()));
		}
		else if (CheckInclusion<DataType>(section.GetPosition(), m_Position, destEnd))
		{
			result.SetUp(m_Position, CheckInclusion<DataType>(section.GetPosition(), srcEnd, destEnd) ? m_Size : (destEnd - m_Position));
		}
		return result;
	}

	// 구간의 size가 자신보다 작을 경우 자신의 영역에서 구간이 벗어나지 못하도록 제한된 새 위치 반환
	inline DataType Confine(const MkLinearSection<DataType>& section) const
	{
		DataType newPos = section.GetPosition();

		if (section.GetSize() == m_Size)
		{
			newPos = m_Position;
		}
		else if (section.GetSize() < m_Size)
		{
			if (section.GetPosition() < m_Position)
			{
				newPos = m_Position;
			}
			else
			{
				DataType sectionEndPos = section.GetEndPosition();
				DataType thisEndPos = GetEndPosition();
				if (sectionEndPos > thisEndPos)
				{
					newPos -= (sectionEndPos - thisEndPos);
				}
			}
		}

		return newPos;
	}

	MkLinearSection()
	{
		m_Position = static_cast<DataType>(0);
		m_Size = static_cast<DataType>(0);
	}

	MkLinearSection(const MkLinearSection<unsigned int>& section)
	{
		m_Position = section.GetPosition();
		m_Size = section.GetSize();
	}

	MkLinearSection(const DataType& positionIn, const DataType& sizeIn)
	{
		m_Position = positionIn;
		m_Size = sizeIn;
	}

	virtual ~MkLinearSection() {}

protected:

	DataType m_Position;
	DataType m_Size;
};

//------------------------------------------------------------------------------------------------//

#endif

