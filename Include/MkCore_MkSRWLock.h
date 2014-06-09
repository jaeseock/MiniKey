#ifndef __MINIKEY_CORE_MKSLIMREADERWRITERLOCK_H__
#define __MINIKEY_CORE_MKSLIMREADERWRITERLOCK_H__

//------------------------------------------------------------------------------------------------//
// 간단한 slim reader-writer lock wrapper
// critical section에 비해 사용 제한이 있지만 속도가 빠르다(심지어 Interlocked... 계열보다도)
//
// (NOTE) 최소 지원 플랫폼이 Windows Vista이다;;
// (NOTE) SRWL의 규칙대로, wtiter 스레드와 reader 스레드를 명확히 나눌 것!
// (NOTE) MkCriticalSection과 마찬가지로 deadlock대비 기능 없음
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
