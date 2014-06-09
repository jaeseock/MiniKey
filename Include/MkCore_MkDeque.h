#ifndef __MINIKEY_CORE_MKDEQUE_H__
#define __MINIKEY_CORE_MKDEQUE_H__


//------------------------------------------------------------------------------------------------//
// std::deque<> wrapper
//
// 최대 크기를 지정할 수 있으며 push시 제한된 크기를 넘을 경우 쉬프트
//
// (NOTE) thread safe 하지 않음
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
	// 최대 크기 관리
	//------------------------------------------------------------------------------------------------//

	// 컨테이너의 최대 크기 설정
	// maxSize가 0일 경우 제한 없음
	// (NOTE) maxSize가 1 이상이고 현재 컨테이너에 담긴 크기보다 작을 경우 나머지를 삭제함
	// ex> 컨테이너에 네개의 값이 존재하고 maxSize가 2일 경우 세번째, 네번째 값은 삭제됨
	inline void SetMaxSize(unsigned int maxSize)
	{
		m_MaxSize = maxSize;
		if ((m_MaxSize > 0) && (m_MaxSize < GetSize()))
		{
			m_Element.erase(m_Element.begin() + m_MaxSize, m_Element.end());
		}
	}

	// 컨테이너의 최대 크기 반환
	inline unsigned int GetMaxSize(void) const { return m_MaxSize; }

	//------------------------------------------------------------------------------------------------//
	// 참조
	//------------------------------------------------------------------------------------------------//

	// 컨테이너가 비었는지 여부
	inline bool Empty(void) const { return m_Element.empty(); }

	// 현재 element 개수 반환
	inline unsigned int GetSize(void) const { return static_cast<unsigned int>(m_Element.size()); }

	// 유효한 인덱스인지 판별
	inline bool IsValidIndex(unsigned int index) const { return (index < GetSize()); }

	// 해당 값 존재여부 판별
	inline bool Exist(const DataType& value) const { return Empty() ? false : (std::find(m_Element.begin(), m_Element.end(), element) != m_Element.end()); }

	// 해당 값이 존재하면 최초 위치를, 없으면 0xffffffff 반환
	inline unsigned int FindFirstInclusion(const DataType& element) const
	{
		if (m_Element.empty())
			return 0xffffffff;

		std::deque<DataType>::const_iterator itr = std::find(m_Element.begin(), m_Element.end(), element);
		return (itr == m_Element.end()) ? 0xffffffff : static_cast<unsigned int>(itr - m_Element.begin());
	}

	// offset 참조
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
	// 값 추가/삭제
	//------------------------------------------------------------------------------------------------//

	// 컨테이너 앞에 빈 값을 삽입하고 참조 반환
	// 최대 크기 제한 받음
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

	// 컨테이너 앞에 값 삽입
	// 해당 값의 인덱스(PushFront의 경우 항상 0) 반환
	// 최대 크기 제한 받음
	inline unsigned int PushFront(const DataType& value)
	{
		PushFront() = value;
		return 0;
	}

	// 컨테이너 뒤에 빈 값을 삽입하고 참조 반환
	// 최대 크기 제한 받음
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

	// 컨테이너 뒤에 값 삽입
	// 해당 값의 인덱스 반환
	// 최대 크기 제한 받음
	inline unsigned int PushBack(const DataType& value)
	{
		PushBack() = value;
		return (GetSize() - 1);
	}

	// 크기 변경
	// 만약 최대 크기가 제한되어 있는데 size가 이보다 클 경우 최대 크기를 size로 갱신
	inline void Resize(unsigned int size, DataType defaultValue = DataType())
	{
		if ((m_MaxSize > 0) && (m_MaxSize < size))
		{
			m_MaxSize = size;
		}
		m_Element.resize(size, defaultValue);
	}

	// 컨테이너 앞에서 하나의 값 제거
	inline void PopFront(void)
	{
		if (!Empty())
		{
			m_Element.pop_front();
		}
	}

	// 컨테이너 뒤에서 하나의 값 제거
	inline void PopBack(void)
	{
		if (!Empty())
		{
			m_Element.pop_back();
		}
	}

	// 해당 위치의 원소를 삭제
	inline void Erase(unsigned int index)
	{
		if (IsValidIndex(index))
		{
			m_Element.erase(m_Element.begin() + index);
		}
	}

	// 해제
	inline void Clear(void) { if (!m_Element.empty()) { m_Element.clear(); } }

	MkDeque() { m_MaxSize = 0; }
	~MkDeque() { Clear(); }

protected:

	std::deque<DataType> m_Element;
	unsigned int m_MaxSize;
};

//------------------------------------------------------------------------------------------------//

#endif
