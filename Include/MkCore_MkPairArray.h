#ifndef __MINIKEY_CORE_MKPAIRARRAY_H__
#define __MINIKEY_CORE_MKPAIRARRAY_H__


//------------------------------------------------------------------------------------------------//
// key-field pair 형태의 배열
// MkArray 기반이기 때문에 특성도 그대로 따라감
// array와 map의 중간 형태
// - 메모리 관리형태는 array 방식(일괄 생성, 참조, 해제시 유리)
// - map과는 달리 key에 따라 자동 정렬되지 않음
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


template <class KeyType, class FieldType>
class MkPairArray
{
public:

	// 크기 예약
	inline void Reserve(unsigned int size)
	{
		m_Keys.Reserve(size);
		m_Fields.Reserve(size);
	}

	// 추가확장공간(확보만 되고 할당되지 않은 메모리)가 있다면 삭제
	inline void OptimizeMemory(void)
	{
		m_Keys.OptimizeMemory();
		m_Fields.OptimizeMemory();
	}

	// 해제. 모든 메모리 삭제
	inline void Clear(void)
	{
		m_Keys.Clear();
		m_Fields.Clear();
	}

	// 확보된 pair 크기 반환
	inline unsigned int GetAllocSize(void) const { return m_Keys.GetAllocSize(); }

	// 배열이 비었는지 여부
	inline bool Empty(void) const { return m_Keys.Empty(); }

	// 할당된 pair 개수 반환
	inline unsigned int GetSize(void) const { return m_Keys.GetSize(); }

	// 유효한 인덱스인지 판별
	inline bool IsValidIndex(unsigned int index) const { return m_Keys.IsValidIndex(index); }

	// 포인터 반환
	inline KeyType* GetKeyPtr(void) { return m_Keys.GetPtr(); }
	inline const KeyType* GetKeyPtr(void) const { return m_Keys.GetPtr(); }
	inline FieldType* GetFieldPtr(void) { return m_Fields.GetPtr(); }
	inline const FieldType* GetFieldPtr(void) const { return m_Fields.GetPtr(); }

	// key list 반환
	const MkArray<KeyType>& GetKeys(void) const { return m_Keys; }

	// field list 반환
	const MkArray<FieldType>& GetFields(void) const { return m_Fields; }

	// 배열의 section에서 해당 key 포함여부
	inline bool KeyExist(const MkArraySection& section, const KeyType& key) const { return m_Keys.Exist(section, key); }

	// 배열의 section에서 해당 key가 존재하는 최초 위치를, 없으면 MKDEF_ARRAY_ERROR 반환
	inline unsigned int FindFirstKey(const MkArraySection& section, const KeyType& key) const { return m_Keys.FindFirstInclusion(section, key); }

	// 배열의 section에서 해당 key가 존재하는 모든 위치를 positions에 담고 검색된 개수 반환
	inline unsigned int FindAllKeys(const MkArraySection& section, const KeyType& key, MkArray<unsigned int>& positions) const { return m_Keys.FindAllInclusions(section, key, positions); }

	// 해당 위치의 key 반환
	inline const KeyType& GetKeyAt(unsigned int position) const
	{
		assert(IsValidIndex(position));
		return m_Keys[position];
	}

	// 해당 위치의 field 반환
	inline const FieldType& GetFieldAt(unsigned int position) const
	{
		assert(IsValidIndex(position));
		return m_Fields[position];
	}

	// key로 field 참조
	inline FieldType& operator [] (const KeyType& key)
	{
		unsigned int pos = FindFirstKey(MkArraySection(0), key);
		assert(pos != MKDEF_ARRAY_ERROR);
		return m_Fields[pos];
	}

	inline const FieldType& operator [] (const KeyType& key) const
	{
		unsigned int pos = FindFirstKey(MkArraySection(0), key);
		assert(pos != MKDEF_ARRAY_ERROR);
		return m_Fields[pos];
	}

	// 할당 공간을 확장하여 값 크기를 size로 맞춤
	inline void Fill(unsigned int size)
	{
		m_Keys.Fill(size);
		m_Fields.Fill(size);
	}

	// pair를 뒤에 추가
	inline void PushBack(const KeyType& key, const FieldType& field)
	{
		m_Keys.PushBack(key);
		m_Fields.PushBack(field);
	}

	// pair를 뒤에 추가하고 빈 field 참조 반환
	inline FieldType& PushBack(const KeyType& key)
	{
		m_Keys.PushBack(key);
		return m_Fields.PushBack();
	}

	// 모든 pair 삭제. 할당 메모리는 유지
	inline void Flush(void)
	{
		m_Keys.Flush();
		m_Fields.Flush();
	}

	// 배열의 section에서 key에 해당하는 pair 모두 삭제
	inline void Erase(const MkArraySection& section, const KeyType& key)
	{
		MkArray<unsigned int> positions;
		if (FindAllKeys(section, key, positions) > 0)
		{
			m_Keys.Erase(positions);
			m_Fields.Erase(positions);
		}
	}

	// 배열 복사(깊은 복사). 추가확장공간 포함
	inline MkPairArray<KeyType, FieldType>& operator = (const MkPairArray<KeyType, FieldType>& srcArray)
	{
		m_Keys = srcArray.GetKeys();
		m_Fields = srcArray.GetFields();
		return *this;
	}

	// key를 기준으로 내림차순 정렬(ex> 10, 9, 8, ...)
	// max heap sort : O(nlog2n)
	// (NOTE) type은 operator <, >= 가 정의되어 있어야 함
	inline void SortInDescendingOrder(void)
	{
		MkPairArraySorter<KeyType, FieldType> sorter;
		sorter.SortInDescendingOrder(m_Keys.GetPtr(), m_Fields.GetPtr(), m_Keys.GetSize());
	}

	// key를 기준으로 오름차순 정렬(ex> 1, 2, 3, ...)
	// min heap sort : O(nlog2n)
	// (NOTE) type은 operator >, <= 가 정의되어 있어야 함
	inline void SortInAscendingOrder(void)
	{
		MkPairArraySorter<KeyType, FieldType> sorter;
		sorter.SortInAscendingOrder(m_Keys.GetPtr(), m_Fields.GetPtr(), m_Keys.GetSize());
	}

	MkPairArray() {}
	MkPairArray(unsigned int size) { Reserve(size); }
	virtual ~MkPairArray() {}

protected:

	MkArray<KeyType> m_Keys;
	MkArray<FieldType> m_Fields;
};

//------------------------------------------------------------------------------------------------//

#endif

