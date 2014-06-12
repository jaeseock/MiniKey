#pragma once

//------------------------------------------------------------------------------------------------//
// double word bit field
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkContainerDefinition.h"


class MkBitFieldDW
{
public:

	//------------------------------------------------------------------------------------------------//

	MkBitFieldDW() { Clear(); }
	MkBitFieldDW(const unsigned int& field) { m_Field = field; }
	MkBitFieldDW(const MkBitFieldDW& bitSet) { m_Field = bitSet.m_Field; }

	inline MkBitFieldDW& operator = (const MkBitFieldDW& bitSet)
	{
		m_Field = bitSet.m_Field;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//

	inline MkBitFieldDW operator & (const MkBitFieldDW& bitSet) const { return MkBitFieldDW(m_Field & bitSet.m_Field); }
	inline MkBitFieldDW operator | (const MkBitFieldDW& bitSet) const { return MkBitFieldDW(m_Field | bitSet.m_Field); }

	inline MkBitFieldDW& operator &= (const MkBitFieldDW& bitSet)
	{
		m_Field &= bitSet.m_Field;
		return *this;
	}

	inline MkBitFieldDW& operator |= (const MkBitFieldDW& bitSet)
	{
		m_Field |= bitSet.m_Field;
		return *this;
	}

	inline MkBitFieldDW operator ~ () const { return MkBitFieldDW(~m_Field); }

	//------------------------------------------------------------------------------------------------//

	// ��� 1�� ����
	inline void Set(void) { m_Field = 0xffffffff; }

	// position��ġ�� bit�� 1�� ����
	inline void Set(unsigned int position)
	{
		assert(position < 32);
		m_Field |= (1 << position);
	}

	// ��� 0���� ����
	inline void Clear(void) { m_Field = 0; }

	// position��ġ�� bit�� 0���� ����
	inline void Clear(unsigned int position)
	{
		assert(position < 32);
		m_Field &= ~(1 << position);
	}

	// position��ġ�� bit�� 0 Ȥ�� 1�� ����
	inline void Assign(unsigned int position, bool enable)
	{
		if (enable)
		{
			Set(position);
		}
		else
		{
			Clear(position);
		}
	}

	// position��ġ�� 1���� ����
	inline bool Check(unsigned int position) const
	{
		assert((position >= 0) && (position < 32));
		return ((m_Field & (1 << position)) != 0);
	}

	// position��ġ�� 1���� ����
	inline bool operator [] (unsigned int position) const
	{
		assert((position >= 0) && (position < 32));
		return ((m_Field & (1 << position)) != 0);
	}

	// bitSet�� ��ħ ���� (�Ϻ� ��Ʈ�� ��ġ�� true)
	inline bool CheckIntersection(const MkBitFieldDW& bitSet) const { return ((m_Field & bitSet.m_Field) != 0); }

	// bitSet�� ���� ���� (bitSet�� ��� ��Ʈ�� ���ľ� true)
	inline bool CheckInclusion(const MkBitFieldDW& bitSet) const { return ((m_Field & bitSet.m_Field) == bitSet.m_Field); }

	// ��� ��Ʈ�� 1�� ���õǾ� �ִٸ� true
	inline bool Any(void) const { return (m_Field != 0); }

public:

	unsigned int m_Field;
};

//------------------------------------------------------------------------------------------------//

// ����ũ��
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBitFieldDW)

//------------------------------------------------------------------------------------------------//
