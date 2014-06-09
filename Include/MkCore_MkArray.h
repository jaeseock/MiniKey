#ifndef __MINIKEY_CORE_MKARRAY_H__
#define __MINIKEY_CORE_MKARRAY_H__


//------------------------------------------------------------------------------------------------//
// array
//
// �������縦 ���� MkArraySection�� ���
// ��� ������ 0 ~ 2147483647(0x7fffffff, MKDEF_MAX_ARRAY_SIZE)
//
// Reserve�� �־��� �뷮��ŭ �Ҵ�������, PushBack, Insert�� ���� �߰��ϸ� �Ҵ��� ���� ���� ��Ģ ����
// - ���� �� �Ҵ�ô� �ϳ��� ���� �Ҵ�
// - ���� �Ҵ� �޸𸮰� ������ ��� ��� ������ Ȯ��(�⺻�� MKDEF_DEFAULT_BLOCK_EXPANSION_SIZE_IN_ARRAY��ŭ)
// - ���� Ư�� Ÿ�Ը� Ȯ�� ũ�⸦ �����ϰ� ���� ��� MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY()�� ����� ����
//
// ����� ����ũ�� �ڷ������� ����Ǿ� ���� ��� �ѹ��� ��� ����� ����(MKDEF_DECLARE_FIXED_SIZE_TYPE)��
// ���� �Ҵ� �ð��� ����
//
// (NOTE) thread safe ���� ����
//
// (NOTE) ����, ����, ������ ����ȭ�� ���� ���� �޸� ��ġ�� ����� �� �����Ƿ� �� �� ������ �迭�� �ּҰ�
// �������̶�� �������� �� ��. �� �ǽð����� ����, ����, ������ �Ͼ �� �ִ� �迭�� �ʿ�ø��� �ּҸ�
// ���� ������ ����ؾ� ��.
//------------------------------------------------------------------------------------------------//

#include <string> // memmove_s(only), memcpy_s, memcmp, memset
#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkContainerDefinition.h"
#include "MkCore_MkGlobalFunctor.h"
#include "MkCore_MkMemoryBlockDescriptor.h"


template <class DataType>
class MkArray
{
public:

	//------------------------------------------------------------------------------------------------//
	// �޸� ����
	//------------------------------------------------------------------------------------------------//

	// ũ�� ����
	// ���� ũ�⺸�� size�� Ŭ ��츸 ����
	// ���� �����Ͱ� ������ ��� �״�� ����(�ּҴ� �����)
	// (NOTE) ������ Ȯ���� ��ҿ� ���� �ʱ�ȭ�� ���� ����(�����Ⱚ ����)
	inline void Reserve(unsigned int size)
	{
		if (MKDEF_CHECK_ARRAY_SIZE(size))
		{
			if (m_AllocSize == 0)
			{
				m_Element = new DataType[size];
				m_AllocSize = size;
			}
			else if (size > m_AllocSize)
			{
				_Realloc(size);
			}
		}
	}

	// �߰�Ȯ�����(Ȯ���� �ǰ� �Ҵ���� ���� �޸�)�� �ִٸ� ����
	// Clear()�� �����ϰ� �̹� Ȯ���� �޸𸮸� ���� �� �ִ� ������ �Լ�
	inline void OptimizeMemory(void)
	{
		if (m_AllocSize > m_ValueSize)
		{
			_Realloc(m_ValueSize);
		}
	}

	// ����. ��� �޸� ����
	inline void Clear(void)
	{
		if (m_Element != 0)
		{
			delete [] m_Element;
		}
		_Initialize();
	}

	// Ȯ���� �� ũ�� ��ȯ
	inline unsigned int GetAllocSize(void) const { return m_AllocSize; }

	//------------------------------------------------------------------------------------------------//
	// ���� ����
	//------------------------------------------------------------------------------------------------//

	// �迭�� ������� ����
	inline bool Empty(void) const { return (m_ValueSize == 0); }

	// �Ҵ�� �� ���� ��ȯ
	inline unsigned int GetSize(void) const { return m_ValueSize; }

	// ��ȿ�� �ε������� �Ǻ�
	inline bool IsValidIndex(unsigned int index) const { return (index < m_ValueSize); }

	// ������ ��ȯ
	inline DataType* GetPtr(void) { return m_Element; }
	inline const DataType* GetPtr(void) const { return m_Element; }

	// �� �迭���� ��� ���� �������� ���� ��ȯ
	inline bool GetAvailableSection(const MkArraySection& section) const { return (IsValidIndex(section.GetPosition()) && (m_ValueSize >= section.GetEndPosition())); }

	// �迭 ��ü�� GetMemoryBlockDescriptor ��ȯ
	inline MkMemoryBlockDescriptor<DataType> GetMemoryBlockDescriptor(void) const { return MkMemoryBlockDescriptor<DataType>(m_Element, m_ValueSize); }

	// �迭 �Ϻ��� GetMemoryBlockDescriptor ��ȯ
	// �־��� ������ �迭�� ������ �Ѿ ��� section.position���� ���θ� �ǹ�
	// ������ ��ġ�� ������ disable descriptor�� ��ȯ
	inline MkMemoryBlockDescriptor<DataType> GetMemoryBlockDescriptor(const MkArraySection& section) const
	{
		MkMemoryBlockDescriptor<DataType> desc;
		MkArraySection currSection = section.GetArraySection(m_ValueSize);
		if (currSection.GetSize() > 0)
		{
			desc.SetUp(m_Element, currSection);
		}
		return desc;
	}

	// �迭�� position ������ ������ �־��� ����� ���Ͽ��� ��ȯ
	inline bool Equals(unsigned int position, const MkMemoryBlockDescriptor<DataType>& desc) const
	{
		if ((!desc.IsValid()) || (!GetAvailableSection(MkArraySection(position, desc.GetSize()))))
			return false;

		return _EqualData(m_Element, position, desc, 0, desc.GetSize());
	}

	// �迭�� position ������ ������ �־��� �迭�� ���Ͽ��� ��ȯ
	inline bool Equals(unsigned int position, const MkArray<DataType>& values) const { return Equals(position, values.GetMemoryBlockDescriptor()); }

	// �迭�� section���� �ش� ��ϵ��� �����ϴ� ���� ��ġ��, ������ MKDEF_ARRAY_ERROR ��ȯ
	inline unsigned int FindFirstInclusion(const MkArraySection& section, const MkArray< MkMemoryBlockDescriptor<DataType> >& descList) const
	{
		MkArray<unsigned int> positions;
		return (_FindBlockInclusion(section, descList, true, positions) == 1) ? positions[0] : MKDEF_ARRAY_ERROR;
	}

	// �迭�� section���� �ش� ���� �����ϴ� ���� ��ġ��, ������ MKDEF_ARRAY_ERROR ��ȯ
	inline unsigned int FindFirstInclusion(const MkArraySection& section, const DataType& value) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(MkMemoryBlockDescriptor<DataType>(value));
		return FindFirstInclusion(section, descList);
	}

	// �迭�� section���� �ش� �迭�� �����ϴ� ���� ��ġ��, ������ MKDEF_ARRAY_ERROR ��ȯ
	inline unsigned int FindFirstInclusion(const MkArraySection& section, const MkArray<DataType>& values) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(values.GetMemoryBlockDescriptor());
		return FindFirstInclusion(section, descList);
	}

	// �迭�� section���� �ش� �±� �� �ϳ��� �����ϴ� ���� ��ġ��, ������ MKDEF_ARRAY_ERROR ��ȯ
	inline unsigned int FindFirstTagInclusion(const MkArraySection& section, const MkArray<DataType>& tags) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		return _ConvertTagToMemoryBlockDescriptors(tags, descList) ? FindFirstInclusion(section, descList) : MKDEF_ARRAY_ERROR;
	}

	// �迭�� section���� �ش� ��ϵ��� �����ϴ� ��� ��ġ�� positions�� ��� �˻��� ���� ��ȯ
	inline unsigned int FindAllInclusions
		(const MkArraySection& section, const MkArray< MkMemoryBlockDescriptor<DataType> >& descList, MkArray<unsigned int>& positions) const
	{
		return _FindBlockInclusion(section, descList, false, positions);
	}

	// �迭�� section���� �ش� ���� �����ϴ� ��� ��ġ�� positions�� ��� �˻��� ���� ��ȯ
	inline unsigned int FindAllInclusions(const MkArraySection& section, const DataType& value, MkArray<unsigned int>& positions) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(MkMemoryBlockDescriptor<DataType>(value));
		return FindAllInclusions(section, descList, positions);
	}

	// �迭�� section���� �ش� �迭�� �����ϴ� ��� ��ġ�� positions�� ��� �˻��� ���� ��ȯ
	inline unsigned int FindAllInclusions(const MkArraySection& section, const MkArray<DataType>& values, MkArray<unsigned int>& positions) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(values.GetMemoryBlockDescriptor());
		return FindAllInclusions(section, descList, positions);
	}

	// �迭�� section���� �ش� �±� �� �ϳ��� �����ϴ� ��� ��ġ�� positions�� ��� �˻��� ���� ��ȯ
	inline unsigned int FindAllTagInclusions(const MkArraySection& section, const MkArray<DataType>& tags, MkArray<unsigned int>& positions) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		return _ConvertTagToMemoryBlockDescriptors(tags, descList) ? FindAllInclusions(section, descList, positions) : 0;
	}

	// �迭�� section���� �ش� ��� ���Կ���
	inline bool Exist(const MkArraySection& section, const MkMemoryBlockDescriptor<DataType>& desc) const { return (FindFirstInclusion(section, desc) != MKDEF_ARRAY_ERROR); }

	// �ش� �� ���Կ���
	inline bool Exist(const MkArraySection& section, const DataType& value) const { return (FindFirstInclusion(section, value) != MKDEF_ARRAY_ERROR); }

	// �ش� �迭 ���Կ���
	inline bool Exist(const MkArraySection& section, const MkArray<DataType>& values) const { return (FindFirstInclusion(section, values) != MKDEF_ARRAY_ERROR); }

	// �ڽŰ� targets�� ���Ͽ� �ڽŸ� ������ ���� sourceOnly, ���� �������� intersection, target�� ������ ���� targetOnly�� ����
	// (NOTE) �� �ߺ��˻縦 ���� �����Ƿ� �ڽŰ� targets�� ��� �ִ� ������ �ش� �迭������ �����ؾ� ��
	inline void IntersectionTest
		(const MkArray<DataType>& targets, MkArray<DataType>& sourceOnly, MkArray<DataType>& intersection, MkArray<DataType>& targetOnly) const
	{
		unsigned int targetSize = targets.GetSize();
		if ((m_ValueSize == 0) && (targetSize > 0))
		{
			targetOnly = targets;
			return;
		}
		else if ((m_ValueSize > 0) && (targetSize == 0))
		{
			sourceOnly = *this;
			return;
		}

		MkArray<bool> enable;
		enable.Fill(targetSize, true);

		sourceOnly.Reserve(m_ValueSize);
		intersection.Reserve(GetMin<unsigned int>(m_ValueSize, targetSize));
		
		for (unsigned int i=0; i<m_ValueSize; ++i)
		{
			bool notFound = true;
			const DataType& currSrc = m_Element[i];
			for (unsigned int j=0; j<targetSize; ++j)
			{
				if (enable[j] && (currSrc == targets[j])) // ��ġ. Ž�� ����Ʈ���� �����ϸ� intersection�� ���
				{
					intersection.PushBack(currSrc);
					enable[j] = false;
					notFound = false;
					break;
				}
			}
			if (notFound)
			{
				sourceOnly.PushBack(currSrc);
			}
		}

		targetOnly.Reserve(targetSize - intersection.GetSize());
		for (unsigned int i=0; i<targetSize; ++i) // ��ġ���� ���� �������� targetOnly�� ���
		{
			if (enable[i])
			{
				targetOnly.PushBack(targets[i]);
			}
		}
	}

	// DataType�� ��Һ񱳰� ������ Ÿ���̰� �迭�� ������������ ���ĵǾ� �ִٰ� ������ ���¿��� value�� lower bound�� ã�� index ��ȯ
	// value�� ���� ���� ��(���� element) ���� ������ 0 ��ȯ
	// value�� ���� ū ��(������ element) ���� ũ�� ������ index ��ȯ
	// �� �迭�̶�� MKDEF_ARRAY_ERROR ��ȯ
	// ex>
	//	MkArray<float> sample; // { -3.f, 1.f, 1.f, 4.f, 9.f }
	//	sample.FindLowerBound(-5.f) -> 0
	//	sample.FindLowerBound(1.f) -> 2
	//	sample.FindLowerBound(1.3f) -> 2
	//	sample.FindLowerBound(4.7f) -> 3
	//	sample.FindLowerBound(9.f) -> 4
	//	sample.FindLowerBound(11.f) -> 4
	inline unsigned int FindLowerBound(const DataType& value) const
	{
		unsigned int boundIndex = FindLowerBoundInArray<DataType>(m_Element, m_ValueSize, value);
		return (boundIndex == 0xffffffff) ? MKDEF_ARRAY_ERROR : boundIndex;
	}

	// offset ����
	inline DataType& operator [] (unsigned int index)
	{
		assert(IsValidIndex(index));
		return m_Element[index];
	}

	inline const DataType& operator [] (unsigned int index) const
	{
		assert(IsValidIndex(index));
		return m_Element[index];
	}

	// �� ������
	inline bool operator == (MkArray<DataType>& values) const { return Equals(0, values); }
	inline bool operator == (const MkArray<DataType>& values) const { return Equals(0, values); }
	inline bool operator != (MkArray<DataType>& values) const { return (!Equals(0, values)); }
	inline bool operator != (const MkArray<DataType>& values) const { return (!Equals(0, values)); }
	
	//------------------------------------------------------------------------------------------------//
	// �� �߰�
	// ���� ���� �ϳ��� ��� ��Ȯ�� �Ѱ��� ������ �Ҵ�, �� �� ������ ���� ��� ��ϴ��� Ȯ��
	// ex> ��� Ȯ��ũ�Ⱑ 4�� ��� �ϳ��� ���� �߰��ϸ� value/alloc size��,
	//	1/1, 2/4, 3/4, 4/4, 5/8, 6/8, ...������ �����
	// (NOTE) �߰������� ��� ���Ӽ� ������ ���� �޸� �̵� ���� �߻�
	//------------------------------------------------------------------------------------------------//

	// �Ҵ� ������ Ȯ���Ͽ� �� ũ�⸦ size�� ����
	// Ȯ��� �� ������ �������� ��� NULL�� �ʱ�ȭ�ǰ� �׷��� ���� ��� �����Ⱚ ���� ����
	inline void Fill(unsigned int size)
	{
		if (m_ValueSize < size)
		{
			unsigned int allocSize = _GetBlockSizeToAlloc(size);
			Reserve(allocSize);

			if (IsPointer<DataType>::result)
			{
				memset(&m_Element[m_ValueSize], 0, (size - m_ValueSize) * sizeof(DataType));
			}
			
			m_ValueSize = size;
		}
	}

	// �Ҵ� ������ Ȯ���Ͽ� �� ũ�⸦ size�� ���߰� �þ ������ initialValue�� �ʱ�ȭ
	inline void Fill(unsigned int size, const DataType& initialValue)
	{
		if (m_ValueSize < size)
		{
			unsigned int allocSize = _GetBlockSizeToAlloc(size);
			Reserve(allocSize);

			for (unsigned int i=m_ValueSize; i<size; ++i)
			{
				m_Element[i] = initialValue;
			}
			
			m_ValueSize = size;
		}
	}

	// �� ���� �ϳ� �߰��ϰ� ���� ��ȯ
	inline DataType& PushBack(void)
	{
		assert(m_ValueSize < MKDEF_MAX_ARRAY_SIZE);
		unsigned int lastIndex = m_ValueSize;
		if ((m_ValueSize < MKDEF_MAX_ARRAY_SIZE))
		{
			Fill(lastIndex + 1);
		}
		else
		{
			--lastIndex;
		}
		
		return m_Element[lastIndex];
	}

	// �������� ��� �߰�
	inline void PushBack(const MkMemoryBlockDescriptor<DataType>& desc)
	{
		if (desc.IsValid())
		{
			unsigned int blockSize = desc.GetSize();
			unsigned int newSize = m_ValueSize + blockSize;
			if (newSize > m_AllocSize)
			{
				Reserve(_GetBlockSizeToAlloc(newSize));
			}
			_CopyData(desc, 0, m_Element, m_ValueSize, blockSize);
			m_ValueSize = newSize;
		}
	}

	// �������� �� �߰�
	inline void PushBack(const DataType& value) { PushBack(MkMemoryBlockDescriptor<DataType>(value)); }

	// �������� �迭 �߰�
	inline void PushBack(const MkArray<DataType>& values) { PushBack(values.GetMemoryBlockDescriptor()); }

	// �迭�� position ��ġ�� ��� ����
	inline void Insert(unsigned int position, const MkMemoryBlockDescriptor<DataType>& desc)
	{
		if (desc.IsValid())
		{
			if (((m_ValueSize == 0) && (position == 0)) || (position == m_ValueSize))
			{
				PushBack(desc);
			}
			else if (position < m_ValueSize)
			{
				unsigned int blockSize = desc.GetSize();
				unsigned int newSize = m_ValueSize + blockSize;
				if (newSize > m_AllocSize)
				{
					unsigned int newAllocSize = _GetBlockSizeToAlloc(newSize);
					if (MKDEF_CHECK_ARRAY_SIZE(newAllocSize))
					{
						DataType* buffer = new DataType[newAllocSize];
						if (position > 0)
						{
							_CopyData(m_Element, 0, buffer, 0, position);
						}
						_CopyData(desc, 0, buffer, position, blockSize);
						_CopyData(m_Element, position, buffer, position + blockSize, m_ValueSize - position);
						m_ValueSize = newSize;
						_ReplaceBuffer(buffer, newAllocSize);
					}
				}
				else
				{
					_CopyData(m_Element, position, m_Element, position + blockSize, m_ValueSize - position);
					_CopyData(desc, 0, m_Element, position, blockSize);
					m_ValueSize = newSize;
				}
			}
		}
	}

	// �迭�� position ��ġ�� �� ����
	inline void Insert(unsigned int position, const DataType& value) { Insert(position, MkMemoryBlockDescriptor<DataType>(value)); }

	// �迭�� position ��ġ�� �迭 ����
	inline void Insert(unsigned int position, const MkArray<DataType>& values) { Insert(position, values.GetMemoryBlockDescriptor()); }

	// operator +
	// (NOTE) operator += �� ���� �ι��� operator = �߰� �߻�
	inline MkArray<DataType> operator + (DataType& value) const { return _OperatorAdd(value); }
	inline MkArray<DataType> operator + (const DataType& value) const { return _OperatorAdd(value); }
	inline MkArray<DataType> operator + (MkArray<DataType>& values) const { return _OperatorAdd(values); }
	inline MkArray<DataType> operator + (const MkArray<DataType>& values) const { return _OperatorAdd(values); }

	// operator +=
	inline MkArray<DataType>& operator += (DataType& value) { return _OperatorAddTo(value); }
	inline MkArray<DataType>& operator += (const DataType& value) { return _OperatorAddTo(value); }
	inline MkArray<DataType>& operator += (MkArray<DataType>& values) { return _OperatorAddTo(values); }
	inline MkArray<DataType>& operator += (const MkArray<DataType>& values) { return _OperatorAddTo(values); }
	
	//------------------------------------------------------------------------------------------------//
	// �� ����
	// �� ��ȿ���� ������ �� �Ҵ�� ���� �޸𸮰� �پ������ ����
	// (NOTE) �߰������� ��� ���Ӽ� ������ ���� �޸� �̵� ���� �߻�
	//------------------------------------------------------------------------------------------------//

	// ��� �� ����. �Ҵ� �޸𸮴� ����
	inline void Flush(void) { m_ValueSize = 0; }

	// ������ ������ size ������ŭ ����
	// size�� ������ �Ѿ ��� ��ü ����
	inline void PopBack(unsigned int size = 1)
	{
		if ((m_ValueSize > 0) && (size > 0))
		{
			m_ValueSize = (size < m_ValueSize) ? (m_ValueSize - size) : 0;
		}
	}

	// �迭�� section�� ����
	inline void Erase(const MkArraySection& section)
	{
		MkArraySection currSection = section.GetArraySection(m_ValueSize);
		if (currSection.GetSize() > 0)
		{
			unsigned int endPos = currSection.GetEndPosition();
			if (endPos == m_ValueSize)
			{
				m_ValueSize = section.GetPosition();
			}
			else
			{
				_CopyData(m_Element, endPos, m_Element, section.GetPosition(), m_ValueSize - endPos);
				m_ValueSize -= currSection.GetSize();
			}
		}
	}

	// �迭���� �־��� ��ġ�� ������� ����
	inline void Erase(const MkArray<unsigned int>& positions)
	{
		if (!positions.Empty())
		{
			MkArray<unsigned int> posCopy = positions;
			_RearrangeForSizeChanging(posCopy, 1, 0);
		}
	}

	// �迭�� section���� ���ʷ� �ش�Ǵ� ��� ����, ��������(���� ����) ��ȯ
	inline bool EraseFirstInclusion(const MkArraySection& section, const MkMemoryBlockDescriptor<DataType>& desc)
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(desc);
		return _EraseArray(FindFirstInclusion(section, descList), desc.GetSize());
	}

	// �迭�� section���� ���ʷ� �ش�Ǵ� �� ����, ��������(���� ����) ��ȯ
	inline bool EraseFirstInclusion(const MkArraySection& section, const DataType& value) { return _EraseArray(FindFirstInclusion(section, value), 1); }

	// �迭�� section���� ���ʷ� �ش�Ǵ� �迭 ����, ��������(���� ����) ��ȯ
	inline bool EraseFirstInclusion(const MkArraySection& section, const MkArray<DataType>& values) { return _EraseArray(FindFirstInclusion(section, values), values.GetSize()); }

	// �迭�� section���� ���ʷ� �ش�Ǵ� �±� ����, ��������(���� ����) ��ȯ
	inline bool EraseFirstTagInclusion(const MkArraySection& section, const MkArray<DataType>& tags) { return _EraseArray(FindFirstTagInclusion(section, tags), 1); }

	// �迭�� section���� �ش�Ǵ� ��� ��� ����, ���� ���� ��ȯ
	inline unsigned int EraseAllInclusions(const MkArraySection& section, const MkMemoryBlockDescriptor<DataType>& desc)
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(desc);
		MkArray<unsigned int> positions;
		return (FindAllInclusions(section, descList, positions) > 0) ? _RearrangeForSizeChanging(positions, desc.GetSize(), 0) : 0;
	}

	// �迭�� section���� �ش�Ǵ� ��� �� ����, ���� ���� ��ȯ
	inline unsigned int EraseAllInclusions(const MkArraySection& section, const DataType& value) { return EraseAllInclusions(section, MkMemoryBlockDescriptor<DataType>(value)); }

	// �迭�� section���� �ش�Ǵ� ��� �迭 ����, ���� ���� ��ȯ
	inline unsigned int EraseAllInclusions(const MkArraySection& section, const MkArray<DataType>& values) { return EraseAllInclusions(section, values.GetMemoryBlockDescriptor()); }

	// �迭�� section���� �ش�Ǵ� ��� �±� ����, ���� ���� ��ȯ
	inline unsigned int EraseAllTagInclusions(const MkArraySection& section, const MkArray<DataType>& tags)
	{
		MkArray<unsigned int> positions;
		return (FindAllTagInclusions(section, tags, positions) > 0) ? _RearrangeForSizeChanging(positions, 1, 0) : 0;
	}

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// �迭 ����(���� ����). �߰�Ȯ����� ����
	inline MkArray<DataType>& operator = (const MkArray<DataType>& values) { return _OperatorAssign(values); }

	// �迭�� section�� buffer�� ����
	inline bool GetSubArray(const MkArraySection& section, MkArray<DataType>& buffer) const
	{
		MkMemoryBlockDescriptor<DataType> desc = GetMemoryBlockDescriptor(section);
		bool ok = desc.IsValid();
		if (ok)
		{
			buffer.Clear();
			buffer.PushBack(desc);
		}
		return ok;
	}

	//------------------------------------------------------------------------------------------------//
	// �迭 ����
	//------------------------------------------------------------------------------------------------//

	// position ��ġ�� block�� ������ ���
	// �������� ��ȯ
	bool Overwrite(unsigned int position, const MkMemoryBlockDescriptor<DataType>& desc)
	{
		bool ok = (desc.IsValid() && GetAvailableSection(MkArraySection(position, desc.GetSize())));
		if (ok)
		{
			_CopyData(desc, 0, m_Element, position, desc.GetSize());
		}
		return ok;
	}

	// ������ ������ �缳��
	// ex> A, B, C -> C, B, A
	inline void ReverseOrder(void)
	{
		if (m_ValueSize < 2)
			return;

		// ���ݺθ� ������ ���� ����
		unsigned int halfSize = m_ValueSize / 2;
		DataType* buffer = new DataType[halfSize];
		unsigned int from = 0;
		unsigned int to = halfSize - 1;
		while (from < halfSize)
		{
			buffer[to] = m_Element[from];
			++from;
			--to;
		}

		// �Ĺݺθ� ������ ���ݺη� ����
		from = m_ValueSize - 1;
		to = 0;
		while (to < halfSize)
		{
			m_Element[to] = m_Element[from];
			--from;
			++to;
		}

		// ���� ������ ���� ������ ���ݺθ� �Ĺݺο� ����
		_CopyData(buffer, 0, m_Element, m_ValueSize - halfSize, halfSize);
		delete [] buffer;
	}

	// �������� ����(ex> 10, 9, 8, ...)
	// max heap sort : O(nlog2n)
	// (NOTE) type�� operator <, >= �� ���ǵǾ� �־�� ��
	inline void SortInDescendingOrder(void)
	{
		MkSingleArraySorter<DataType> sorter;
		sorter.SortInDescendingOrder(m_Element, m_ValueSize);
	}

	// �������� ����(ex> 1, 2, 3, ...)
	// min heap sort : O(nlog2n)
	// (NOTE) type�� operator >, <= �� ���ǵǾ� �־�� ��
	inline void SortInAscendingOrder(void)
	{
		MkSingleArraySorter<DataType> sorter;
		sorter.SortInAscendingOrder(m_Element, m_ValueSize);
	}

	// �迭�� section���� �ش�Ǵ� ��� ����� ����(from -> to)
	inline void ReplaceAll(const MkArraySection& section, const MkMemoryBlockDescriptor<DataType>& from, const MkMemoryBlockDescriptor<DataType>& to)
	{
		if (from.IsValid())
		{
			if (!to.IsValid())
			{
				EraseAllInclusions(section, from);
			}
			else
			{
				unsigned int fSize = from.GetSize();
				unsigned int tSize = to.GetSize();
				if ((fSize == 1) && (tSize == 1))
				{
					ReplaceAll(section, from.GetAt(0), to.GetAt(0));
				}
				else
				{
					MkArray< MkMemoryBlockDescriptor<DataType> > descList;
					descList.PushBack(from);
					MkArray<unsigned int> positions;
					unsigned int matchCount = FindAllInclusions(section, descList, positions);
					if (matchCount > 0)
					{
						if (fSize != tSize)
						{
							_RearrangeForSizeChanging(positions, fSize, tSize);
						}
						for (unsigned int i=0; i<matchCount; ++i)
						{
							_CopyData(to, 0, m_Element, positions[i], tSize);
						}
					}
				}
			}
		}
	}

	// �迭�� section���� �ش�Ǵ� ��� ���� ����(from -> to)
	inline void ReplaceAll(const MkArraySection& section, const DataType& from, const DataType& to)
	{
		MkArraySection currSection = section.GetArraySection(m_ValueSize);
		if (currSection.GetSize() > 0)
		{
			for (unsigned int counter = currSection.GetSize(), i=currSection.GetPosition(); i<counter; ++i)
			{
				if (m_Element[i] == from)
				{
					m_Element[i] = to;
				}
			}
		}
	}

	// �迭�� section���� �ش�Ǵ� ��� �迭�� ����(from -> to)
	inline void ReplaceAll(const MkArraySection& section, const MkArray<DataType>& from, const MkArray<DataType>& to) { ReplaceAll(section, from.GetMemoryBlockDescriptor(), to.GetMemoryBlockDescriptor()); }

	//------------------------------------------------------------------------------------------------//

	MkArray()
	{
		_Initialize();
	}

	MkArray(unsigned int size)
	{
		_Initialize();
		Reserve(size);
	}

	MkArray(const DataType* arrayPtr, unsigned int size)
	{
		_Initialize();
		PushBack(MkMemoryBlockDescriptor<DataType>(arrayPtr, size));
	}

	MkArray(const DataType* arrayPtr, unsigned int position, unsigned int size)
	{
		_Initialize();
		PushBack(MkMemoryBlockDescriptor<DataType>(arrayPtr, position, size));
	}

	MkArray(const MkMemoryBlockDescriptor<DataType>& desc)
	{
		_Initialize();
		PushBack(desc);
	}

	MkArray(const MkArray<DataType>& values)
	{
		_Initialize();
		*this = values;
	}

	virtual ~MkArray() { Clear(); }

	//------------------------------------------------------------------------------------------------//

protected:

	inline void _Initialize(void)
	{
		m_Element = 0;
		m_AllocSize = 0;
		m_ValueSize = 0;
	}

	inline unsigned int _GetBlockSizeToAlloc(unsigned int inputSize)
	{
		if (inputSize <= 1)
			return inputSize;

		unsigned int expansionSize = __GetBlockExpansionSizeInArray();
		return (expansionSize <= 1) ? inputSize : (((inputSize - 1) / expansionSize + 1) * expansionSize);
	}

	inline void _CopyData(const DataType* srcPtr, unsigned int srcPos, DataType* destPtr, unsigned int destPos, unsigned int size)
	{
		if (size == 1)
		{
			destPtr[destPos] = srcPtr[srcPos];
		}
		else if (size > 1)
		{
			if (IsFixedSizeType<DataType>::Get() || IsPointer<DataType>::result)
			{
				// ���� �ڷ� �ѹ��� ����
				const unsigned int copySize = size * sizeof(DataType);
				if (srcPtr == destPtr)
				{
					memmove_s(&destPtr[destPos], copySize, &srcPtr[srcPos], copySize);
				}
				else
				{
					memcpy_s(&destPtr[destPos], copySize, &srcPtr[srcPos], copySize);
				}
			}
			else
			{
				// �����ϸ� ���� �Ҵ�
				if (srcPos >= destPos)
				{
					for (unsigned int i=0; i<size; ++i)
					{
						destPtr[destPos + i] = srcPtr[srcPos + i];
					}
				}
				else
				{
					for (unsigned int i=size-1; i!=0xffffffff; --i)
					{
						destPtr[destPos + i] = srcPtr[srcPos + i];
					}
				}
			}
		}
	}

	inline bool _EqualData(const DataType* srcPtr, unsigned int srcPos, const DataType* destPtr, unsigned int destPos, unsigned int size) const
	{
		if (size == 1)
		{
			return (srcPtr[srcPos] == destPtr[destPos]);
		}
		
		// ����ũ���ڷ���
		if (IsFixedSizeType<DataType>::Get() || IsPointer<DataType>::result)
		{
			return (memcmp(&srcPtr[srcPos], &destPtr[destPos], size * sizeof(DataType)) == 0);
		}
		
		// ����ũ���ڷ���
		unsigned int i = 0;
		while (i < size)
		{
			if (srcPtr[srcPos + i] != destPtr[destPos + i]) // ������� ���� �񱳷� �Ǵ�
				return false;
			++i;
		}
		return true;
	}

	inline void _Realloc(unsigned int newAllocSize)
	{
		DataType* buffer = new DataType[newAllocSize];
		_CopyData(m_Element, 0, buffer, 0, m_ValueSize);
		_ReplaceBuffer(buffer, newAllocSize);
	}

	inline unsigned int _FindBlockInclusion
		(const MkArraySection& section, const MkArray< MkMemoryBlockDescriptor<DataType> >& descList,
		bool onlyFirstMaching, MkArray<unsigned int>& positions) const
	{
		if (descList.Empty())
			return 0;

		MkArraySection currSection = section.GetArraySection(m_ValueSize);
		unsigned int sectionSize = currSection.GetSize();
		unsigned int sectionEnd = currSection.GetEndPosition();
		unsigned int minBlockSize = 0xffffffff; // ���� ���� �޸� ����� ũ��

		// ��� block desc �Ǻ�
		MkArray< MkMemoryBlockDescriptor<DataType> > newDescList(descList.GetSize()); // Ž�� ����� �� desc list
		MkArray<unsigned int> endPos(descList.GetSize()); // �� desc���� ��������

		MK_INDEXING_LOOP(descList, i)
		{
			const MkMemoryBlockDescriptor<DataType>& desc = descList[i];
			if (desc.IsValid())
			{
				unsigned int blockSize = desc.GetSize();
				if (sectionSize >= blockSize) // ��� �����ϰ� ũ�Ⱑ ������
				{
					newDescList.PushBack(desc);
					endPos.PushBack(sectionEnd - blockSize + 1);

					if (blockSize < minBlockSize)
					{
						minBlockSize = blockSize;
					}
				}
			}
		}

		if (newDescList.Empty())
			return 0;

		if (!onlyFirstMaching)
		{
			positions.Reserve(positions.GetSize() + sectionSize - minBlockSize + 1);
		}

		// �迭 ��ȸ
		unsigned int lastPos = sectionEnd - minBlockSize + 1;
		for (unsigned int currPos = currSection.GetPosition(); currPos < lastPos; ++currPos)
		{
			MK_INDEXING_LOOP(newDescList, i)
			{
				const MkMemoryBlockDescriptor<DataType>& desc = newDescList[i];
				if (currPos < endPos[i])
				{
					if (_EqualData(m_Element, currPos, desc, 0, desc.GetSize()))
					{
						positions.PushBack(currPos);
						if (onlyFirstMaching)
							return 1;
					}
				}
			}
		}
		return positions.GetSize();
	}

	inline bool _ConvertTagToMemoryBlockDescriptors(const MkArray<DataType>& tags, MkArray< MkMemoryBlockDescriptor<DataType> >& descList) const
	{
		if (tags.Empty())
			return false;

		descList.Reserve(tags.GetSize());
		MK_INDEXING_LOOP(tags, i)
		{
			descList.PushBack(MkMemoryBlockDescriptor<DataType>(tags[i]));
		}
		return true;
	}

	inline void _ReplaceBuffer(DataType* newBuffer, unsigned int newAllocSize)
	{
		delete [] m_Element;
		m_Element = newBuffer;
		m_AllocSize = newAllocSize;
	}

	inline MkArray<DataType> _OperatorAdd(const DataType& value) const
	{
		MkArray<DataType> tmp = *this;
		tmp.PushBack(value);
		return tmp;
	}

	inline MkArray<DataType> _OperatorAdd(const MkArray<DataType>& values) const
	{
		MkArray<DataType> tmp = *this;
		if (!values.Empty())
		{
			tmp.PushBack(values);
		}
		return tmp;
	}

	inline MkArray<DataType>& _OperatorAddTo(const DataType& value)
	{
		PushBack(value);
		return *this;
	}

	inline MkArray<DataType>& _OperatorAddTo(const MkArray<DataType>& values)
	{
		if (!values.Empty())
		{
			PushBack(values);
		}
		return *this;
	}

	inline MkArray<DataType>& _OperatorAssign(const MkArray<DataType>& values)
	{
		Clear();
		m_AllocSize = values.GetAllocSize();
		if (m_AllocSize > 0)
		{
			m_ValueSize = values.GetSize();
			m_Element = new DataType[m_AllocSize];
			_CopyData(values.GetPtr(), 0, m_Element, 0, m_ValueSize);
		}
		return *this;
	}

	inline bool _EraseArray(unsigned int position, unsigned int size)
	{
		bool found = (position != MKDEF_ARRAY_ERROR);
		if (found)
		{
			Erase(MkArraySection(position, size));
		}
		return found;
	}

	inline void _SwapValue(unsigned int a, unsigned int b)
	{
		if (a != b)
		{
			DataType tmp = m_Element[a];
			m_Element[a] = m_Element[b];
			m_Element[b] = tmp;
		}
	}

	inline bool _BiggerConditionByMinMax(const DataType& left, const DataType& right, bool minHeap) const { return (minHeap) ? (left <= right) : (left >= right); }
	inline bool _SwapConditionByMinMax(const DataType& begin, const DataType& index, bool minHeap) const { return (minHeap) ? (begin > index) : (begin < index); }
	
	inline void _MinMaxHeap(unsigned int beginPosition, unsigned int limitCount, bool minHeap)
	{
		if ((limitCount - beginPosition) < 2)
			return;

		unsigned int nLeft, nRight, nBegin, nIndex, nOld;
		unsigned int nCurrent = nOld = beginPosition;

		while (nCurrent <= nOld)
		{
			nOld = nBegin = nIndex = nCurrent;

			while ((nBegin * 2 + 1) < limitCount)
			{
				nLeft = nBegin * 2 + 1;
				nRight = nBegin * 2 + 2;
				nBegin = nLeft;

				if ((nRight < limitCount) && _BiggerConditionByMinMax(m_Element[nLeft], m_Element[nRight], minHeap))
				{
					nBegin = nRight;
				}
				
				if (_SwapConditionByMinMax(m_Element[nBegin], m_Element[nIndex], minHeap))
				{
					_SwapValue(nIndex, nBegin);
					nIndex = nBegin;
				}
			}
			--nCurrent;
		}
	}

	inline void _HeapSort(bool minHeap)
	{
		if (m_ValueSize < 2)
			return;

		unsigned int limitCount = m_ValueSize;
		_MinMaxHeap(limitCount / 2 - 1, limitCount, minHeap);

		while (limitCount > 0)
		{
			--limitCount;
			_SwapValue(0, limitCount);
			_MinMaxHeap(0, limitCount, minHeap);
		}
	}

	inline void _RearrangeByDecreasementRule // sizeToErase > sizeToAdd
		(DataType* targetPtr, MkArray<unsigned int>& positions, unsigned int sizeToErase, unsigned int sizeToAdd)
	{
		assert(sizeToErase > sizeToAdd);
		MkArray<unsigned int> posCopy = positions;
		unsigned int memberCount = posCopy.GetSize();
		unsigned int sizeMovement = sizeToErase - sizeToAdd;
		unsigned int offset = 0;
		for (unsigned int i=0; i<memberCount; ++i)
		{
			unsigned int next = i + 1;
			unsigned int srcFrom = posCopy[i] + sizeToErase;
			unsigned int srcTo = posCopy.IsValidIndex(next) ? posCopy[next] : m_ValueSize;

			positions[i] -= offset;
			offset += sizeMovement;
			if (srcFrom < srcTo)
			{
				_CopyData(m_Element, srcFrom, targetPtr, srcFrom - offset, srcTo - srcFrom);
			}
		}
	}

	inline void _RearrangeByIncreasementRule // sizeToErase < sizeToAdd
		(DataType* targetPtr, MkArray<unsigned int>& positions, unsigned int sizeToErase, unsigned int sizeToAdd)
	{
		assert(sizeToErase < sizeToAdd);
		MkArray<unsigned int> posCopy = positions;
		unsigned int memberCount = posCopy.GetSize();
		unsigned int sizeMovement = sizeToAdd - sizeToErase;
		unsigned int offset = memberCount * sizeMovement;
		for (unsigned int i=memberCount-1; i!=0xffffffff; --i)
		{
			unsigned int next = i + 1;
			unsigned int srcFrom = posCopy[i] + sizeToErase;
			unsigned int srcTo = posCopy.IsValidIndex(next) ? posCopy[next] : m_ValueSize;

			if (srcFrom < srcTo)
			{
				_CopyData(m_Element, srcFrom, targetPtr, srcFrom + offset, srcTo - srcFrom);
			}
			offset -= sizeMovement;
			positions[i] += offset;
		}
	}

	inline unsigned int _RearrangeForSizeChanging(MkArray<unsigned int>& positions, unsigned int sizeToErase, unsigned int sizeToAdd)
	{
		unsigned int memberCount = positions.GetSize();
		if ((memberCount > 0) && (sizeToErase != sizeToAdd))
		{
			unsigned int newSize = m_ValueSize - memberCount * sizeToErase + memberCount * sizeToAdd;
			if (sizeToErase < sizeToAdd) // Ȯ��
			{
				if (newSize > m_AllocSize) // �޸� Ȯ�� �ʿ�
				{
					unsigned int newAllocSize = _GetBlockSizeToAlloc(newSize);
					if (MKDEF_CHECK_ARRAY_SIZE(newAllocSize))
					{
						DataType* buffer = new DataType[newAllocSize];
						_RearrangeByIncreasementRule(buffer, positions, sizeToErase, sizeToAdd);
						if (positions[0] > 0) // ���� �˻������� �迭 ������ �ƴ� ��� ���ۺ��� ���� �˻����������� ���� ����
						{
							_CopyData(m_Element, 0, buffer, 0, positions[0]);
						}
						_ReplaceBuffer(buffer, newAllocSize);
					}
					else
						return 0;
				}
				else // Ȯ�� ������
				{
					_RearrangeByIncreasementRule(m_Element, positions, sizeToErase, sizeToAdd);
				}
			}
			else // ��� ������
			{
				_RearrangeByDecreasementRule(m_Element, positions, sizeToErase, sizeToAdd);
			}

			m_ValueSize = newSize;
		}
		return memberCount;
	}

public:

	// Ư��ȭ�� ����Լ�
	inline unsigned int __GetBlockExpansionSizeInArray(void) const { return MKDEF_DEFAULT_BLOCK_EXPANSION_SIZE_IN_ARRAY; }

	//------------------------------------------------------------------------------------------------//

protected:

	DataType* m_Element;
	unsigned int m_AllocSize;
	unsigned int m_ValueSize;
};


//------------------------------------------------------------------------------------------------//
// ���ø� Ư��ȭ ��ũ��
//------------------------------------------------------------------------------------------------//

// ��� Ȯ��� �뷮 ����
// (NOTE) s�� 1 �̻��� ��. 1�̸� �䱸 �뷮 �״�� �Ҵ�
#define MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY(t, s) \
	template <> unsigned int MkArray<t>::__GetBlockExpansionSizeInArray(void) const { return s; }

// wchar_t�� ���(���ڿ�) 32���� Ȯ��
MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY(wchar_t, 32)

// char, unsigned char�� ���(byte block) ��Ȯ�� �ʿ� ũ�⸸ŭ Ȯ��
MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY(char, 1)
MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY(unsigned char, 1)

//------------------------------------------------------------------------------------------------//
// MkByteArray ����
//------------------------------------------------------------------------------------------------//

typedef MkArray<unsigned char> MkByteArray;
typedef MkMemoryBlockDescriptor<unsigned char> MkByteArrayDescriptor;

//------------------------------------------------------------------------------------------------//

#endif
