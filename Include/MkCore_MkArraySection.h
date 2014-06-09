#ifndef __MINIKEY_CORE_MKARRAYSECTION_H__
#define __MINIKEY_CORE_MKARRAYSECTION_H__


//------------------------------------------------------------------------------------------------//
// MkArray ������ �����ϴ� descriptor
// unsigned int base�̱� ������ ��Ģ������ ������ 0 ~ 4294967295(0xffffffff)�̾�� ������,
// ������ overflow�� ���ϱ� ���� ������ 0 ~ 2147483647(0x7fffffff, MKDEF_MAX_ARRAY_SIZE)�� ������
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkLinearSection.h"


class MkArraySection : public MkLinearSection<unsigned int>
{
public:

	// position, size ����
	virtual void SetUp(const unsigned int& position, const unsigned int& size)
	{
		MkLinearSection<unsigned int>::SetUp(position, size);
		_Clamp();
	}

	// position, size(position ���� ����) ����
	inline void SetUp(const unsigned int& position) { SetUp(position, MKDEF_MAX_ARRAY_SIZE); }

	// �־��� array size�� �°� �缳��
	inline MkArraySection GetArraySection(unsigned int arraySize) const
	{
		MkArraySection section;
		if (m_Position < arraySize)
		{
			section.SetUp(m_Position, GetMin<unsigned int>(arraySize - m_Position, m_Size));
		}
		return section;
	}

	MkArraySection() : MkLinearSection<unsigned int>() {}
	MkArraySection(const unsigned int& position) : MkLinearSection<unsigned int>(position, MKDEF_MAX_ARRAY_SIZE) { _Clamp(); }
	MkArraySection(const MkLinearSection<unsigned int>& section) : MkLinearSection<unsigned int>(section) { _Clamp(); }
	MkArraySection(const unsigned int& positionIn, const unsigned int& sizeIn) : MkLinearSection<unsigned int>(positionIn, sizeIn) { _Clamp(); }

	virtual ~MkArraySection() {}

protected:

	inline void _Clamp(void)
	{
		assert(m_Position < MKDEF_MAX_ARRAY_SIZE);
		if (m_Position >= MKDEF_MAX_ARRAY_SIZE)
		{
			m_Position -= MKDEF_MAX_ARRAY_SIZE;
		}

		unsigned int availableSize = MKDEF_MAX_ARRAY_SIZE - m_Position; // MKDEF_MAX_ARRAY_SIZE ~ 1
		if (m_Size > availableSize)
		{
			m_Size = availableSize;
		}
	}
};

//------------------------------------------------------------------------------------------------//

#endif

