#ifndef __MINIKEY_CORE_MKGLOBALFUNCTOR_H__
#define __MINIKEY_CORE_MKGLOBALFUNCTOR_H__

//#include <assert.h>


//------------------------------------------------------------------------------------------------//
// 단일 배열 정렬기
// heap sort : O(nlog2n)
// (NOTE) KeyType은 operator <, >= 가 정의되어 있어야 함
// ex>
//	float keys[5] = { 4.f, 1.f, -3.f, 1.f, 9.f };
//	MkSingleArraySorter<float> sorter;
//	sorter.SortInDescendingOrder(keys, 5); -> 9.f, 4.f, 1.f, 1.f, -3.f
//	sorter.SortInAscendingOrder(keys, 5); -> -3.f, 1.f, 1.f, 4.f, 9.f
//------------------------------------------------------------------------------------------------//

template<class KeyType>
class MkSingleArraySorter
{
public:

	// 내림차순 정렬(ex> 10, 9, 8, ...)
	inline void SortInDescendingOrder(KeyType* keys, unsigned int arraySize) { _HeapSort(keys, arraySize, false); }

	// 오름차순 정렬(ex> 1, 2, 3, ...)
	inline void SortInAscendingOrder(KeyType* keys, unsigned int arraySize) { _HeapSort(keys, arraySize, true); }

protected:

	virtual void _SwapValue(unsigned int a, unsigned int b)
	{
		if (a != b)
		{
			KeyType tmp = m_Keys[a];
			m_Keys[a] = m_Keys[b];
			m_Keys[b] = tmp;
		}
	}

	inline bool _BiggerConditionByMinMax(const KeyType& left, const KeyType& right, bool minHeap) const { return (minHeap) ? (left <= right) : (left >= right); }
	inline bool _SwapConditionByMinMax(const KeyType& begin, const KeyType& index, bool minHeap) const { return (minHeap) ? (begin > index) : (begin < index); }

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

				if ((nRight < limitCount) && _BiggerConditionByMinMax(m_Keys[nLeft], m_Keys[nRight], minHeap))
				{
					nBegin = nRight;
				}
				
				if (_SwapConditionByMinMax(m_Keys[nBegin], m_Keys[nIndex], minHeap))
				{
					_SwapValue(nIndex, nBegin);
					nIndex = nBegin;
				}
			}
			--nCurrent;
		}
	}

	inline void _SortOperation(unsigned int limitCount, bool minHeap)
	{
		_MinMaxHeap(limitCount / 2 - 1, limitCount, minHeap);
		while (limitCount > 0)
		{
			--limitCount;
			_SwapValue(0, limitCount);
			_MinMaxHeap(0, limitCount, minHeap);
		}
	}

	inline void _HeapSort(KeyType* keys, unsigned int arraySize, bool minHeap)
	{
		if ((keys == 0) || (arraySize < 2))
			return;

		m_Keys = keys;

		_SortOperation(arraySize, minHeap);
	}

protected:

	KeyType* m_Keys;
};


//------------------------------------------------------------------------------------------------//
// key-field로 매칭되어 있는 pair 배열 정렬기. key 기준으로 정렬됨
// heap sort : O(nlog2n)
// (NOTE) KeyType은 operator <, >= 가 정의되어 있어야 함
// ex>
//	float keys[5] = { 4.f, 1.f, -3.f, 1.f, 9.f };
//	int fields[5] = { 3, 1, 0, 1, 4 };
//	MkPairArraySorter<float, int> sorter;
//	sorter.SortInDescendingOrder(keys, fields, 5); -> key:{ 9.f, 4.f, 1.f, 1.f, -3.f }, field:{ 4, 3, 1, 1, 0 }
//	sorter.SortInAscendingOrder(keys, fields, 5); -> key:{ -3.f, 1.f, 1.f, 4.f, 9.f }, field:{ 0, 1, 1, 3, 4 }
//------------------------------------------------------------------------------------------------//

template<class KeyType, class FieldType>
class MkPairArraySorter : public MkSingleArraySorter<KeyType>
{
public:

	// 내림차순 정렬(ex> 10, 9, 8, ...)
	inline void SortInDescendingOrder(KeyType* keys, FieldType* fields, unsigned int arraySize) { _HeapSort(keys, fields, arraySize, false); }

	// 오름차순 정렬(ex> 1, 2, 3, ...)
	inline void SortInAscendingOrder(KeyType* keys, FieldType* fields, unsigned int arraySize) { _HeapSort(keys, fields, arraySize, true); }

protected:

	virtual void _SwapValue(unsigned int a, unsigned int b)
	{
		if (a != b)
		{
			KeyType tmpKey = m_Keys[a];
			m_Keys[a] = m_Keys[b];
			m_Keys[b] = tmpKey;

			FieldType tmpField = m_Fields[a];
			m_Fields[a] = m_Fields[b];
			m_Fields[b] = tmpField;
		}
	}

	inline void _HeapSort(KeyType* keys, FieldType* fields, unsigned int arraySize, bool minHeap)
	{
		if ((keys == 0) || (fields == 0) || (arraySize < 2))
			return;

		m_Keys = keys;
		m_Fields = fields;

		_SortOperation(arraySize, minHeap);
	}

protected:

	FieldType* m_Fields;
};

//------------------------------------------------------------------------------------------------//

#endif

