#ifndef __MINIKEY_CORE_MKHASHMAP_H__
#define __MINIKEY_CORE_MKHASHMAP_H__


//------------------------------------------------------------------------------------------------//
// CAtlMap wrapper
//
// 검색 테이블용 컨테이너로서 stdext::hash_map은 느리다는 이야기가 존재 (http://minjang.egloos.com/1983788)
// 실험을 통해 증명된 자료도 있음 (http://www.gpgstudy.com/forum/viewtopic.php?p=114564)
// 따라서 그나마 무난하다는 CAtlMap을 사용해 래핑 (http://jacking.tistory.com/343)
//
// std::map과 비교하면 대용량 순회를 제외한 모든 오퍼레이션(특히 참조)가 빠름
// 대용량 일괄 삽입 - 잦은 참조 - 일괄 삭제 조건일때 가장 유리
//
// key로 문자열을 사용할 경우 MKHashMap<MKHashStr, ...> 조합은 동일 기능인 MkMap<MkStr, ...> 조합에 비해 속도면에서 유리
// MKHashStr 유지비용을 제외한 순수 성능은 MkMap<MkStr, ...> 조합에 비해 대략,
// - insert : 30% 수준
// - find : 15% 수준(rehash가 되어 있으면 더 줄어듬)
// - erase : 20% 수준
//
// 일부 오퍼레이션만 래핑. 최적화를 위한 SetOptimalLoad 등은 필요할 때 추가
//
// (NOTE) hash key를 생성할 수 없는 자료형은 key가 될 수 없음 (ex> MkStr)
// (NOTE) 사용 자료형에 기본 생성자 이외의 생성자가 정의되어 있으면 무조건 기본 생성자를 정의해야 사용 가능
// (NOTE) 복사생성자가 허용되지 않음
// (NOTE) thread safe 하지 않음
//------------------------------------------------------------------------------------------------//
//	MkHashMap<int, float> sample;
//	sample.Create(0, 0.f);
//	sample.Create(1, 10.f);
//	sample.Create(2, 20.f);
//	sample.Create(3, 30.f);
//	sample.Rehash();
//
//	MkHashMapLooper<int, float> looper(sample); // const key, field
//	MK_STL_LOOP(looper)
//	{
//		int key = looper.GetCurrentKey();
//		float& field = looper.GetCurrentField();
//	}
//
//	MkConstHashMapLooper<int, float> looper(sample); // const key, const field
//	MK_STL_LOOP(looper)
//	{
//		int key = looper.GetCurrentKey();
//		float field = looper.GetCurrentField();
//	}
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkContainerDefinition.h"
#include <atlcoll.h>
#include "MkCore_MkArray.h"


template <class KeyType, class FieldType>
class MkHashMap
{
public:

	//------------------------------------------------------------------------------------------------//
	// 최적화, 메모리 관리
	//------------------------------------------------------------------------------------------------//

	// bin을 재설정하여 초기화
	// allocNow는 초기화시 메모리 할당 여부. false일 경우 필요할때 할당된다
	bool Init(unsigned int binCount = 17, bool allocNow = true)
	{
		m_Optimized = true;
		return m_Pair.InitHashTable(binCount, allocNow);
	}

	// 자동 최적화
	inline void Rehash(void)
	{
		if (!m_Optimized)
		{
			m_Pair.Rehash(0);
			m_Optimized = true;
		}
	}

	// 해제
	virtual void Clear(void)
	{
		if (!m_Pair.IsEmpty())
		{
			m_Pair.RemoveAll();
		}
		m_Optimized = true;
	}

	//------------------------------------------------------------------------------------------------//
	// 값 참조
	//------------------------------------------------------------------------------------------------//

	// 컨테이너가 비었는지 여부
	inline bool Empty(void) const { return m_Pair.IsEmpty(); }

	// 현재 pair 개수 반환
	inline unsigned int GetSize(void) const { return static_cast<int>(m_Pair.GetCount()); }

	// 해당 pair 존재여부 판별
	inline bool Exist(const KeyType& key) const { return Empty() ? false : (m_Pair.Lookup(key) != NULL); }

	// 컨테이너에 담긴 pair들의 key 리스트를 buffer에 삽입
	// pair 개수 반환
	inline unsigned int GetKeyList(MkArray<KeyType>& buffer) const
	{
		if (m_Pair.IsEmpty())
			return 0;

		unsigned int size = GetSize();
		buffer.Reserve(size);
		POSITION pos = m_Pair.GetStartPosition();
		while (pos != NULL)
		{
			const CAtlMap<KeyType, FieldType>::CPair* pair = m_Pair.GetNext(pos);
			buffer.PushBack(pair->m_key);
		}
		return size;
	}

	// 컨테이너에 담긴 pair들의 field 리스트를 buffer에 삽입
	// pair 개수 반환
	inline unsigned int GetFieldList(MkArray<FieldType>& buffer) const
	{
		if (m_Pair.IsEmpty())
			return 0;

		unsigned int size = GetSize();
		buffer.Reserve(size);
		POSITION pos = m_Pair.GetStartPosition();
		while (pos != NULL)
		{
			const CAtlMap<KeyType, FieldType>::CPair* pair = m_Pair.GetNext(pos);
			buffer.PushBack(pair->m_value);
		}
		return size;
	}

	// pair의 key 반환
	inline const KeyType& GetKey(POSITION pos) const
	{
		assert(pos != NULL);
		return m_Pair.GetKeyAt(pos);
	}

	// pair의 field 반환
	inline FieldType& GetField(POSITION pos)
	{
		assert(pos != NULL);
		return m_Pair.GetValueAt(pos);
	}

	inline const FieldType& GetField(POSITION pos) const
	{
		assert(pos != NULL);
		return m_Pair.GetValueAt(pos);
	}

	// pair의 첫번째 field 반환
	inline FieldType& GetFirstField(void)
	{
		return GetField(m_Pair.GetStartPosition());
	}

	inline const FieldType& GetFirstField(POSITION pos) const
	{
		return GetField(m_Pair.GetStartPosition());
	}

	// (NOTE) std::map의 operator[]와 달리 삽입을 할 수 없음
	inline FieldType& operator [] (const KeyType& key)
	{
		assert(!m_Pair.IsEmpty());
		CAtlMap<KeyType, FieldType>::CPair* pair = m_Pair.Lookup(key);
		assert(pair != NULL);
		return pair->m_value;
	}

	inline const FieldType& operator [] (const KeyType& key) const
	{
		assert(!m_Pair.IsEmpty());
		const CAtlMap<KeyType, FieldType>::CPair* pair = m_Pair.Lookup(key);
		assert(pair != NULL);
		return pair->m_value;
	}

	//------------------------------------------------------------------------------------------------//
	// 값 추가
	//------------------------------------------------------------------------------------------------//

	// pair 삽입
	inline void Create(const KeyType& key, const FieldType& field)
	{
		assert(!Exist(key));
		POSITION pos = m_Pair.SetAt(key, field);
		assert(pos != NULL);
		m_Optimized = false;
	}

	// pair 생성 후 생성된 필드 참조를 리턴
	inline FieldType& Create(const KeyType& key)
	{
		assert(!Exist(key));
		FieldType newField;
		POSITION pos = m_Pair.SetAt(key, newField);
		assert(pos != NULL);
		m_Optimized = false;
		return m_Pair.GetValueAt(pos);
	}

	// 복사(operator =)
	// 원칙적으로 hash map은 operator =를 허용하지 않지만 꼭 필요할 경우를 대비해 명시적인 복사 기능을 부여
	inline void CopyTo(MkHashMap<KeyType, FieldType>& target) const
	{
		if (!Empty())
		{
			for (POSITION pos = m_Pair.GetStartPosition(); pos != NULL; m_Pair.GetNext(pos))
			{
				target.Create(GetKey(pos), GetField(pos));
			}
		}
	}

	//------------------------------------------------------------------------------------------------//
	// 값 삭제
	//------------------------------------------------------------------------------------------------//

	// pair 삭제
	inline void Erase(const KeyType& key)
	{
		m_Pair.RemoveKey(key);
		m_Optimized = false;
	}

	// pair list 삭제
	inline void Erase(const MkArray<KeyType>& keyList)
	{
		if (!keyList.Empty())
		{
			MK_INDEXING_LOOP(keyList, i)
			{
				m_Pair.RemoveKey(keyList[i]);
			}
			m_Optimized = false;
		}
	}

	//------------------------------------------------------------------------------------------------//
	// 루핑
	//------------------------------------------------------------------------------------------------//

	inline POSITION __GetStartPosition(void) const { return m_Pair.GetStartPosition(); }
	inline void __GetNext(POSITION& pos) const { m_Pair.GetNext(pos); }

	//------------------------------------------------------------------------------------------------//

	MkHashMap() { Init(); }
	virtual ~MkHashMap() { Clear(); }

protected:

	CAtlMap<KeyType, FieldType> m_Pair;
	bool m_Optimized;
};

//------------------------------------------------------------------------------------------------//
// looper
//------------------------------------------------------------------------------------------------//

template <class KeyType, class FieldType>
class MkHashMapLooper
{
public:
	inline void SetUp(MkHashMap<KeyType, FieldType>& target)
	{
		m_Target = &target;
		m_Position = m_Target->__GetStartPosition();
	}

	inline const KeyType& GetCurrentKey(void) const
	{
		assert(m_Target != NULL);
		assert(m_Position != NULL);
		return m_Target->GetKey(m_Position);
	}

	inline FieldType& GetCurrentField(void) const
	{
		assert(m_Target != NULL);
		assert(m_Position != NULL);
		return m_Target->GetField(m_Position);
	}

	inline bool __CheckCondition(void)
	{
		return (m_Position != NULL);
	}

	inline void __MoveToNext(void)
	{
		assert(m_Target != NULL);
		m_Target->__GetNext(m_Position);
	}

	MkHashMapLooper(MkHashMap<KeyType, FieldType>& target) { SetUp(target); }

private:

	MkHashMap<KeyType, FieldType>* m_Target;
	POSITION m_Position;
};

template <class KeyType, class FieldType>
class MkConstHashMapLooper
{
public:
	inline void SetUp(const MkHashMap<KeyType, FieldType>& target)
	{
		m_Target = &target;
		m_Position = m_Target->__GetStartPosition();
	}

	inline const KeyType& GetCurrentKey(void) const
	{
		assert(m_Target != NULL);
		assert(m_Position != NULL);
		return m_Target->GetKey(m_Position);
	}

	inline const FieldType& GetCurrentField(void) const
	{
		assert(m_Target != NULL);
		assert(m_Position != NULL);
		return m_Target->GetField(m_Position);
	}

	inline bool __CheckCondition(void)
	{
		return (m_Position != NULL);
	}

	inline void __MoveToNext(void)
	{
		assert(m_Target != NULL);
		m_Target->__GetNext(m_Position);
	}

	MkConstHashMapLooper(const MkHashMap<KeyType, FieldType>& target) { SetUp(target); }

private:

	const MkHashMap<KeyType, FieldType>* m_Target;
	POSITION m_Position;
};

//------------------------------------------------------------------------------------------------//

#endif
