
#include "MkCore_MkMessageHistory.h"


//------------------------------------------------------------------------------------------------//

void MkMessageHistory::SetUp(unsigned int capacity)
{
	assert(capacity != 0xffffffff);

	MkScopedCriticalSection(m_CS);
	m_History.SetMaxSize(capacity);
	m_CurrentPosition = 0xffffffff;
}

void MkMessageHistory::Record(const MkStr& msg)
{
	MkScopedCriticalSection(m_CS);
	if (m_History.Empty() || (msg != m_History[m_History.GetSize() - 1]))
	{
		m_History.PushBack(msg);
		m_CurrentPosition = 0xffffffff;
	}
}

MkStr MkMessageHistory::StepBack(void)
{
	MkScopedCriticalSection(m_CS);
	if (m_History.Empty())
		return L"";

	if (m_CurrentPosition == 0xffffffff)
	{
		m_CurrentPosition = m_History.GetSize() - 1; // newest msg
	}
	else if (m_CurrentPosition > 0)
	{
		--m_CurrentPosition; // elder msg
	}
	return m_History[m_CurrentPosition];
}

bool MkMessageHistory::StepForward(MkStr& msg)
{
	MkScopedCriticalSection(m_CS);
	if (m_History.Empty() || (m_CurrentPosition == 0xffffffff))
		return false;

	if ((m_CurrentPosition >= 0) && (m_CurrentPosition < (m_History.GetSize() - 1)))
	{
		++m_CurrentPosition; // newer msg
	}
	msg = m_History[m_CurrentPosition];
	return true;
}

void MkMessageHistory::Clear(void)
{
	MkScopedCriticalSection(m_CS);
	m_History.Clear();
	m_CurrentPosition = 0xffffffff;
}

//------------------------------------------------------------------------------------------------//
