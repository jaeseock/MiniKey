#ifndef __MINIKEY_CORE_MKCRITICALSECTION_H__
#define __MINIKEY_CORE_MKCRITICALSECTION_H__

//------------------------------------------------------------------------------------------------//
// ������ critical section wrapper
//
// �������� ����Ѵٸ� deadlock�� ���ϱ� ���� lock�Ǵ� ���ҽ��� ������ �ڵ����� �˻��ϴ�
// ����� �־�� ������ �̴� ������� ������ ���ܵд�. ��� �� ������ ��Ƽ������ ������
// ����ؾ� �ϴ� ��Ȳ ��ü�� �߸��Ǿ��ٰ� ���� �����̴�
//------------------------------------------------------------------------------------------------//

// ex>
//	MkWrapInCriticalSection(m_CS)
//	{
//		���������� ���� �۾�
//	}
// 
#define MkWrapInCriticalSection(A) if (MkCriticalSection::_MkAutoLock __lockInstance = A) {} else

// ex>
//	method()
//	{
//		...
//		MkScopedCriticalSection(m_CS);
//		���������� ���� �۾�
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
		// ���� �ھ��� ���� �ڵ����� spin count�� 0���� �����ǹǷ� ���� ó�� �� �ʿ� ����
		// 4000�̶� ���� ������ ��ó�� Windows via C/C++���� ��Ʈ
		// ���ǵ��� ���� ����(����� ���� Ȯ�� ���н�)�� ������� ����
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
