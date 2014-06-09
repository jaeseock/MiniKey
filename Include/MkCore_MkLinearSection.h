#ifndef __MINIKEY_CORE_MKLINEARSECTION_H__
#define __MINIKEY_CORE_MKLINEARSECTION_H__


//------------------------------------------------------------------------------------------------//
// position, size�� ǥ�õǴ� ���� ����
// (NOTE) ������ ��� overflow�� ������� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalFunction.h"


template <class DataType>
class MkLinearSection
{
public:

	// �� ����
	virtual void SetUp(const DataType& position, const DataType& size)
	{
		m_Position = position;
		m_Size = size;
	}

	// ��ġ ��ȯ
	inline const DataType& GetPosition(void) const { return m_Position; }

	// ũ�� ��ȯ
	inline const DataType& GetSize(void) const { return m_Size; }

	// ���� ��ġ ��ȯ
	inline DataType GetEndPosition(void) const { return (m_Position + m_Size); }

	// ������ ���翩�� ��ȯ
	inline bool CheckIntersection(const MkLinearSection<DataType>& section) const
	{
		DataType srcEnd = GetEndPosition();
		DataType destEnd = section.GetEndPosition();

		return (CheckInclusion<DataType>(m_Position, section.GetPosition(), srcEnd) ||
			CheckInclusion<DataType>(section.GetPosition(), m_Position, destEnd));
	} 

	// ������ ������ ��ȯ
	// ��ȯ�� MkLinearSection�� size�� 0���� Ŭ ��� ���� �߻�
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

	// ������ size�� �ڽź��� ���� ��� �ڽ��� �������� ������ ����� ���ϵ��� ���ѵ� �� ��ġ ��ȯ
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

