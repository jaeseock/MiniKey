#ifndef __MINIKEY_CORE_MKLOCKABLE_H__
#define __MINIKEY_CORE_MKLOCKABLE_H__

//------------------------------------------------------------------------------------------------//
// 간단한 critical section 으로 둘러싸인 자료형 템플릿
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkCriticalSection.h"


template<class T>
class MkLockable
{
public:

	MkLockable() {}
	MkLockable(const T& value)
	{
		MkScopedCriticalSection(m_CS);
		m_Value = value;
	}

	inline void operator = (const T& value) // T& 반환 없음
	{
		MkScopedCriticalSection(m_CS);
		m_Value = value;
	}

	inline bool operator == (const T& value)
	{
		MkScopedCriticalSection(m_CS);
		return (m_Value == value);
	}

	inline bool operator != (const T& value)
	{
		MkScopedCriticalSection(m_CS);
		return (m_Value != value);
	}

	inline operator T()
	{
		MkScopedCriticalSection(m_CS);
		return m_Value;
	}

private:

	T m_Value;
    MkCriticalSection m_CS;
};

//------------------------------------------------------------------------------------------------//

#endif
