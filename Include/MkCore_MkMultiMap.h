#ifndef __MINIKEY_CORE_MKMULTIMAP_H__
#define __MINIKEY_CORE_MKMULTIMAP_H__


//------------------------------------------------------------------------------------------------//
// std::multimap<> wrapper
//
// (NOTE) 루핑 중 생성이나 삭제는 허용하지 않음
// (NOTE) 루핑 중 루핑을 원할 경우 별도의 looper 객체를 사용
// (NOTE) thread safe 하지 않음
//------------------------------------------------------------------------------------------------//
// ex>
//	MkMultiMap<int, float> sample;
//	sample.Create(0, 0.f);
//	sample.Create(1, 10.f);
//	sample.Create(2, 20.f);
//	sample.Create(3, 30.f);
//	sample.Create(0, 40.f);
//	sample.Create(2, 50.f);
//	sample.Create(2, 80.f);
//
//	MkMultiMapLooper<int, float> looper(sample); // 전체순회 : const key, field
//	MK_STL_LOOP(looper)
//	{
//		int key = looper.GetCurrentKey();
//		float& field = looper.GetCurrentField();
//	}
//
//	MkMultiMapLooper<int, float> looper(sample, 2); // key(2) 부분순회 : const key, field
//	MK_STL_LOOP(looper)
//	{
//		int key = looper.GetCurrentKey();
//		float& field = looper.GetCurrentField();
//	}
//
//	MkConstMultiMapLooper<int, float> looper(sample); // 전체순회 : const key, const field
//	MK_STL_LOOP(looper)
//	{
//		int key = looper.GetCurrentKey();
//		float field = looper.GetCurrentField();
//	}
//
//	MkConstMultiMapLooper<int, float> looper(sample, 2); // key(2) 부분순회 : const key, const field
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
class MkMultiMap
{
public:

	//------------------------------------------------------------------------------------------------//
	// 메모리 관리
	//------------------------------------------------------------------------------------------------//

	// 해제
	inline void Clear(void) { if (!m_Pair.empty()) { m_Pair.clear(); } }

	//------------------------------------------------------------------------------------------------//
	// 값 참조
	//------------------------------------------------------------------------------------------------//

	// 컨테이너가 비었는지 여부
	inline bool Empty(void) const { return m_Pair.empty(); }

	// 현재 pair 개수 반환
	inline unsigned int GetSize(void) const { return static_cast<unsigned int>(m_Pair.size()); }

	// 해당 key를 가진 pair 개수 반환
	inline unsigned int GetSize(const KeyType& key) const { return static_cast<unsigned int>(m_Pair.count(key)); }

	// 해당 key를 가진 pair 존재여부 판별
	inline bool Exist(const KeyType& key) const { return Empty() ? false : (m_Pair.find(key) != m_Pair.end()); }

	// 컨테이너에 담긴 pair들의 key 리스트를 buffer에 삽입
	// (NOTE) 한 key에 복수의 field가 존재하더라도 key는 하나만 담김
	inline unsigned int GetKeyList(MkArray<KeyType>& buffer) const
	{
		if (m_Pair.empty())
			return 0;

		buffer.Reserve(GetSize());
		std::multimap<KeyType, FieldType>::const_iterator itr = m_Pair.begin();
		KeyType lastKey = itr->first;
		buffer.PushBack(lastKey);
		++itr;

		while (itr != m_Pair.end())
		{
			if (itr->first != lastKey)
			{
				lastKey = itr->first;
				buffer.PushBack(lastKey);
			}
			++itr;
		}
		return buffer.GetSize();
	}

	// 컨테이너에 담긴 pair들의 field 리스트를 buffer에 삽입
	inline unsigned int GetFieldList(MkArray<FieldType>& buffer) const
	{
		if (m_Pair.empty())
			return 0;

		unsigned int size = GetSize();
		buffer.Reserve(size);
		for (std::multimap<KeyType, FieldType>::const_iterator itr = m_Pair.begin(); itr != m_Pair.end(); ++itr)
		{
			buffer.PushBack(itr->second);
		}
		return size;
	}

	// 컨테이너에 담긴 해당 key를 가진 pair들의 field 리스트를 buffer에 삽입
	inline unsigned int GetFieldList(const KeyType& key, MkArray<FieldType>& buffer) const
	{
		if (m_Pair.empty())
			return 0;

		unsigned int keyCount = GetSize(key);
		if (keyCount == 0)
			return 0;

		buffer.Reserve(keyCount);
		std::multimap<KeyType, FieldType>::const_iterator bItr = m_Pair.lower_bound(key);
		std::multimap<KeyType, FieldType>::const_iterator eItr = m_Pair.upper_bound(key);
		for (std::multimap<KeyType, FieldType>::const_iterator itr = bItr; itr != eItr; ++itr)
		{
			buffer.PushBack(itr->second);
		}
		return keyCount;
	}

	// 첫번째 pair의 key 반환
	inline const KeyType& GetFirstKey(void) const
	{
		assert(!Empty());
		std::multimap<KeyType, FieldType>::const_iterator itr = m_Pair.begin();
		return itr->first;
	}

	// 마지막 pair의 key 반환
	inline const KeyType& GetLastKey(void) const
	{
		assert(!Empty());
		std::multimap<KeyType, FieldType>::const_reverse_iterator itr = m_Pair.rbegin();
		return itr->first;
	}

	//------------------------------------------------------------------------------------------------//
	// 값 추가
	//------------------------------------------------------------------------------------------------//

	// key-field pair 생성 후 생성된 필드 참조를 리턴
	inline FieldType& Create(const KeyType& key)
	{
		FieldType newField;
		std::multimap<KeyType, FieldType>::iterator itr = m_Pair.insert(std::multimap<KeyType, FieldType>::value_type(key, newField));
		return itr->second;
	}

	// key-field pair 삽입
	inline void Create(const KeyType& key, const FieldType& field)
	{
		Create(key) = field;
	}

	//------------------------------------------------------------------------------------------------//
	// 값 삭제
	//------------------------------------------------------------------------------------------------//

	// 해당 key를 가진 pair 모두 삭제
	inline void Erase(const KeyType& key)
	{
		if (Exist(key))
		{
			m_Pair.erase(key);
		}
	}

	// 해당 key를 가진 pair list 모두 삭제
	inline void Erase(const MkArray<KeyType>& keyList)
	{
		if (!keyList.Empty())
		{
			MK_INDEXING_LOOP(keyList, i)
			{
				Erase(keyList[i]);
			}
		}
	}

	// 해당 iterator pair 삭제
	inline void Erase(typename const std::multimap<KeyType, FieldType>::iterator& itr)
	{
		if (itr != m_Pair.end())
		{
			m_Pair.erase(itr);
		}
	}
	
	//------------------------------------------------------------------------------------------------//
	
	// reserved
	inline typename std::multimap<KeyType, FieldType>::iterator __GetBeginItr() { return m_Pair.begin(); }
	inline typename std::multimap<KeyType, FieldType>::iterator __GetEndItr() { return m_Pair.end(); }
	inline typename std::multimap<KeyType, FieldType>::const_iterator __GetBeginItr() const { return m_Pair.begin(); }
	inline typename std::multimap<KeyType, FieldType>::const_iterator __GetEndItr() const { return m_Pair.end(); }

	inline typename std::multimap<KeyType, FieldType>::iterator __GetBeginItr(const KeyType& key) { return m_Pair.lower_bound(key); }
	inline typename std::multimap<KeyType, FieldType>::iterator __GetEndItr(const KeyType& key) { return m_Pair.upper_bound(key); }
	inline typename std::multimap<KeyType, FieldType>::const_iterator __GetBeginItr(const KeyType& key) const { return m_Pair.lower_bound(key); }
	inline typename std::multimap<KeyType, FieldType>::const_iterator __GetEndItr(const KeyType& key) const { return m_Pair.upper_bound(key); }

	MkMultiMap() {}
	~MkMultiMap() { Clear(); }

protected:

	std::multimap<KeyType, FieldType> m_Pair;
};


//------------------------------------------------------------------------------------------------//
// looper
//------------------------------------------------------------------------------------------------//

template <class KeyType, class FieldType>
class MkMultiMapLooper : public MkStlLooper<KeyType, FieldType, MkMultiMap<KeyType, FieldType>, typename std::multimap<KeyType, FieldType>::iterator>
{
public:
	inline void SetUp(MkMultiMap<KeyType, FieldType>& target)
	{
		m_Itr = target.__GetBeginItr();
		m_End = target.__GetEndItr();
	}

	inline void SetUp(MkMultiMap<KeyType, FieldType>& target, const KeyType& key)
	{
		m_Itr = target.__GetBeginItr(key);
		m_End = target.__GetEndItr(key);
	}

	MkMultiMapLooper(MkMultiMap<KeyType, FieldType>& target) : MkStlLooper(target) {}
	MkMultiMapLooper(MkMultiMap<KeyType, FieldType>& target, const KeyType& key) : MkStlLooper(target) { SetUp(target, key); }
};

template <class KeyType, class FieldType>
class MkConstMultiMapLooper : public MkConstStlLooper<KeyType, FieldType, MkMultiMap<KeyType, FieldType>, typename std::multimap<KeyType, FieldType>::const_iterator>
{
public:
	inline void SetUp(const MkMultiMap<KeyType, FieldType>& target)
	{
		m_Itr = target.__GetBeginItr();
		m_End = target.__GetEndItr();
	}

	inline void SetUp(const MkMultiMap<KeyType, FieldType>& target, const KeyType& key)
	{
		m_Itr = target.__GetBeginItr(key);
		m_End = target.__GetEndItr(key);
	}

	MkConstMultiMapLooper(const MkMultiMap<KeyType, FieldType>& target) : MkConstStlLooper(target) {}
	MkConstMultiMapLooper(const MkMultiMap<KeyType, FieldType>& target, const KeyType& key) : MkConstStlLooper(target) { SetUp(target, key); }
};

//------------------------------------------------------------------------------------------------//

#endif
