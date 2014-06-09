#ifndef __MINIKEY_CORE_MKHASHMAP_H__
#define __MINIKEY_CORE_MKHASHMAP_H__


//------------------------------------------------------------------------------------------------//
// CAtlMap wrapper
//
// �˻� ���̺�� �����̳ʷμ� stdext::hash_map�� �����ٴ� �̾߱Ⱑ ���� (http://minjang.egloos.com/1983788)
// ������ ���� ����� �ڷᵵ ���� (http://www.gpgstudy.com/forum/viewtopic.php?p=114564)
// ���� �׳��� �����ϴٴ� CAtlMap�� ����� ���� (http://jacking.tistory.com/343)
//
// std::map�� ���ϸ� ��뷮 ��ȸ�� ������ ��� ���۷��̼�(Ư�� ����)�� ����
// ��뷮 �ϰ� ���� - ���� ���� - �ϰ� ���� �����϶� ���� ����
//
// key�� ���ڿ��� ����� ��� MKHashMap<MKHashStr, ...> ������ ���� ����� MkMap<MkStr, ...> ���տ� ���� �ӵ��鿡�� ����
// MKHashStr ��������� ������ ���� ������ MkMap<MkStr, ...> ���տ� ���� �뷫,
// - insert : 30% ����
// - find : 15% ����(rehash�� �Ǿ� ������ �� �پ��)
// - erase : 20% ����
//
// �Ϻ� ���۷��̼Ǹ� ����. ����ȭ�� ���� SetOptimalLoad ���� �ʿ��� �� �߰�
//
// (NOTE) hash key�� ������ �� ���� �ڷ����� key�� �� �� ���� (ex> MkStr)
// (NOTE) ��� �ڷ����� �⺻ ������ �̿��� �����ڰ� ���ǵǾ� ������ ������ �⺻ �����ڸ� �����ؾ� ��� ����
// (NOTE) ��������ڰ� ������ ����
// (NOTE) thread safe ���� ����
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
	// ����ȭ, �޸� ����
	//------------------------------------------------------------------------------------------------//

	// bin�� �缳���Ͽ� �ʱ�ȭ
	// allocNow�� �ʱ�ȭ�� �޸� �Ҵ� ����. false�� ��� �ʿ��Ҷ� �Ҵ�ȴ�
	bool Init(unsigned int binCount = 17, bool allocNow = true)
	{
		m_Optimized = true;
		return m_Pair.InitHashTable(binCount, allocNow);
	}

	// �ڵ� ����ȭ
	inline void Rehash(void)
	{
		if (!m_Optimized)
		{
			m_Pair.Rehash(0);
			m_Optimized = true;
		}
	}

	// ����
	virtual void Clear(void)
	{
		if (!m_Pair.IsEmpty())
		{
			m_Pair.RemoveAll();
		}
		m_Optimized = true;
	}

	//------------------------------------------------------------------------------------------------//
	// �� ����
	//------------------------------------------------------------------------------------------------//

	// �����̳ʰ� ������� ����
	inline bool Empty(void) const { return m_Pair.IsEmpty(); }

	// ���� pair ���� ��ȯ
	inline unsigned int GetSize(void) const { return static_cast<int>(m_Pair.GetCount()); }

	// �ش� pair ���翩�� �Ǻ�
	inline bool Exist(const KeyType& key) const { return Empty() ? false : (m_Pair.Lookup(key) != NULL); }

	// �����̳ʿ� ��� pair���� key ����Ʈ�� buffer�� ����
	// pair ���� ��ȯ
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

	// �����̳ʿ� ��� pair���� field ����Ʈ�� buffer�� ����
	// pair ���� ��ȯ
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

	// pair�� key ��ȯ
	inline const KeyType& GetKey(POSITION pos) const
	{
		assert(pos != NULL);
		return m_Pair.GetKeyAt(pos);
	}

	// pair�� field ��ȯ
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

	// pair�� ù��° field ��ȯ
	inline FieldType& GetFirstField(void)
	{
		return GetField(m_Pair.GetStartPosition());
	}

	inline const FieldType& GetFirstField(POSITION pos) const
	{
		return GetField(m_Pair.GetStartPosition());
	}

	// (NOTE) std::map�� operator[]�� �޸� ������ �� �� ����
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
	// �� �߰�
	//------------------------------------------------------------------------------------------------//

	// pair ����
	inline void Create(const KeyType& key, const FieldType& field)
	{
		assert(!Exist(key));
		POSITION pos = m_Pair.SetAt(key, field);
		assert(pos != NULL);
		m_Optimized = false;
	}

	// pair ���� �� ������ �ʵ� ������ ����
	inline FieldType& Create(const KeyType& key)
	{
		assert(!Exist(key));
		FieldType newField;
		POSITION pos = m_Pair.SetAt(key, newField);
		assert(pos != NULL);
		m_Optimized = false;
		return m_Pair.GetValueAt(pos);
	}

	// ����(operator =)
	// ��Ģ������ hash map�� operator =�� ������� ������ �� �ʿ��� ��츦 ����� ������� ���� ����� �ο�
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
	// �� ����
	//------------------------------------------------------------------------------------------------//

	// pair ����
	inline void Erase(const KeyType& key)
	{
		m_Pair.RemoveKey(key);
		m_Optimized = false;
	}

	// pair list ����
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
	// ����
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
