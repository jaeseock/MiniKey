#ifndef __MINIKEY_CORE_MKDEQUE_H__
#define __MINIKEY_CORE_MKDEQUE_H__


//------------------------------------------------------------------------------------------------//
// std::deque<> wrapper
//
// �ִ� ũ�⸦ ������ �� ������ push�� ���ѵ� ũ�⸦ ���� ��� ����Ʈ
//
// (NOTE) thread safe ���� ����
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkContainerDefinition.h"
#include <deque>
#include <algorithm>


template <class DataType>
class MkDeque
{
public:

	//------------------------------------------------------------------------------------------------//
	// �ִ� ũ�� ����
	//------------------------------------------------------------------------------------------------//

	// �����̳��� �ִ� ũ�� ����
	// maxSize�� 0�� ��� ���� ����
	// (NOTE) maxSize�� 1 �̻��̰� ���� �����̳ʿ� ��� ũ�⺸�� ���� ��� �������� ������
	// ex> �����̳ʿ� �װ��� ���� �����ϰ� maxSize�� 2�� ��� ����°, �׹�° ���� ������
	inline void SetMaxSize(unsigned int maxSize)
	{
		m_MaxSize = maxSize;
		if ((m_MaxSize > 0) && (m_MaxSize < GetSize()))
		{
			m_Element.erase(m_Element.begin() + m_MaxSize, m_Element.end());
		}
	}

	// �����̳��� �ִ� ũ�� ��ȯ
	inline unsigned int GetMaxSize(void) const { return m_MaxSize; }

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// �����̳ʰ� ������� ����
	inline bool Empty(void) const { return m_Element.empty(); }

	// ���� element ���� ��ȯ
	inline unsigned int GetSize(void) const { return static_cast<unsigned int>(m_Element.size()); }

	// ��ȿ�� �ε������� �Ǻ�
	inline bool IsValidIndex(unsigned int index) const { return (index < GetSize()); }

	// �ش� �� ���翩�� �Ǻ�
	inline bool Exist(const DataType& value) const { return Empty() ? false : (std::find(m_Element.begin(), m_Element.end(), element) != m_Element.end()); }

	// �ش� ���� �����ϸ� ���� ��ġ��, ������ 0xffffffff ��ȯ
	inline unsigned int FindFirstInclusion(const DataType& element) const
	{
		if (m_Element.empty())
			return 0xffffffff;

		std::deque<DataType>::const_iterator itr = std::find(m_Element.begin(), m_Element.end(), element);
		return (itr == m_Element.end()) ? 0xffffffff : static_cast<unsigned int>(itr - m_Element.begin());
	}

	// offset ����
	inline DataType& operator [] (unsigned int offset)
	{
		assert(IsValidIndex(offset));
		return m_Element[offset];
	}

	inline const DataType& operator [] (unsigned int offset) const
	{
		assert(IsValidIndex(offset));
		return m_Element[offset];
	}

	//------------------------------------------------------------------------------------------------//
	// �� �߰�/����
	//------------------------------------------------------------------------------------------------//

	// �����̳� �տ� �� ���� �����ϰ� ���� ��ȯ
	// �ִ� ũ�� ���� ����
	inline DataType& PushFront(void)
	{
		if ((m_MaxSize > 0) && (m_MaxSize == GetSize()))
		{
			m_Element.pop_back();
		}
		DataType tmp;
		m_Element.push_front(tmp);
		return m_Element[0];
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
		if ((m_MaxSize > 0) && (m_MaxSize == GetSize()))
		{
			m_Element.pop_front();
		}
		DataType tmp;
		m_Element.push_back(tmp);
		return m_Element[GetSize() - 1];
	}

	// �����̳� �ڿ� �� ����
	// �ش� ���� �ε��� ��ȯ
	// �ִ� ũ�� ���� ����
	inline unsigned int PushBack(const DataType& value)
	{
		PushBack() = value;
		return (GetSize() - 1);
	}

	// ũ�� ����
	// ���� �ִ� ũ�Ⱑ ���ѵǾ� �ִµ� size�� �̺��� Ŭ ��� �ִ� ũ�⸦ size�� ����
	inline void Resize(unsigned int size, DataType defaultValue = DataType())
	{
		if ((m_MaxSize > 0) && (m_MaxSize < size))
		{
			m_MaxSize = size;
		}
		m_Element.resize(size, defaultValue);
	}

	// �����̳� �տ��� �ϳ��� �� ����
	inline void PopFront(void)
	{
		if (!Empty())
		{
			m_Element.pop_front();
		}
	}

	// �����̳� �ڿ��� �ϳ��� �� ����
	inline void PopBack(void)
	{
		if (!Empty())
		{
			m_Element.pop_back();
		}
	}

	// �ش� ��ġ�� ���Ҹ� ����
	inline void Erase(unsigned int index)
	{
		if (IsValidIndex(index))
		{
			m_Element.erase(m_Element.begin() + index);
		}
	}

	// ����
	inline void Clear(void) { if (!m_Element.empty()) { m_Element.clear(); } }

	MkDeque() { m_MaxSize = 0; }
	~MkDeque() { Clear(); }

protected:

	std::deque<DataType> m_Element;
	unsigned int m_MaxSize;
};

//------------------------------------------------------------------------------------------------//

#endif
