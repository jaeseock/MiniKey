#ifndef __MINIKEY_CORE_MKARRAY_H__
#define __MINIKEY_CORE_MKARRAY_H__


//------------------------------------------------------------------------------------------------//
// array
//
// 구간묘사를 위해 MkArraySection를 사용
// 허용 범위는 0 ~ 2147483647(0x7fffffff, MKDEF_MAX_ARRAY_SIZE)
//
// Reserve는 주어진 용량만큼 할당하지만, PushBack, Insert로 값을 추가하며 할당할 때는 별도 규칙 적용
// - 최초 값 할당시는 하나의 값만 할당
// - 이후 할당 메모리가 부족할 경우 블록 단위로 확장(기본은 MKDEF_DEFAULT_BLOCK_EXPANSION_SIZE_IN_ARRAY만큼)
// - 만약 특정 타입만 확장 크기를 변경하고 싶을 경우 MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY()를 사용해 설정
//
// 복사시 고정크기 자료형으로 선언되어 있을 경우 한번에 모든 멤버를 복사(MKDEF_DECLARE_FIXED_SIZE_TYPE)해
// 개별 할당 시간을 절약
//
// (NOTE) thread safe 하지 않음
//
// (NOTE) 생성, 삽입, 삭제시 최적화를 위해 내부 메모리 위치가 변경될 수 있으므로 한 번 생성된 배열의 주소가
// 고정적이라고 생각하지 말 것. 즉 실시간으로 생성, 삽입, 삭제가 일어날 수 있는 배열은 필요시마다 주소를
// 새로 가져와 사용해야 함.
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
	// 메모리 관리
	//------------------------------------------------------------------------------------------------//

	// 크기 예약
	// 이전 크기보다 size가 클 경우만 실행
	// 이전 데이터가 존재할 경우 그대로 보존(주소는 변경됨)
	// (NOTE) 새로이 확보된 장소에 대한 초기화는 하지 않음(쓰레기값 상태)
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

	// 추가확장공간(확보만 되고 할당되지 않은 메모리)가 있다면 삭제
	// Clear()를 제외하고 이미 확보된 메모리를 지울 수 있는 유일한 함수
	inline void OptimizeMemory(void)
	{
		if (m_AllocSize > m_ValueSize)
		{
			_Realloc(m_ValueSize);
		}
	}

	// 해제. 모든 메모리 삭제
	inline void Clear(void)
	{
		if (m_Element != 0)
		{
			delete [] m_Element;
		}
		_Initialize();
	}

	// 확보된 값 크기 반환
	inline unsigned int GetAllocSize(void) const { return m_AllocSize; }

	//------------------------------------------------------------------------------------------------//
	// 정보 참조
	//------------------------------------------------------------------------------------------------//

	// 배열이 비었는지 여부
	inline bool Empty(void) const { return (m_ValueSize == 0); }

	// 할당된 값 개수 반환
	inline unsigned int GetSize(void) const { return m_ValueSize; }

	// 유효한 인덱스인지 판별
	inline bool IsValidIndex(unsigned int index) const { return (index < m_ValueSize); }

	// 포인터 반환
	inline DataType* GetPtr(void) { return m_Element; }
	inline const DataType* GetPtr(void) const { return m_Element; }

	// 현 배열에서 사용 가능 구간인지 여부 반환
	inline bool GetAvailableSection(const MkArraySection& section) const { return (IsValidIndex(section.GetPosition()) && (m_ValueSize >= section.GetEndPosition())); }

	// 배열 전체의 GetMemoryBlockDescriptor 반환
	inline MkMemoryBlockDescriptor<DataType> GetMemoryBlockDescriptor(void) const { return MkMemoryBlockDescriptor<DataType>(m_Element, m_ValueSize); }

	// 배열 일부의 GetMemoryBlockDescriptor 반환
	// 주어진 범위가 배열의 범위를 넘어갈 경우 section.position이후 전부를 의미
	// 범위가 겹치지 않으면 disable descriptor를 반환
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

	// 배열의 position 부터의 구간과 주어진 블록의 동일여부 반환
	inline bool Equals(unsigned int position, const MkMemoryBlockDescriptor<DataType>& desc) const
	{
		if ((!desc.IsValid()) || (!GetAvailableSection(MkArraySection(position, desc.GetSize()))))
			return false;

		return _EqualData(m_Element, position, desc, 0, desc.GetSize());
	}

	// 배열의 position 부터의 구간과 주어진 배열의 동일여부 반환
	inline bool Equals(unsigned int position, const MkArray<DataType>& values) const { return Equals(position, values.GetMemoryBlockDescriptor()); }

	// 배열의 section에서 해당 블록들이 존재하는 최초 위치를, 없으면 MKDEF_ARRAY_ERROR 반환
	inline unsigned int FindFirstInclusion(const MkArraySection& section, const MkArray< MkMemoryBlockDescriptor<DataType> >& descList) const
	{
		MkArray<unsigned int> positions;
		return (_FindBlockInclusion(section, descList, true, positions) == 1) ? positions[0] : MKDEF_ARRAY_ERROR;
	}

	// 배열의 section에서 해당 값이 존재하는 최초 위치를, 없으면 MKDEF_ARRAY_ERROR 반환
	inline unsigned int FindFirstInclusion(const MkArraySection& section, const DataType& value) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(MkMemoryBlockDescriptor<DataType>(value));
		return FindFirstInclusion(section, descList);
	}

	// 배열의 section에서 해당 배열이 존재하는 최초 위치를, 없으면 MKDEF_ARRAY_ERROR 반환
	inline unsigned int FindFirstInclusion(const MkArraySection& section, const MkArray<DataType>& values) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(values.GetMemoryBlockDescriptor());
		return FindFirstInclusion(section, descList);
	}

	// 배열의 section에서 해당 태그 중 하나가 존재하는 최초 위치를, 없으면 MKDEF_ARRAY_ERROR 반환
	inline unsigned int FindFirstTagInclusion(const MkArraySection& section, const MkArray<DataType>& tags) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		return _ConvertTagToMemoryBlockDescriptors(tags, descList) ? FindFirstInclusion(section, descList) : MKDEF_ARRAY_ERROR;
	}

	// 배열의 section에서 해당 블록들이 존재하는 모든 위치를 positions에 담고 검색된 개수 반환
	inline unsigned int FindAllInclusions
		(const MkArraySection& section, const MkArray< MkMemoryBlockDescriptor<DataType> >& descList, MkArray<unsigned int>& positions) const
	{
		return _FindBlockInclusion(section, descList, false, positions);
	}

	// 배열의 section에서 해당 값이 존재하는 모든 위치를 positions에 담고 검색된 개수 반환
	inline unsigned int FindAllInclusions(const MkArraySection& section, const DataType& value, MkArray<unsigned int>& positions) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(MkMemoryBlockDescriptor<DataType>(value));
		return FindAllInclusions(section, descList, positions);
	}

	// 배열의 section에서 해당 배열이 존재하는 모든 위치를 positions에 담고 검색된 개수 반환
	inline unsigned int FindAllInclusions(const MkArraySection& section, const MkArray<DataType>& values, MkArray<unsigned int>& positions) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(values.GetMemoryBlockDescriptor());
		return FindAllInclusions(section, descList, positions);
	}

	// 배열의 section에서 해당 태그 중 하나가 존재하는 모든 위치를 positions에 담고 검색된 개수 반환
	inline unsigned int FindAllTagInclusions(const MkArraySection& section, const MkArray<DataType>& tags, MkArray<unsigned int>& positions) const
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		return _ConvertTagToMemoryBlockDescriptors(tags, descList) ? FindAllInclusions(section, descList, positions) : 0;
	}

	// 배열의 section에서 해당 블록 포함여부
	inline bool Exist(const MkArraySection& section, const MkMemoryBlockDescriptor<DataType>& desc) const { return (FindFirstInclusion(section, desc) != MKDEF_ARRAY_ERROR); }

	// 해당 값 포함여부
	inline bool Exist(const MkArraySection& section, const DataType& value) const { return (FindFirstInclusion(section, value) != MKDEF_ARRAY_ERROR); }

	// 해당 배열 포함여부
	inline bool Exist(const MkArraySection& section, const MkArray<DataType>& values) const { return (FindFirstInclusion(section, values) != MKDEF_ARRAY_ERROR); }

	// 자신과 targets를 비교하여 자신만 소유한 값은 sourceOnly, 공동 소유값은 intersection, target만 소유한 값은 targetOnly에 넣음
	// (NOTE) 값 중복검사를 하지 않으므로 자신과 targets에 들어 있는 값들은 해당 배열에서는 유일해야 함
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
				if (enable[j] && (currSrc == targets[j])) // 일치. 탐색 리스트에서 제거하며 intersection에 등록
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
		for (unsigned int i=0; i<targetSize; ++i) // 일치되지 않은 나머지는 targetOnly에 등록
		{
			if (enable[i])
			{
				targetOnly.PushBack(targets[i]);
			}
		}
	}

	// DataType이 대소비교가 가능한 타입이고 배열이 오름차순으로 정렬되어 있다고 가정한 상태에서 value의 lower bound를 찾아 index 반환
	// value가 가장 작은 값(최초 element) 보다 작으면 0 반환
	// value가 가장 큰 값(마지막 element) 보다 크면 마지막 index 반환
	// 빈 배열이라면 MKDEF_ARRAY_ERROR 반환
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

	// offset 참조
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

	// 비교 연산자
	inline bool operator == (MkArray<DataType>& values) const { return Equals(0, values); }
	inline bool operator == (const MkArray<DataType>& values) const { return Equals(0, values); }
	inline bool operator != (MkArray<DataType>& values) const { return (!Equals(0, values)); }
	inline bool operator != (const MkArray<DataType>& values) const { return (!Equals(0, values)); }
	
	//------------------------------------------------------------------------------------------------//
	// 값 추가
	// 최초 값이 하나일 경우 정확히 한개의 공간만 할당, 그 외 공간이 없을 경우 블록단위 확장
	// ex> 블록 확장크기가 4일 경우 하나씩 값을 추가하면 value/alloc size는,
	//	1/1, 2/4, 3/4, 4/4, 5/8, 6/8, ...식으로 진행됨
	// (NOTE) 중간삽입의 경우 연속성 보장을 위한 메모리 이동 부하 발생
	//------------------------------------------------------------------------------------------------//

	// 할당 공간을 확장하여 값 크기를 size로 맞춤
	// 확장된 빈 공간은 포인터일 경우 NULL로 초기화되고 그렇지 않을 경우 쓰레기값 상태 유지
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

	// 할당 공간을 확장하여 값 크기를 size로 맞추고 늘어난 공간을 initialValue로 초기화
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

	// 빈 값을 하나 추가하고 참조 반환
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

	// 마지막에 블록 추가
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

	// 마지막에 값 추가
	inline void PushBack(const DataType& value) { PushBack(MkMemoryBlockDescriptor<DataType>(value)); }

	// 마지막에 배열 추가
	inline void PushBack(const MkArray<DataType>& values) { PushBack(values.GetMemoryBlockDescriptor()); }

	// 배열의 position 위치에 블록 삽입
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

	// 배열의 position 위치에 값 삽입
	inline void Insert(unsigned int position, const DataType& value) { Insert(position, MkMemoryBlockDescriptor<DataType>(value)); }

	// 배열의 position 위치에 배열 삽입
	inline void Insert(unsigned int position, const MkArray<DataType>& values) { Insert(position, values.GetMemoryBlockDescriptor()); }

	// operator +
	// (NOTE) operator += 에 비해 두번의 operator = 추가 발생
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
	// 값 삭제
	// 값 유효성만 없어질 뿐 할당된 실제 메모리가 줄어들지는 않음
	// (NOTE) 중간삭제의 경우 연속성 보장을 위한 메모리 이동 부하 발생
	//------------------------------------------------------------------------------------------------//

	// 모든 값 삭제. 할당 메모리는 유지
	inline void Flush(void) { m_ValueSize = 0; }

	// 마지막 값부터 size 개수만큼 삭제
	// size가 범위를 넘어갈 경우 전체 삭제
	inline void PopBack(unsigned int size = 1)
	{
		if ((m_ValueSize > 0) && (size > 0))
		{
			m_ValueSize = (size < m_ValueSize) ? (m_ValueSize - size) : 0;
		}
	}

	// 배열의 section을 삭제
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

	// 배열에서 주어진 위치의 멤버들을 삭제
	inline void Erase(const MkArray<unsigned int>& positions)
	{
		if (!positions.Empty())
		{
			MkArray<unsigned int> posCopy = positions;
			_RearrangeForSizeChanging(posCopy, 1, 0);
		}
	}

	// 배열의 section에서 최초로 해당되는 블록 삭제, 성공여부(존재 여부) 반환
	inline bool EraseFirstInclusion(const MkArraySection& section, const MkMemoryBlockDescriptor<DataType>& desc)
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(desc);
		return _EraseArray(FindFirstInclusion(section, descList), desc.GetSize());
	}

	// 배열의 section에서 최초로 해당되는 값 삭제, 성공여부(존재 여부) 반환
	inline bool EraseFirstInclusion(const MkArraySection& section, const DataType& value) { return _EraseArray(FindFirstInclusion(section, value), 1); }

	// 배열의 section에서 최초로 해당되는 배열 삭제, 성공여부(존재 여부) 반환
	inline bool EraseFirstInclusion(const MkArraySection& section, const MkArray<DataType>& values) { return _EraseArray(FindFirstInclusion(section, values), values.GetSize()); }

	// 배열의 section에서 최초로 해당되는 태그 삭제, 성공여부(존재 여부) 반환
	inline bool EraseFirstTagInclusion(const MkArraySection& section, const MkArray<DataType>& tags) { return _EraseArray(FindFirstTagInclusion(section, tags), 1); }

	// 배열의 section에서 해당되는 모든 블록 삭제, 삭제 개수 반환
	inline unsigned int EraseAllInclusions(const MkArraySection& section, const MkMemoryBlockDescriptor<DataType>& desc)
	{
		MkArray< MkMemoryBlockDescriptor<DataType> > descList;
		descList.PushBack(desc);
		MkArray<unsigned int> positions;
		return (FindAllInclusions(section, descList, positions) > 0) ? _RearrangeForSizeChanging(positions, desc.GetSize(), 0) : 0;
	}

	// 배열의 section에서 해당되는 모든 값 삭제, 삭제 개수 반환
	inline unsigned int EraseAllInclusions(const MkArraySection& section, const DataType& value) { return EraseAllInclusions(section, MkMemoryBlockDescriptor<DataType>(value)); }

	// 배열의 section에서 해당되는 모든 배열 삭제, 삭제 개수 반환
	inline unsigned int EraseAllInclusions(const MkArraySection& section, const MkArray<DataType>& values) { return EraseAllInclusions(section, values.GetMemoryBlockDescriptor()); }

	// 배열의 section에서 해당되는 모든 태그 삭제, 삭제 개수 반환
	inline unsigned int EraseAllTagInclusions(const MkArraySection& section, const MkArray<DataType>& tags)
	{
		MkArray<unsigned int> positions;
		return (FindAllTagInclusions(section, tags, positions) > 0) ? _RearrangeForSizeChanging(positions, 1, 0) : 0;
	}

	//------------------------------------------------------------------------------------------------//
	// 복사
	//------------------------------------------------------------------------------------------------//

	// 배열 복사(깊은 복사). 추가확장공간 포함
	inline MkArray<DataType>& operator = (const MkArray<DataType>& values) { return _OperatorAssign(values); }

	// 배열의 section을 buffer에 복사
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
	// 배열 변조
	//------------------------------------------------------------------------------------------------//

	// position 위치에 block의 내용을 덮어씀
	// 성공여부 반환
	bool Overwrite(unsigned int position, const MkMemoryBlockDescriptor<DataType>& desc)
	{
		bool ok = (desc.IsValid() && GetAvailableSection(MkArraySection(position, desc.GetSize())));
		if (ok)
		{
			_CopyData(desc, 0, m_Element, position, desc.GetSize());
		}
		return ok;
	}

	// 순서를 뒤집어 재설정
	// ex> A, B, C -> C, B, A
	inline void ReverseOrder(void)
	{
		if (m_ValueSize < 2)
			return;

		// 전반부를 뒤집어 따로 저장
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

		// 후반부를 뒤집어 전반부로 복사
		from = m_ValueSize - 1;
		to = 0;
		while (to < halfSize)
		{
			m_Element[to] = m_Element[from];
			--from;
			++to;
		}

		// 따로 저장해 놓은 뒤집힌 전반부를 후반부에 복사
		_CopyData(buffer, 0, m_Element, m_ValueSize - halfSize, halfSize);
		delete [] buffer;
	}

	// 내림차순 정렬(ex> 10, 9, 8, ...)
	// max heap sort : O(nlog2n)
	// (NOTE) type은 operator <, >= 가 정의되어 있어야 함
	inline void SortInDescendingOrder(void)
	{
		MkSingleArraySorter<DataType> sorter;
		sorter.SortInDescendingOrder(m_Element, m_ValueSize);
	}

	// 오름차순 정렬(ex> 1, 2, 3, ...)
	// min heap sort : O(nlog2n)
	// (NOTE) type은 operator >, <= 가 정의되어 있어야 함
	inline void SortInAscendingOrder(void)
	{
		MkSingleArraySorter<DataType> sorter;
		sorter.SortInAscendingOrder(m_Element, m_ValueSize);
	}

	// 배열의 section에서 해당되는 모든 블록을 변경(from -> to)
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

	// 배열의 section에서 해당되는 모든 값을 변경(from -> to)
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

	// 배열의 section에서 해당되는 모든 배열을 변경(from -> to)
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
				// 이전 자료 한번에 복사
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
				// 루핑하며 개별 할당
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
		
		// 고정크기자료형
		if (IsFixedSizeType<DataType>::Get() || IsPointer<DataType>::result)
		{
			return (memcmp(&srcPtr[srcPos], &destPtr[destPos], size * sizeof(DataType)) == 0);
		}
		
		// 가변크기자료형
		unsigned int i = 0;
		while (i < size)
		{
			if (srcPtr[srcPos + i] != destPtr[destPos + i]) // 멤버들의 개별 비교로 판단
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
		unsigned int minBlockSize = 0xffffffff; // 가장 작은 메모리 블록의 크기

		// 대상 block desc 판별
		MkArray< MkMemoryBlockDescriptor<DataType> > newDescList(descList.GetSize()); // 탐색 대상이 될 desc list
		MkArray<unsigned int> endPos(descList.GetSize()); // 각 desc들의 종료지점

		MK_INDEXING_LOOP(descList, i)
		{
			const MkMemoryBlockDescriptor<DataType>& desc = descList[i];
			if (desc.IsValid())
			{
				unsigned int blockSize = desc.GetSize();
				if (sectionSize >= blockSize) // 사용 가능하고 크기가 맞으면
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

		// 배열 순회
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
			if (sizeToErase < sizeToAdd) // 확대
			{
				if (newSize > m_AllocSize) // 메모리 확장 필요
				{
					unsigned int newAllocSize = _GetBlockSizeToAlloc(newSize);
					if (MKDEF_CHECK_ARRAY_SIZE(newAllocSize))
					{
						DataType* buffer = new DataType[newAllocSize];
						_RearrangeByIncreasementRule(buffer, positions, sizeToErase, sizeToAdd);
						if (positions[0] > 0) // 최초 검색지점이 배열 시작이 아닐 경우 시작부터 최초 검색지점까지의 값을 복사
						{
							_CopyData(m_Element, 0, buffer, 0, positions[0]);
						}
						_ReplaceBuffer(buffer, newAllocSize);
					}
					else
						return 0;
				}
				else // 확대 재정렬
				{
					_RearrangeByIncreasementRule(m_Element, positions, sizeToErase, sizeToAdd);
				}
			}
			else // 축소 재정렬
			{
				_RearrangeByDecreasementRule(m_Element, positions, sizeToErase, sizeToAdd);
			}

			m_ValueSize = newSize;
		}
		return memberCount;
	}

public:

	// 특수화용 멤버함수
	inline unsigned int __GetBlockExpansionSizeInArray(void) const { return MKDEF_DEFAULT_BLOCK_EXPANSION_SIZE_IN_ARRAY; }

	//------------------------------------------------------------------------------------------------//

protected:

	DataType* m_Element;
	unsigned int m_AllocSize;
	unsigned int m_ValueSize;
};


//------------------------------------------------------------------------------------------------//
// 템플릿 특수화 매크로
//------------------------------------------------------------------------------------------------//

// 블록 확장시 용량 결정
// (NOTE) s는 1 이상일 것. 1이면 요구 용량 그대로 할당
#define MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY(t, s) \
	template <> unsigned int MkArray<t>::__GetBlockExpansionSizeInArray(void) const { return s; }

// wchar_t의 경우(문자열) 32개씩 확장
MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY(wchar_t, 32)

// char, unsigned char의 경우(byte block) 정확히 필요 크기만큼 확장
MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY(char, 1)
MKDEF_DECALRE_BLOCK_EXPANSION_SIZE_IN_ARRAY(unsigned char, 1)

//------------------------------------------------------------------------------------------------//
// MkByteArray 정의
//------------------------------------------------------------------------------------------------//

typedef MkArray<unsigned char> MkByteArray;
typedef MkMemoryBlockDescriptor<unsigned char> MkByteArrayDescriptor;

//------------------------------------------------------------------------------------------------//

#endif
