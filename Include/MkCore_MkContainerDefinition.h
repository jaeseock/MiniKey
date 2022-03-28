#ifndef __MINIKEY_CORE_MKCONTAINERDEFINITION_H__
#define __MINIKEY_CORE_MKCONTAINERDEFINITION_H__


#include "MkCore_MkGlobalDefinition.h"


//------------------------------------------------------------------------------------------------//
// indexing looping
// - MkArray
// - MkPairArray
// - MkDeque
// - MkCircularDeque
//------------------------------------------------------------------------------------------------//

// 정주행
#define MK_INDEXING_LOOP(instance, index) \
	for (unsigned int _looping_counter = instance.GetSize(), index = 0; index < _looping_counter; ++index)

// 역주행
#define MK_INDEXING_RLOOP(instance, index) \
	for (unsigned int index = instance.GetSize()-1; index != 0xffffffff; --index)


//------------------------------------------------------------------------------------------------//
// STL wrapper looping
// - MkMap
// - MkMultiMap
// - MkHashMap
//------------------------------------------------------------------------------------------------//

template <class KeyType, class IteratorType>
class MkBaseStlLooper
{
public:
	inline const KeyType& GetCurrentKey(void) const { return m_Itr->first; }
	inline const IteratorType& GetCurrentIterator(void) { return m_Itr; }

	inline bool __CheckCondition(void) { return (m_Itr != m_End); }
	inline void __MoveToNext(void) { ++m_Itr; }

protected:
	IteratorType m_Itr;
	IteratorType m_End;
};

template <class KeyType, class FieldType, class ContainerType, class IteratorType>
class MkStlLooper : public MkBaseStlLooper<KeyType, IteratorType>
{
public:
	inline void SetUp(ContainerType& target)
	{
		this->m_Itr = target.__GetBeginItr();
		this->m_End = target.__GetEndItr();
	}

	inline FieldType& GetCurrentField(void) { return this->m_Itr->second; }
	MkStlLooper(ContainerType& target) { SetUp(target); }
};

template <class KeyType, class FieldType, class ContainerType, class IteratorType>
class MkConstStlLooper : public MkBaseStlLooper<KeyType, IteratorType>
{
public:
	inline void SetUp(const ContainerType& target)
	{
		this->m_Itr = target.__GetBeginItr();
		this->m_End = target.__GetEndItr();
	}

	inline const FieldType& GetCurrentField(void) const { return this->m_Itr->second; }
	MkConstStlLooper(const ContainerType& target) { SetUp(target); }
};

// 정주행
#define MK_STL_LOOP(looper) \
	for (; looper.__CheckCondition(); looper.__MoveToNext())


//------------------------------------------------------------------------------------------------//
// 고정 크기 자료형 여부 선언
//------------------------------------------------------------------------------------------------//

template <class DataType>
class IsFixedSizeType
{
public:
	inline static bool Get(void) { return false; }
};

// 고정 크기 선언
#define MKDEF_DECLARE_FIXED_SIZE_TYPE(t) \
	template <> inline static bool IsFixedSizeType<t>::Get(void) { return true; }

// primitive type의 경우 크기 고정
MKDEF_DECLARE_FIXED_SIZE_TYPE(bool)
MKDEF_DECLARE_FIXED_SIZE_TYPE(char)
MKDEF_DECLARE_FIXED_SIZE_TYPE(unsigned char)
MKDEF_DECLARE_FIXED_SIZE_TYPE(wchar_t)
MKDEF_DECLARE_FIXED_SIZE_TYPE(short)
MKDEF_DECLARE_FIXED_SIZE_TYPE(unsigned short)
MKDEF_DECLARE_FIXED_SIZE_TYPE(int)
MKDEF_DECLARE_FIXED_SIZE_TYPE(unsigned int)
MKDEF_DECLARE_FIXED_SIZE_TYPE(long)
MKDEF_DECLARE_FIXED_SIZE_TYPE(unsigned long)
MKDEF_DECLARE_FIXED_SIZE_TYPE(float)
MKDEF_DECLARE_FIXED_SIZE_TYPE(__int64)
MKDEF_DECLARE_FIXED_SIZE_TYPE(unsigned __int64)
MKDEF_DECLARE_FIXED_SIZE_TYPE(double)


//------------------------------------------------------------------------------------------------//
// array size checking
//------------------------------------------------------------------------------------------------//

#define MKDEF_CHECK_ARRAY_SIZE(size) ((size > 0) && (size <= MKDEF_MAX_ARRAY_SIZE))

//------------------------------------------------------------------------------------------------//

#endif

