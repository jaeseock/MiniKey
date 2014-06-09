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
	inline bool CheckIntersection(const MkBitSet32& bitSet) const { return ((m_Field & bitSet.m_Field) != 0); }

	// bitSet의 포함 여부 (bitSet의 모든 비트가 겹쳐야 true)
	inline bool CheckInclusion(const MkBitSet32& bitSet) const { return ((m_Field & bitSet.m_Field) == bitSet.m_Field); }

	// 어떠한 비트라도 1이 세팅되어 있다면 true
	inline bool Any(void) const { return (m_Field != 0); }

public:

	unsigned int m_Field;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBitSet32)

//------------------------------------------------------------------------------------------------//

#endif
