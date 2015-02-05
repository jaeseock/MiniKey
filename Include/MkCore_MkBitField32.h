#pragma once

//------------------------------------------------------------------------------------------------//
// double word bit field
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkContainerDefinition.h"


class MkBitField32
{
public:

	//------------------------------------------------------------------------------------------------//

	MkBitField32() { Clear(); }
	MkBitField32(const unsigned int& field) { m_Field = field; }
	MkBitField32(const MkBitField32& bitSet) { m_Field = bitSet.m_Field; }

	inline MkBitField32& operator = (const MkBitField32& bitSet)
	{
		m_Field = bitSet.m_Field;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//

	inline MkBitField32 operator & (const MkBitField32& bitSet) const { return MkBitField32(m_Field & bitSet.m_Field); }
	inline MkBitField32 operator | (const MkBitField32& bitSet) const { return MkBitField32(m_Field | bitSet.m_Field); }

	inline MkBitField32& operator &= (const MkBitField32& bitSet)
	{
		m_Field &= bitSet.m_Field;
		return *this;
	}

	inline MkBitField32& operator |= (const MkBitField32& bitSet)
	{
		m_Field |= bitSet.m_Field;
		return *this;
	}

	inline MkBitField32 operator ~ () const { return MkBitField32(~m_Field); }

	//------------------------------------------------------------------------------------------------//
	// bit flag
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
	inline bool CheckIntersection(const MkBitField32& bitSet) const { return ((m_Field & bitSet.m_Field) != 0); }

	// bitSet�� ���� ���� (bitSet�� ��� ��Ʈ�� ���ľ� true)
	inline bool CheckInclusion(const MkBitField32& bitSet) const { return ((m_Field & bitSet.m_Field) == bitSet.m_Field); }

	// ��� ��Ʈ�� 1�� ���õǾ� �ִٸ� true
	inline bool Any(void) const { return (m_Field != 0); }

	//------------------------------------------------------------------------------------------------//
	// value in bandwidth
	// bit field�� ���� �뿪���� ���������� �Ҵ�
	//------------------------------------------------------------------------------------------------//

	// position ��ġ���� bandwidth��ŭ�� bit field�� value�� ����
	// (NOTE) value�� bandwidth�� �Ѿ�� ���� ���(ex> value=100, bandwidth=6(0~63)) overflow �߻�
	inline void SetValue(unsigned int value, unsigned int position, unsigned int bandwidth)
	{
		assert((position >= 0) && (position < 32));
		assert((bandwidth > 0) && (bandwidth <= (32 - position)));
		assert(value <= ((0xffffffff << (32 - bandwidth)) >> (32 - bandwidth))); // overflow

		unsigned int bfUpperSide = ((m_Field >> (position + bandwidth)) << (position + bandwidth));
		unsigned int bfLowerSide = ((m_Field << (32 - position)) >> (32 - position));
		m_Field = bfUpperSide | bfLowerSide | (value << position);
	}

	// position ��ġ���� bandwidth��ŭ�� bit field�� �����ϴ� value�� ��ȯ
	inline unsigned int GetValue(unsigned int position, unsigned int bandwidth) const
	{
		assert((position >= 0) && (position < 32));
		assert((bandwidth > 0) && (bandwidth <= (32 - position)));

		return ((m_Field << (32 - position - bandwidth)) >> (32 - bandwidth));
	}

public:

	unsigned int m_Field;
};

//------------------------------------------------------------------------------------------------//

// ����ũ��
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBitField32)

//------------------------------------------------------------------------------------------------//
