#ifndef __MINIKEY_CORE_MKDEQUE_H__
#define __MINIKEY_CORE_MKDEQUE_H__


//------------------------------------------------------------------------------------------------//
// deque�� �������̽��� ���� circular queue
//
// MkArray ���
// �ʱ�ȭ�� �ʿ��� ��� ������ Ȯ���� ���·� ����
// - ���� Push/Pop(Front/Back)
// - �߰� ����/���� ����
// - max size�� ä��� ���� Ȯ���� ���� ��� ����(pooling ��)
// - ������ SetUp()�� ȣ������ ���� ��� MKDEF_DEFAULT_SIZE_IN_CIRCULAR_DEQUE�� ���
//
// (NOTE) thread safe ���� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


template <class DataType>
class MkCircularDeque
{
public:

	//------------------------------------------------------------------------------------------------//
	// �ʱ�ȭ
	//------------------------------------------------------------------------------------------------//

	// �ִ� ũ�� �������� �����̳� �ʱ�ȭ
	// maxSize�� ����(1 ~ MKDEF_MAX_ARRAY_SIZE)�� �߸��ǰų� ���� �ʱ�ȭ�Ǿ� �ִ� �����̸� ����
	inline bool SetUp(unsigned int maxSize)
	{
		bool ok = ((m_MaxSize == 0) && MKDEF_CHECK_ARRAY_SIZE(maxSize));
		if (ok)
		{
			m_Array.Fill(maxSize);
			_Initialize(maxSize);
		}
		return ok;
	}

	// �����̳��� �ִ� ũ�� ��ȯ
	inline unsigned int GetMaxSize(void) const { return m_MaxSize; }

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// �����̳ʰ� ������� ����
	inline bool Empty(void) const { return (m_Size == 0); }

	// ���� element ���� ��ȯ
	inline unsigned int GetSize(void) const { return m_Size; }

	// ��ȿ�� �ε������� �Ǻ�
	inline bool IsValidIndex(unsigned int index) const { return (index < m_Size); }

	// �ش� �� ���翩�� �Ǻ�
	inline bool Exist(const DataType& value) const { return Empty() ? false : m_Array.Exist(MkArraySection(0), value); }

	// �ش� ���� �����ϸ� ���� ��ġ��, ������ MKDEF_ARRAY_ERROR ��ȯ
	inline unsigned int FindFirstInclusion(const DataType& value) const { return m_Array.FindFirstInclusion(MkArraySection(0), value); }

	// offset ����
	inline DataType& operator [] (unsigned int offset)
	{
		assert(IsValidIndex(offset));
		return m_Array[_GetIndex(offset)];
	}

	inline const DataType& operator [] (unsigned int offset) const
	{
		assert(IsValidIndex(offset));
		return m_Array[_GetIndex(offset)];
	}

	//------------------------------------------------------------------------------------------------//
	// �� �߰�/����
	//------------------------------------------------------------------------------------------------//

	// �����̳� �տ� �� ���� �����ϰ� ���� ��ȯ
	// �ִ� ũ�� ���� ����
	inline DataType& PushFront(void)
	{
		if (m_MaxSize == 0)
		{
			SetUp(MKDEF_DEFAULT_SIZE_IN_CIRCULAR_DEQUE);
			m_Size = 1;
		}		
		else if (m_Size < m_MaxSize)
		{
			++m_Size;
		}

		_DecreasePosition();
		return m_Array[m_Position];
	}

	// �����̳� �տ� �� ����
	// �ش� ���� �ε���(PushFront�� ��� �׻� 0) ��ȯ
	// �ִ� ũ�� ���� ����
	inline unsigned int PushFront(const DataType& value)
	{
		PushFront() = value;
		return 0;
	}

	// �����̳� �ڿ� �� ���� �����ϰ� ���� ��ȯ
	// �ִ� ũ�� ���� ����
	inline DataType& PushBack(void)
	{
		if (m_MaxSize == 0)
		{
			SetUp(MKDEF_DEFAULT_SIZE_IN_CIRCULAR_DEQUE);
			m_Size = 1;
		}
		else if (m_Size < m_MaxSize)
		{
			++m_Size;
		}
		else // (m_Size == m_MaxSize)
		{
			_IncreasePosition();
		}
		return m_Array[_GetIndex(m_Size-1)];
	}

	// �����̳� �ڿ� �� ����
	// �ش� ���� �ε��� ��ȯ
	// �ִ� ũ�� ���� ����
	inline unsigned int PushBack(const DataType& value)
	{
		PushBack() = value;
		return (m_Size - 1);
	}

	// �����̳� �տ��� �ϳ��� �� ����
	inline void PopFront(void)
	{
		if (m_Size > 0)
		{
			--m_Size;
			_IncreasePosition();
		}
	}

	// �����̳� �ڿ��� �ϳ��� �� ����
	inline void PopBack(void)
	{
		if (m_Size > 0)
		{
			--m_Size;
		}
	}

	// ����
	inline void Clear(void)
	{
		m_Array.Clear();
		_Initialize(0);
	}

	MkCircularDeque() { _Initialize(0); }
	MkCircularDeque(unsigned int maxSize) { SetUp(maxSize); }
	~MkCircularDeque() { Clear(); }

protected:

	inline void _Initialize(unsigned int maxSize)
	{
		m_MaxSize = maxSize;
		m_Position = 0;
		m_Size = 0;
	}

	inline void _IncreasePosition(void)
	{
		++m_Position;
		if (m_Position == m_MaxSize)
		{
			m_Position = 0;
		}
	}

	inline void _DecreasePosition(void)
	{
		--m_Position;
		if (m_Position == 0xffffffff)
		{
			m_Position = m_MaxSize - 1;
		}
	}

	inline unsigned int _GetIndex(unsigned int offset) const
	{
		unsigned int index = m_Position + offset;
		if (index >= m_MaxSize)
		{
			index -= m_MaxSize;
		}
		return index;
	}

protected:

	MkArray<DataType> m_Array;
	unsigned int m_MaxSize;
	unsigned int m_Position;
	unsigned int m_Size;
};

//------------------------------------------------------------------------------------------------//

#endif
