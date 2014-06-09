#ifndef __MINIKEY_CORE_MKBITSET32_H__
#define __MINIKEY_CORE_MKBITSET32_H__

//------------------------------------------------------------------------------------------------//
// 32 bit field
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkContainerDefinition.h"


class MkBitSet32
{
public:

	//------------------------------------------------------------------------------------------------//

	MkBitSet32() { Clear(); }
	MkBitSet32(const unsigned int& field) { m_Field = field; }
	MkBitSet32(const MkBitSet32& bitSet) { m_Field = bitSet.m_Field; }

	inline MkBitSet32& operator = (const MkBitSet32& bitSet)
	{
		m_Field = bitSet.m_Field;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//

	inline MkBitSet32 operator & (const MkBitSet32& bitSet) const { return MkBitSet32(m_Field & bitSet.m_Field); }
	inline MkBitSet32 operator | (const MkBitSet32& bitSet) const { return MkBitSet32(m_Field | bitSet.m_Field); }

	inline MkBitSet32& operator &= (const MkBitSet32& bitSet)
	{
		m_Field &= bitSet.m_Field;
		return *this;
	}

	inline MkBitSet32& operator |= (const MkBitSet32& bitSet)
	{
		m_Field |= bitSet.m_Field;
		return *this;
	}

	inline MkBitSet32 operator ~ () const { return MkBitSet32(~m_Field); }

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
	inline bool CheckIntersection(const MkBitSet32& bitSet) const { return ((m_Field & bitSet.m_Field) != 0); }

	// bitSet�� ���� ���� (bitSet�� ��� ��Ʈ�� ���ľ� true)
	inline bool CheckInclusion(const MkBitSet32& bitSet) const { return ((m_Field & bitSet.m_Field) == bitSet.m_Field); }

	// ��� ��Ʈ�� 1�� ���õǾ� �ִٸ� true
	inline bool Any(void) const { return (m_Field != 0); }

public:

	unsigned int m_Field;
};

//------------------------------------------------------------------------------------------------//

// ����ũ��
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBitSet32)

//------------------------------------------------------------------------------------------------//

#endif
