#ifndef __MINIKEY_CORE_MKPAIRARRAY_H__
#define __MINIKEY_CORE_MKPAIRARRAY_H__


//------------------------------------------------------------------------------------------------//
// key-field pair ������ �迭
// MkArray ����̱� ������ Ư���� �״�� ����
// array�� map�� �߰� ����
// - �޸� �������´� array ���(�ϰ� ����, ����, ������ ����)
// - map���� �޸� key�� ���� �ڵ� ���ĵ��� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


template <class KeyType, class FieldType>
class MkPairArray
{
public:

	// ũ�� ����
	inline void Reserve(unsigned int size)
	{
		m_Keys.Reserve(size);
		m_Fields.Reserve(size);
	}

	// �߰�Ȯ�����(Ȯ���� �ǰ� �Ҵ���� ���� �޸�)�� �ִٸ� ����
	inline void OptimizeMemory(void)
	{
		m_Keys.OptimizeMemory();
		m_Fields.OptimizeMemory();
	}

	// ����. ��� �޸� ����
	inline void Clear(void)
	{
		m_Keys.Clear();
		m_Fields.Clear();
	}

	// Ȯ���� pair ũ�� ��ȯ
	inline unsigned int GetAllocSize(void) const { return m_Keys.GetAllocSize(); }

	// �迭�� ������� ����
	inline bool Empty(void) const { return m_Keys.Empty(); }

	// �Ҵ�� pair ���� ��ȯ
	inline unsigned int GetSize(void) const { return m_Keys.GetSize(); }

	// ��ȿ�� �ε������� �Ǻ�
	inline bool IsValidIndex(unsigned int index) const { return m_Keys.IsValidIndex(index); }

	// ������ ��ȯ
	inline KeyType* GetKeyPtr(void) { return m_Keys.GetPtr(); }
	inline const KeyType* GetKeyPtr(void) const { return m_Keys.GetPtr(); }
	inline FieldType* GetFieldPtr(void) { return m_Fields.GetPtr(); }
	inline const FieldType* GetFieldPtr(void) const { return m_Fields.GetPtr(); }

	// key list ��ȯ
	const MkArray<KeyType>& GetKeys(void) const { return m_Keys; }

	// field list ��ȯ
	const MkArray<FieldType>& GetFields(void) const { return m_Fields; }

	// �迭�� section���� �ش� key ���Կ���
	inline bool KeyExist(const MkArraySection& section, const KeyType& key) const { return m_Keys.Exist(section, key); }

	// �迭�� section���� �ش� key�� �����ϴ� ���� ��ġ��, ������ MKDEF_ARRAY_ERROR ��ȯ
	inline unsigned int FindFirstKey(const MkArraySection& section, const KeyType& key) const { return m_Keys.FindFirstInclusion(section, key); }

	// �迭�� section���� �ش� key�� �����ϴ� ��� ��ġ�� positions�� ��� �˻��� ���� ��ȯ
	inline unsigned int FindAllKeys(const MkArraySection& section, const KeyType& key, MkArray<unsigned int>& positions) const { return m_Keys.FindAllInclusions(section, key, positions); }

	// �ش� ��ġ�� key ��ȯ
	inline const KeyType& GetKeyAt(unsigned int position) const
	{
		assert(IsValidIndex(position));
		return m_Keys[position];
	}

	// �ش� ��ġ�� field ��ȯ
	inline const FieldType& GetFieldAt(unsigned int position) const
	{
		assert(IsValidIndex(position));
		return m_Fields[position];
	}

	// key�� field ����
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

	// �Ҵ� ������ Ȯ���Ͽ� �� ũ�⸦ size�� ����
	inline void Fill(unsigned int size)
	{
		m_Keys.Fill(size);
		m_Fields.Fill(size);
	}

	// pair�� �ڿ� �߰�
	inline void PushBack(const KeyType& key, const FieldType& field)
	{
		m_Keys.PushBack(key);
		m_Fields.PushBack(field);
	}

	// pair�� �ڿ� �߰��ϰ� �� field ���� ��ȯ
	inline FieldType& PushBack(const KeyType& key)
	{
		m_Keys.PushBack(key);
		return m_Fields.PushBack();
	}

	// ��� pair ����. �Ҵ� �޸𸮴� ����
	inline void Flush(void)
	{
		m_Keys.Flush();
		m_Fields.Flush();
	}

	// �迭�� section���� key�� �ش��ϴ� pair ��� ����
	inline void Erase(const MkArraySection& section, const KeyType& key)
	{
		MkArray<unsigned int> positions;
		if (FindAllKeys(section, key, positions) > 0)
		{
			m_Keys.Erase(positions);
			m_Fields.Erase(positions);
		}
	}

	// �迭 ����(���� ����). �߰�Ȯ����� ����
	inline MkPairArray<KeyType, FieldType>& operator = (const MkPairArray<KeyType, FieldType>& srcArray)
	{
		m_Keys = srcArray.GetKeys();
		m_Fields = srcArray.GetFields();
		return *this;
	}

	// key�� �������� �������� ����(ex> 10, 9, 8, ...)
	// max heap sort : O(nlog2n)
	// (NOTE) type�� operator <, >= �� ���ǵǾ� �־�� ��
	inline void SortInDescendingOrder(void)
	{
		MkPairArraySorter<KeyType, FieldType> sorter;
		sorter.SortInDescendingOrder(m_Keys.GetPtr(), m_Fields.GetPtr(), m_Keys.GetSize());
	}

	// key�� �������� �������� ����(ex> 1, 2, 3, ...)
	// min heap sort : O(nlog2n)
	// (NOTE) type�� operator >, <= �� ���ǵǾ� �־�� ��
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

