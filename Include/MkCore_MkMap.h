#ifndef __MINIKEY_CORE_MKMAP_H__
#define __MINIKEY_CORE_MKMAP_H__


//------------------------------------------------------------------------------------------------//
// std::map<> wrapper
// ������ �Ҵ�, ������ ��Ȯ�� ���еǾ�������� std::map�� operator[]���� ���� ��� ����
// ������ Create()�θ� ����
//
// (NOTE) ���� �� �����̳� ������ ������� ����
// (NOTE) ���� �� ������ ���� ��� ������ looper ��ü�� ���
// (NOTE) thread safe ���� ����
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
	// �޸� ����
	//------------------------------------------------------------------------------------------------//

	// ����
	virtual void Clear(void) { if (!m_Pair.empty()) { m_Pair.clear(); } }

	//------------------------------------------------------------------------------------------------//
	// �� ����
	//------------------------------------------------------------------------------------------------//

	// �����̳ʰ� ������� ����
	inline bool Empty(void) const { return m_Pair.empty(); }

	// ���� pair ���� ��ȯ
	inline unsigned int GetSize(void) const { return static_cast<unsigned int>(m_Pair.size()); }

	// �ش� key-field pair ���翩�� �Ǻ�
	inline bool Exist(const KeyType& key) const { return Empty() ? false : (m_Pair.find(key) != m_Pair.end()); }

	// �����̳ʿ� ��� pair���� key ����Ʈ�� buffer�� ����
	// pair ���� ��ȯ
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

	// �����̳ʿ� ��� pair���� field ����Ʈ�� buffer�� ����
	// pair ���� ��ȯ
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

	// ù��° pair�� key ��ȯ
	inline const KeyType& GetFirstKey(void) const
	{
		assert(!Empty());
		std::map<KeyType, FieldType>::const_iterator itr = m_Pair.begin();
		return itr->first;
	}

	// ������ pair�� key ��ȯ
	inline const KeyType& GetLastKey(void) const
	{
		assert(!Empty());
		std::map<KeyType, FieldType>::const_reverse_iterator itr = m_Pair.rbegin();
		return itr->first;
	}

	// key�� �����ϴ� ������ Ű�� ���� (lowerBoundKey <= key < upperBoundKey)
	// (NOTE) std::map�� lower_bound() / upper_bound()�� ������ �ٸ�!!!
	// (NOTE) ����� ���� ��� ����
	//
	// (lowerBoundKey == upperBoundKey)�� ���� ������ �� �� �ϳ���
	//	- ����� �ϳ��� ��� key ��� ���� �� �ϳ��� Ű�� ����
	//	- key�� ù��° Ű ������ ���(GetFirstKey() >= key) ù��° Ű ����
	//	- key�� ������ Ű �̻��� ���(GetLastKey() <= key) ������ Ű ����
	//
	// ex> key (4, 6, 7, 10) �� ��,
	//		.GetBoundKey(1) -> 4, 4
	//		.GetBoundKey(12) -> 10, 10
	//		.GetBoundKey(5) -> 4, 6
	//		.GetBoundKey(8) -> 7, 10
	inline void GetBoundKey(const KeyType& key, KeyType& lowerBoundKey, KeyType& upperBoundKey) const
	{
		// std::map�� key���� �� <=, >= operator�� ����
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

		// ����� �ϳ��� ��� �� �������� ���� ��
		// ���� �� �������� ����Ǵ� ������ �ּ� ����� �ΰ� �̻���
		// (lastKey <= key)������ ���ܵǹǷ� upper_bound() �����ϰ� ��밡��(�ݵ�� ����)
		std::map<KeyType, FieldType>::const_iterator itr = m_Pair.upper_bound(key);
		upperBoundKey = itr->first;

		// (firstKey >= key)������ ���ܵǹǷ� --itr �����ϰ� ��밡��(�ݵ�� ����)
		--itr;
		lowerBoundKey = itr->first;
	}

	// GetBoundKey()�� lowerBoundKey�� �ش��ϴ� �ʵ� ����
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

	// �ʵ� ����
	// (NOTE) std::map�� operator[]�� �޸� ������ �� �� ����
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
	// �� �߰�
	//------------------------------------------------------------------------------------------------//

	// key-field pair ���� �� ������ �ʵ� ������ ����
	// (NOTE) �����ڰ� �����ϴ� type
	inline FieldType& Create(const KeyType& key)
	{
		assert(!Exist(key));
		FieldType newField;
		std::pair<std::map<KeyType, FieldType>::iterator, bool> rpair = m_Pair.insert(std::map<KeyType, FieldType>::value_type(key, newField));
		return rpair.first->second;
	}

	// key-field pair ����
	inline void Create(const KeyType& key, const FieldType& field)
	{
		Create(key) = field;
	}

	//------------------------------------------------------------------------------------------------//
	// �� ����
	//------------------------------------------------------------------------------------------------//

	// key-field pair ����
	inline void Erase(const KeyType& key)
	{
		if (Exist(key))
		{
			m_Pair.erase(key);
		}
	}

	// key-field pair list ����
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
