#ifndef __MINIKEY_CORE_MKDEQUE_H__
#define __MINIKEY_CORE_MKDEQUE_H__


//------------------------------------------------------------------------------------------------//
// deque의 인터페이스를 갖춘 circular queue
//
// MkArray 기반
// 초기화시 필요한 모든 공간을 확보한 상태로 시작
// - 빠른 Push/Pop(Front/Back)
// - 중간 삽입/삭제 없음
// - max size를 채우고 있을 확률이 높을 경우 유용(pooling 등)
// - 별도의 SetUp()을 호출하지 않을 경우 MKDEF_DEFAULT_SIZE_IN_CIRCULAR_DEQUE를 사용
//
// (NOTE) thread safe 하지 않음
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


template <class DataType>
class MkCircularDeque
{
public:

	//------------------------------------------------------------------------------------------------//
	// 초기화
	//------------------------------------------------------------------------------------------------//

	// 최대 크기 설정으로 컨테이너 초기화
	// maxSize의 범위(1 ~ MKDEF_MAX_ARRAY_SIZE)가 잘못되거나 기존 초기화되어 있는 상태이면 실패
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

	// 컨테이너의 최대 크기 반환
	inline unsigned int GetMaxSize(void) const { return m_MaxSize; }

	//------------------------------------------------------------------------------------------------//
	// 참조
	//------------------------------------------------------------------------------------------------//

	// 컨테이너가 비었는지 여부
	inline bool Empty(void) const { return (m_Size == 0); }

	// 현재 element 개수 반환
	inline unsigned int GetSize(void) const { return m_Size; }

	// 유효한 인덱스인지 판별
	inline bool IsValidIndex(unsigned int index) const { return (index < m_Size); }

	// 해당 값 존재여부 판별
	inline bool Exist(const DataType& value) const { return Empty() ? false : m_Array.Exist(MkArraySection(0), value); }

	// 해당 값이 존재하면 최초 위치를, 없으면 MKDEF_ARRAY_ERROR 반환
	inline unsigned int FindFirstInclusion(const DataType& value) const { return m_Array.FindFirstInclusion(MkArraySection(0), value); }

	// offset 참조
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
	// 값 추가/삭제
	//------------------------------------------------------------------------------------------------//

	// 컨테이너 앞에 빈 값을 삽입하고 참조 반환
	// 최대 크기 제한 받음
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

	// 컨테이너 뒤에 값 삽입
	// 해당 값의 인덱스 반환
	// 최대 크기 제한 받음
	inline unsigned int PushBack(const DataType& value)
	{
		PushBack() = value;
		return (m_Size - 1);
	}

	// 컨테이너 앞에서 하나의 값 제거
	inline void PopFront(void)
	{
		if (m_Size > 0)
		{
			--m_Size;
			_IncreasePosition();
		}
	}

	// 컨테이너 뒤에서 하나의 값 제거
	inline void PopBack(void)
	{
		if (m_Size > 0)
		{
			--m_Size;
		}
	}

	// 해제
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
