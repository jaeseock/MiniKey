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
	inline bool CheckIntersection(const MkBitField32& bitSet) const { return ((m_Field & bitSet.m_Field) != 0); }

	// bitSet의 포함 여부 (bitSet의 모든 비트가 겹쳐야 true)
	inline bool CheckInclusion(const MkBitField32& bitSet) const { return ((m_Field & bitSet.m_Field) == bitSet.m_Field); }

	// 어떠한 비트라도 1이 세팅되어 있다면 true
	inline bool Any(void) const { return (m_Field != 0); }

	//------------------------------------------------------------------------------------------------//
	// value in bandwidth
	// bit field의 일정 대역폭을 정수값으로 할당
	//------------------------------------------------------------------------------------------------//

	// position 위치에서 bandwidth만큼의 bit field에 value를 삽입
	// (NOTE) value가 bandwidth를 넘어가는 값일 경우(ex> value=100, bandwidth=6(0~63)) overflow 발생
	inline void SetValue(unsigned int value, unsigned int position, unsigned int bandwidth)
	{
		assert((position >= 0) && (position < 32));
		assert((bandwidth > 0) && (bandwidth <= (32 - position)));
		assert(value <= ((0xffffffff << (32 - bandwidth)) >> (32 - bandwidth))); // overflow

		unsigned int bfUpperSide = ((m_Field >> (position + bandwidth)) << (position + bandwidth));
		unsigned int bfLowerSide = ((m_Field << (32 - position)) >> (32 - position));
		m_Field = bfUpperSide | bfLowerSide | (value << position);
	}

	// position 위치에서 bandwidth만큼의 bit field에 존재하는 value를 반환
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

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkBitField32)

//------------------------------------------------------------------------------------------------//
