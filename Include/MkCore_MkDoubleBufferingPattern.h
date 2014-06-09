#ifndef __MINIKEY_CORE_MKDOUBLEBUFFERINGPATTERN_H__
#define __MINIKEY_CORE_MKDOUBLEBUFFERINGPATTERN_H__

//------------------------------------------------------------------------------------------------//
// double buffering pattern
// - producer-consumer 관계에 있는 두 thread 사이에서 크고 구성시간이 긴 객체를 주고 받을 때 사용
// - producer의 AddNewBuffer() 호출과 consumer의 GetCurrentBuffer() 호출은 비동기적임
//
// (NOTE) AddNewBuffer()에 입력되는 buffer는 항상 새로이 할당된 객체이어야 함
// (NOTE) 한 번 AddNewBuffer()를 통해 할당된 buffer 객체는 이후 참조 금지
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkCriticalSection.h"


template <class TargetClass>
class MkDoubleBufferingPattern
{
protected:

	enum eBufferIndex
	{
		eOld = 0,
		eNew
	};

public:

	// 새 buffer 기록
	inline void AddNewBuffer(TargetClass* newBuffer)
	{
		if (newBuffer != NULL)
		{
			MkScopedCriticalSection(m_CS);

			assert(newBuffer != m_Buffer[eOld]);
			if (newBuffer != m_Buffer[eOld])
			{
				TargetClass* lastBuffer = m_Buffer[eNew];
				if (lastBuffer != NULL)
				{
					delete lastBuffer;
				}

				m_Buffer[eNew] = newBuffer;
			}
		}
	}

	// 최신 buffer 반환
	inline TargetClass* GetCurrentBuffer(void)
	{
		MkScopedCriticalSection(m_CS);

		TargetClass* oldBuffer = m_Buffer[eOld];
		TargetClass* newBuffer = m_Buffer[eNew];

		if (newBuffer != NULL) // 새 buffer가 준비되어 있음
		{
			if (oldBuffer != NULL)
			{
				delete oldBuffer;
			}

			m_Buffer[eOld] = newBuffer;
			m_Buffer[eNew] = NULL;

			return newBuffer;
		}
		else if (oldBuffer != NULL) // 이전 buffer 재사용
		{
			return oldBuffer;
		}

		return NULL; // 준비된 buffer가 없음
	}

	// 해제
	inline void Clear(void)
	{
		MkScopedCriticalSection(m_CS);

		if (m_Buffer[eOld] != NULL)
		{
			delete m_Buffer[eOld];
			m_Buffer[eOld] = NULL;
		}
		if (m_Buffer[eNew] != NULL)
		{
			delete m_Buffer[eNew];
			m_Buffer[eNew] = NULL;
		}
	}

	// 사용가능여부
	inline bool IsValid(void)
	{
		MkScopedCriticalSection(m_CS);

		return ((m_Buffer[eOld] != NULL) || (m_Buffer[eNew] != NULL));
	}

public:

	MkDoubleBufferingPattern()
	{
		MkScopedCriticalSection(m_CS);

		m_Buffer[eOld] = NULL;
		m_Buffer[eNew] = NULL;
	}

	virtual ~MkDoubleBufferingPattern() { Clear(); }

protected:

	TargetClass* m_Buffer[2];
	MkCriticalSection m_CS;
};

//------------------------------------------------------------------------------------------------//

#endif
