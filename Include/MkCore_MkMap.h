#ifndef __MINIKEY_CORE_MKMAP_H__
#define __MINIKEY_CORE_MKMAP_H__


//------------------------------------------------------------------------------------------------//
// std::map<> wrapper
// 생성과 할당, 참조는 명확히 구분되어야함으로 std::map의 operator[]에서 삽입 기능 삭제
// 삽입은 Create()로만 가능
//
// (NOTE) 루핑 중 생성이나 삭제는 허용하지 않음
// (NOTE) 루핑 중 루핑을 원할 경우 별도의 looper 객체를 사용
// (NOTE) thread safe 하지 않음
//------------------------------------------------------------------------------------------------//
// ex>
//	MkMap<int, float> sample;
//	sample.Create(1, 10.f);
//	sample.Create(3, 30.f);
//	sample.Create(6, 60.f);
//	sample.Create(8, 80.f);
//
//	MkMapLooper<int, float> looper(sample); // const key, field
//	MK_STL_LOOP(looper)
//	{
//		int key = looper.GetCurrentKey();
//		float& field = looper.GetCurrentField();
//	}
//
//	MkConstMapLooper<int, float> looper(sample); // const key, const field
//	MK_STL_LOOP(looper)
//	{
//		int key = looper.GetCurrentKey();
//		float field = looper.GetCurrentField();
//	}
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkContainerDefinition.h"
#include <map>
#include "MkCore_MkArray.h"


template <class KeyType, class FieldType>
class MkMap
{
public:

	//------------------------------------------------------------------------------------------------//
	// 메모리 관리
	//------------------------------------------------------------------------------------------------//

	// 해제
	virtual void Clear(void) { if (!m_Pair.empty()) { m_Pair.clear(); } }

	//------------------------------------------------------------------------------------------------//
	// 값 참조
	//------------------------------------------------------------------------------------------------//

	// 컨테이너가 비었는지 여부
	inline bool Empty(void) const { return m_Pair.empty(); }

	// 현재 pair 개수 반환
	inline unsigned int GetSize(void) const { return static_cast<unsigned int>(m_Pair.size()); }

	// 해당 key-field pair 존재여부 판별
	inline bool Exist(const KeyType& key) const { return Empty() ? false : (m_Pair.find(key) != m_Pair.end()); }

	// 컨테이너에 담긴 pair들의 key 리스트를 buffer에 삽입
	// pair 개수 반환
	inline unsigned int GetKeyList(MkArray<KeyType>& buffer) const
	{
		if (m_Pair.empty())
			return 0;

		unsigned int size = GetSize();
		buffer.Reserve(size);
		for (std::map<KeyType, FieldType>::const_iterator itr = m_Pair.begin(); itr != m_Pair.end(); ++itr)
		{
			buffer.PushBack(itr->first);
		}
		return size;
	}

	// 컨테이너에 담긴 pair들의 field 리스트를 buffer에 삽입
	// pair 개수 반환
	inline unsigned int GetFieldList(MkArray<FieldType>& buffer) const
	{
		if (m_Pair.empty())
			return 0;

		unsigned int size = GetSize();
		buffer.Reserve(size);
		for (std::map<KeyType, FieldType>::const_iterator itr = m_Pair.begin(); itr != m_Pair.end(); ++itr)
		{
			buffer.PushBack(itr->second);
		}
		return size;
	}

	// 첫번째 pair의 key 반환
	inline const KeyType& GetFirstKey(void) const
	{
		assert(!Empty());
		std::map<KeyType, FieldType>::const_iterator itr = m_Pair.begin();
		return itr->first;
	}

	// 마지막 pair의 key 반환
	inline const KeyType& GetLastKey(void) const
	{
		assert(!Empty());
		std::map<KeyType, FieldType>::const_reverse_iterator itr = m_Pair.rbegin();
		return itr->first;
	}

	// key를 포함하는 구간의 키를 리턴 (lowerBoundKey <= key < upperBoundKey)
	// (NOTE) std::map의 lower_bound() / upper_bound()와 개념이 다름!!!
	// (NOTE) 멤버가 없을 경우 에러
	//
	// (lowerBoundKey == upperBoundKey)의 경우는 다음의 셋 중 하나임
	//	- 멤버가 하나일 경우 key 상관 없이 그 하나의 키가 리턴
	//	- key가 첫번째 키 이하일 경우(GetFirstKey() >= key) 첫번째 키 리턴
	//	- key가 마지막 키 이상일 경우(GetLastKey() <= key) 마지막 키 리턴
	//
	// ex> key (4, 6, 7, 10) 일 때,
	//		.GetBoundKey(1) -> 4, 4
	//		.GetBoundKey(12) -> 10, 10
	//		.GetBoundKey(5) -> 4, 6
	//		.GetBoundKey(8) -> 7, 10
	inline void GetBoundKey(const KeyType& key, KeyType& lowerBoundKey, KeyType& upperBoundKey) const
	{
		// std::map의 key조건 상 <=, >= operator가 존재
		const KeyType& firstKey = GetFirstKey();
		if (firstKey >= key)
		{
			lowerBoundKey = firstKey;
			upperBoundKey = firstKey;
			return;
		}

		const KeyType& lastKey = GetLastKey();
		if (lastKey <= key)
		{
			lowerBoundKey = lastKey;
			upperBoundKey = lastKey;
			return;
		}

		// 멤버가 하나일 경우 위 과정에서 리턴 됨
		// 따라서 이 시점까지 진행되는 조건은 최소 멤버가 두개 이상임
		// (lastKey <= key)조건이 제외되므로 upper_bound() 안전하게 사용가능(반드시 존재)
		std::map<KeyType, FieldType>::const_iterator itr = m_Pair.upper_bound(key);
		upperBoundKey = itr->first;

		// (firstKey >= key)조건이 제외되므로 --itr 안전하게 사용가능(반드시 존재)
		--itr;
		lowerBoundKey = itr->first;
	}

	// GetBoundKey()의 lowerBoundKey에 해당하는 필드 참조
	inline FieldType& GetLowerBoundField(const KeyType& key)
	{
		KeyType lowerBoundKey, upperBoundKey;
		GetBoundKey(key, lowerBoundKey, upperBoundKey);
		std::map<KeyType, FieldType>::iterator itr = m_Pair.find(lowerBoundKey);
		return itr->second;
	}

	inline const FieldType& GetLowerBoundField(const KeyType& key) const
	{
		KeyType lowerBoundKey, upperBoundKey;
		GetBoundKey(key, lowerBoundKey, upperBoundKey);
		std::map<KeyType, FieldType>::const_iterator itr = m_Pair.find(lowerBoundKey);
		return itr->second;
	}

	// 필드 참조
	// (NOTE) std::map의 operator[]와 달리 삽입을 할 수 없음
	inline FieldType& operator [] (const KeyType& key)
	{
		assert(!m_Pair.empty());
		std::map<KeyType, FieldType>::iterator itr = m_Pair.find(key);
		assert(itr != m_Pair.end());
		return itr->second;
	}

	inline const FieldType& operator [] (const KeyType& key) const
	{
		assert(!m_Pair.empty());
		std::map<KeyType, FieldType>::const_iterator itr = m_Pair.find(key);
		assert(itr != m_Pair.end());
		return itr->second;
	}

	//------------------------------------------------------------------------------------------------//
	// 값 추가
	//------------------------------------------------------------------------------------------------//

	// key-field pair 생성 후 생성된 필드 참조를 리턴
	// (NOTE) 생성자가 존재하는 type
	inline FieldType& Create(const KeyType& key)
	{
		assert(!Exist(key));
		FieldType newField;
		std::pair<std::map<KeyType, FieldType>::iterator, bool> rpair = m_Pair.insert(std::map<KeyType, FieldType>::value_type(key, newField));
		return rpair.first->second;
	}

	// key-field pair 삽입
	inline void Create(const KeyType& key, const FieldType& field)
	{
		Create(key) = field;
	}

	//------------------------------------------------------------------------------------------------//
	// 값 삭제
	//------------------------------------------------------------------------------------------------//

	// key-field pair 삭제
	inline void Erase(const KeyType& key)
	{
		if (Exist(key))
		{
			m_Pair.erase(key);
		}
	}

	// key-field pair list 삭제
	inline void Erase(const MkArray<KeyType>& keyList)
	{
		if (!keyList.Empty())
		{
			MK_INDEXING_LOOP(keyList, i)
			{
				m_Pair.erase(keyList[i]);
			}
		}
	}

	//------------------------------------------------------------------------------------------------//
	
	// reserved
	inline typename std::map<KeyType, FieldType>::iterator __GetBeginItr() { return m_Pair.begin(); }
	inline typename std::map<KeyType, FieldType>::iterator __GetEndItr() { return m_Pair.end(); }
	inline typename std::map<KeyType, FieldType>::const_iterator __GetBeginItr() const { return m_Pair.begin(); }
	inline typename std::map<KeyType, FieldType>::const_iterator __GetEndItr() const { return m_Pair.end(); }

	MkMap() {}
	virtual ~MkMap() { Clear(); }

protected:

	std::map<KeyType, FieldType> m_Pair;
};


//------------------------------------------------------------------------------------------------//
// looper
//------------------------------------------------------------------------------------------------//

template <class KeyType, class FieldType>
class MkMapLooper : public MkStlLooper<KeyType, FieldType, MkMap<KeyType, FieldType>, typename std::map<KeyType, FieldType>::iterator>
{
public:
	MkMapLooper(MkMap<KeyType, FieldType>& target) : MkStlLooper(target) {}
};

template <class KeyType, class FieldType>
class MkConstMapLooper : public MkConstStlLooper<KeyType, FieldType, MkMap<KeyType, FieldType>, typename std::map<KeyType, FieldType>::const_iterator>
{
public:
	MkConstMapLooper(const MkMap<KeyType, FieldType>& target) : MkConstStlLooper(target) {}
};

//------------------------------------------------------------------------------------------------//

#endif
