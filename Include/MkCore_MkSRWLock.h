#ifndef __MINIKEY_CORE_MKSLIMREADERWRITERLOCK_H__
#define __MINIKEY_CORE_MKSLIMREADERWRITERLOCK_H__

//------------------------------------------------------------------------------------------------//
// ������ slim reader-writer lock wrapper
// critical section�� ���� ��� ������ ������ �ӵ��� ������(������ Interlocked... �迭���ٵ�)
//
// (NOTE) �ּ� ���� �÷����� Windows Vista�̴�;;
// (NOTE) SRWL�� ��Ģ���, wtiter ������� reader �����带 ��Ȯ�� ���� ��!
// (NOTE) MkCriticalSection�� ���������� deadlock��� ��� ����
//------------------------------------------------------------------------------------------------//

#define MkWrapInWriterLock(A) if (MkSRWLock::_MkAutoWriterLock __wLockInstance = A) {} else
#define MkScopedWriterLock(A) MkSRWLock::_MkAutoWriterLock __wLockInstance = A

//------------------------------------------------------------------------------------------------//

#define MkWrapInReaderLock(A) if (MkSRWLock::_MkAutoReaderLock __rLockInstance = A) {} else
#define MkScopedReaderLock(A) MkSRWLock::_MkAutoReaderLock __rLockInstance = A

//------------------------------------------------------------------------------------------------//

#include <Windows.h>


class MkSRWLock
{
public:
    MkSRWLock()
	{
		InitializeSRWLock(&m_SRWLock);
	}

	inline SRWLOCK* GetSRWLockPtr(void) { return &m_SRWLock; }

public:

	// writer lock
	class _MkAutoWriterLock
	{
	public:
		_MkAutoWriterLock()
		{
			m_LockPtr = NULL;
		}

		_MkAutoWriterLock(MkSRWLock& lock)
		{
			m_LockPtr = lock.GetSRWLockPtr();
			AcquireSRWLockExclusive(m_LockPtr);
		}

		~_MkAutoWriterLock()
		{
			if (m_LockPtr != NULL)
			{
				ReleaseSRWLockExclusive(m_LockPtr);
			}
		}

		inline operator bool() const
		{
			return false;
		}

	private:
		SRWLOCK* m_LockPtr;
	};

	// reader lock
	class _MkAutoReaderLock
	{
	public:
		_MkAutoReaderLock()
		{
			m_LockPtr = NULL;
		}

		_MkAutoReaderLock(MkSRWLock& lock)
		{
			m_LockPtr = lock.GetSRWLockPtr();
			AcquireSRWLockShared(m_LockPtr);
		}

		~_MkAutoReaderLock()
		{
			if (m_LockPtr != NULL)
			{
				ReleaseSRWLockShared(m_LockPtr);
			}
		}

		inline operator bool() const
		{
			return false;
		}

	private:
		SRWLOCK* m_LockPtr;
	};

private:

    SRWLOCK m_SRWLock;
};

//------------------------------------------------------------------------------------------------//

#endif
