#ifndef __MINIKEY_CORE_MKSTACKPATTERN_H__
#define __MINIKEY_CORE_MKSTACKPATTERN_H__

//------------------------------------------------------------------------------------------------//
// stack pattern
// first in last out(FILO)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


template <class TargetClass>
class MkStackPattern
{
public:

	inline void Reserve(unsigned int size)
	{
		m_Stack.Reserve(size);
	}

	virtual TargetClass& Push(void)
	{
		return m_Stack.PushBack();
	}

	virtual unsigned int Push(const TargetClass& instance)
	{
		unsigned int index = m_Stack.GetSize();
		m_Stack.PushBack(instance);
		return index;
	}

	virtual bool Pop(void)
	{
		bool ok = !m_Stack.Empty();
		if (ok)
		{
			m_Stack.PopBack(1);
		}
		return ok;
	}

	virtual bool Pop(TargetClass& buffer)
	{
		bool ok = !m_Stack.Empty();
		if (ok)
		{
			unsigned int index = m_Stack.GetSize() - 1;
			buffer = m_Stack[index];
			m_Stack.PopBack(1);
		}
		return ok;
	}

	virtual void Clear(void)
	{
		m_Stack.Clear();
	}

	inline unsigned int GetSize(void) const { return m_Stack.GetSize(); }

public:

	MkStackPattern() {}
	MkStackPattern(unsigned int size) { Reserve(size); }
	virtual ~MkStackPattern() { Clear(); }

protected:

	MkArray<TargetClass> m_Stack;
};

//------------------------------------------------------------------------------------------------//

#endif
