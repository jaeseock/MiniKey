#ifndef __MINIKEY_CORE_MKMEMORYBLOCKDESCRIPTOR_H__
#define __MINIKEY_CORE_MKMEMORYBLOCKDESCRIPTOR_H__

//------------------------------------------------------------------------------------------------//
// memory block�� ���� ����
//------------------------------------------------------------------------------------------------//

#include <assert.h>
#include "MkCore_MkArraySection.h"


template <class DataType>
class MkMemoryBlockDescriptor
{
public:

	// �ʱ�ȭ
	// (NOTE) ��ȿ�� üũ ���� ����
	inline void SetUp(DataType* ptr, unsigned int size) { m_Ptr = ptr; m_Size = size; }
	inline void SetUp(const DataType* ptr, unsigned int size) { SetUp(const_cast<DataType*>(ptr), size); }
	inline void SetUp(DataType* ptr, unsigned int offset, unsigned int size) { SetUp(&ptr[offset], size); }
	inline void SetUp(const DataType* ptr, unsigned int offset, unsigned int size) { SetUp(const_cast<DataType*>(ptr), offset, size); }
	inline void SetUp(DataType* ptr, const MkArraySection& section) { SetUp(&ptr[section.GetPosition()], section.GetSize()); }
	inline void SetUp(const DataType* ptr, const MkArraySection& section) { SetUp(const_cast<DataType*>(ptr), section.GetPosition(), section.GetSize()); }
	inline void SetUp(const DataType& instance) { SetUp(&instance, 1); }

	// ����ȯ
	inline operator DataType*() { return m_Ptr; }
	inline operator const DataType*() const { return m_Ptr; }

	inline DataType* GetPtr(void) { return m_Ptr; }
	inline const DataType* GetPtr(void) const { return m_Ptr; }

	// ũ�� ��ȯ
	inline unsigned int GetSize(void) const { return m_Size; }
	
	// ��밡�ɿ���
	inline bool IsValid(void) const { return ((m_Ptr != 0) && (m_Size > 0)); }

	// ���� ��ü���� ��ȯ
	inline DataType& GetAt(unsigned int offset)
	{
		assert(IsValid());
		assert(offset < m_Size);
		return m_Ptr[offset];
	}

	inline const DataType& GetAt(unsigned int offset) const
	{
		assert(IsValid());
		assert(offset < m_Size);
		return m_Ptr[offset];
	}

	MkMemoryBlockDescriptor() { m_Ptr = 0; m_Size = 0; }
	MkMemoryBlockDescriptor(DataType* ptr, unsigned int size) { SetUp(ptr, size); }
	MkMemoryBlockDescriptor(const DataType* ptr, unsigned int size) { SetUp(ptr, size); }
	MkMemoryBlockDescriptor(DataType* ptr, unsigned int offset, unsigned int size) { SetUp(ptr, offset, size); }
	MkMemoryBlockDescriptor(const DataType* ptr, unsigned int offset, unsigned int size) { SetUp(ptr, offset, size); }
	MkMemoryBlockDescriptor(DataType* ptr, const MkArraySection& section) { SetUp(ptr, section); }
	MkMemoryBlockDescriptor(const DataType* ptr, const MkArraySection& section) { SetUp(ptr, section); }
	MkMemoryBlockDescriptor(const DataType& instance) { SetUp(instance); }
	~MkMemoryBlockDescriptor() {}

private:

	DataType* m_Ptr;
	unsigned int m_Size;
};

//------------------------------------------------------------------------------------------------//

#endif