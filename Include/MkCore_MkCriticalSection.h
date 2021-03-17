#ifndef __MINIKEY_CORE_MKCRITICALSECTION_H__
#define __MINIKEY_CORE_MKCRITICALSECTION_H__

//------------------------------------------------------------------------------------------------//
// 간단한 critical section wrapper
//
// 안정성을 고려한다면 deadlock을 피하기 위해 lock되는 리소스의 순서를 자동으로 검사하는
// 기능이 있어야 하지만 이는 사용자의 몫으로 남겨둔다. 사실 팀 전원이 멀티스레드 문제로
// 고민해야 하는 상황 자체가 잘못되었다고 보기 때문이다
//------------------------------------------------------------------------------------------------//

// ex>
//	MkWrapInCriticalSection(m_CS)
//	{
//		공유영역에 대한 작업
//	}
// 
#define MkWrapInCriticalSection(A) if (MkCriticalSection::_MkAutoLock __lockInstance = A) {} else

// ex>
//	method()
//	{
//		...
//		MkScopedCriticalSection(m_CS);
//		공유영역에 대한 작업
//	}
//
#define MkScopedCriticalSection(A) MkCriticalSection::_MkAutoLock __lockInstance = A

//------------------------------------------------------------------------------------------------//

#include <Windows.h>


class MkCriticalSection
{
public:
    MkCriticalSection()
	{
		// 단일 코어인 경우는 자동으로 spin count가 0으로 설정되므로 별도 처리 할 필요 없음
		// 4000이란 수는 제프리 리처의 Windows via C/C++에서 힌트
		// 정의되지 않은 에러(디버깅 영역 확보 실패시)는 고려하지 않음
		InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 4000);
	}

    ~MkCriticalSection()
	{
		DeleteCriticalSection(&m_CriticalSection);
	}

	inline CRITICAL_SECTION* GetCriticalSectionPtr(void) { return &m_CriticalSection; }

public:

	class _MkAutoLock
	{
	public:
		_MkAutoLock()
		{
			m_CSPtr = NULL;
		}

		_MkAutoLock(MkCriticalSection& cs)
		{
			m_CSPtr = cs.GetCriticalSectionPtr();
			EnterCriticalSection(m_CSPtr);
		}

		~_MkAutoLock()
		{
			if (m_CSPtr != NULL)
			{
				LeaveCriticalSection(m_CSPtr);
			}
		}

		inline operator bool() const
		{
			return false;
		}

	private:
		CRITICAL_SECTION* m_CSPtr;
	};

private:

	CRITICAL_SECTION m_CriticalSection;
};

//------------------------------------------------------------------------------------------------//

#endif
