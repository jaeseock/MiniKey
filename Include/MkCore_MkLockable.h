#ifndef __MINIKEY_CORE_MKLOCKABLE_H__
#define __MINIKEY_CORE_MKLOCKABLE_H__

//------------------------------------------------------------------------------------------------//
// ������ critical section ���� �ѷ����� �ڷ��� ���ø�
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

	inline void operator = (const T& value) // T& ��ȯ ����
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
