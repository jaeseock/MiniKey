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

	// 모두 1로 세팅
	inline void Set(void) { m_Field = 0xffffffff; }

	// position위치의 bit를 1로 세팅
	inline void Set(unsigned int position)
	{
		assert(position < 32);
		m_Field |= (1 << position);
	}

	// 모두 0으로 세팅
	inline void Clear(void) { m_Field = 0; }

	// position위치의 bit를 0으로 세팅
	inline void Clear(unsigned int position)
	{
		assert(position < 32);
		m_Field &= ~(1 << position);
	}

	// position위치의 bit를 0 혹은 1로 세팅
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

	// position위치가 1인지 여부
	inline bool Check(unsigned int position) const
	{
		assert((position >= 0) && (position < 32));
		return ((m_Field & (1 << position)) != 0);
	}

	// position위치가 1인지 여부
	inline bool operator [] (unsigned int position) const
	{
		assert((position >= 0) && (position < 32));
		return ((m_Field & (1 << position)) != 0);
	}

	// bitSet의 겹침 여부 (일부 비트가 겹치면 true)
	inline bool CheckIntersection(const MkBitFieldDW& bitSet) const { return ((m_Field & bitSet.m_Field) != 0); }

	// bitSet의 포함 여부 (bitSet의 모든 비트가 겹쳐야 true)
	inline bool CheckInclusion(const MkBitFieldDW& bitSet) const { return ((m_Field & bitSet.m_Field) == bitSet.m_Field); }

	// 어떠한 비트라도 1이 세팅되어 있다면 true
	inline bool Any(void) const { return (m_Field != 0); }

public:

	unsigned int m_Field;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBitFieldDW)

//------------------------------------------------------------------------------------------------//
