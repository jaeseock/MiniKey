#ifndef __MINIKEY_CORE_MKDOUBLEBUFFERINGPATTERN_H__
#define __MINIKEY_CORE_MKDOUBLEBUFFERINGPATTERN_H__

//------------------------------------------------------------------------------------------------//
// double buffering pattern
// - producer-consumer ���迡 �ִ� �� thread ���̿��� ũ�� �����ð��� �� ��ü�� �ְ� ���� �� ���
// - producer�� AddNewBuffer() ȣ��� consumer�� GetCurrentBuffer() ȣ���� �񵿱�����
//
// (NOTE) AddNewBuffer()�� �ԷµǴ� buffer�� �׻� ������ �Ҵ�� ��ü�̾�� ��
// (NOTE) �� �� AddNewBuffer()�� ���� �Ҵ�� buffer ��ü�� ���� ���� ����
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

	// �� buffer ���
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

	// �ֽ� buffer ��ȯ
	inline TargetClass* GetCurrentBuffer(void)
	{
		MkScopedCriticalSection(m_CS);

		TargetClass* oldBuffer = m_Buffer[eOld];
		TargetClass* newBuffer = m_Buffer[eNew];

		if (newBuffer != NULL) // �� buffer�� �غ�Ǿ� ����
		{
			if (oldBuffer != NULL)
			{
				delete oldBuffer;
			}

			m_Buffer[eOld] = newBuffer;
			m_Buffer[eNew] = NULL;

			return newBuffer;
		}
		else if (oldBuffer != NULL) // ���� buffer ����
		{
			return oldBuffer;
		}

		return NULL; // �غ�� buffer�� ����
	}

	// ����
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

	// ��밡�ɿ���
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
