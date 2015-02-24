#ifndef __MINIKEY_CORE_MKGLOBALFUNCTION_H__
#define __MINIKEY_CORE_MKGLOBALFUNCTION_H__

#include <assert.h>
#include "MkCore_MkGlobalDefinition.h"

//------------------------------------------------------------------------------------------------//
// macro
//------------------------------------------------------------------------------------------------//

#define MK_DELETE(a) if (a != NULL) { delete a; a = NULL; }
#define MK_DELETE_OBJECT(a) if (a != NULL) { DeleteObject(a); a = NULL; }
#define MK_DELETE_ARRAY(a) if (a != NULL) { delete [] a; a = NULL; }
#define MK_RELEASE(a) if (a != NULL) { a->Release(); a = NULL; }

#define MK_FLAG_EXIST(flag, filter) (((flag) & (filter)) == filter)

#define MK_PTR_TO_ID64(ptr) (reinterpret_cast<ID64>(ptr))

#define MK_VALUE_TO_STRING(v) (L#v)

//------------------------------------------------------------------------------------------------//
// �ڷ����� ������������ ������������ �Ǻ�
// ex>
//	const bool rlt0 = IsPointer<int>::result; // false
//	const bool rlt1 = IsPointer<int*>::result; // true
//------------------------------------------------------------------------------------------------//

template<class DataType>
class IsPointer
{
private:
	template<class T> struct PointerTraits { enum { result = false }; };
	template<class T> struct PointerTraits<T*> { enum { result = true }; };
public:
	enum { result = PointerTraits<DataType>::result };
};


//------------------------------------------------------------------------------------------------//
// min / max
// ex>
//	const int rlt0 = GetMax<int>(15, 20); // 20
//	const float rlt1 = GetMin<float>(-1.f, 1.f); // -1.f
//------------------------------------------------------------------------------------------------//

template<class DataType>
inline DataType GetMax(const DataType& a, const DataType& b) { return (a > b) ? a : b; }

template<class DataType>
inline DataType GetMin(const DataType& a, const DataType& b) { return (a < b) ? a : b; }

//------------------------------------------------------------------------------------------------//
// ���Կ��� �Ǵ�
// ex>
//	const int rlt0 = CheckInclusion<int>(15, 18, 20); // true
//	const int rlt1 = CheckInclusion<int>(15, 22, 20); // false
//------------------------------------------------------------------------------------------------//

template<class DataType>
inline bool CheckInclusion(const DataType& begin, const DataType& value, const DataType& end) { return (((begin) <= (value)) && ((value) <= (end))); }

//------------------------------------------------------------------------------------------------//
// clamp
// ex>
//	int rlt = Clamp<int>(10, 15, 20); // 10 -> 15
//	rlt = Clamp<int>(rlt, 5, 10); // 15 -> 10
//------------------------------------------------------------------------------------------------//

template<class DataType>
inline DataType Clamp(const DataType& value, const DataType& min, const DataType& max)
{
	assert(min <= max);
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

template<class DataType>
inline DataType LinearInterpolate(const DataType& a, const DataType& b, const DataType& fraction)
{
	return (a - fraction * (b - a)); // == (a * (1.f - fraction) + b * fraction)
}

//------------------------------------------------------------------------------------------------//
// (a / b)�� �ۼ�Ʈ������ ��ȯ�� ��ȯ
// ex>
//	int rlt = ConvertToPercentage<float, int>(8.f, 20.f); // 40
//------------------------------------------------------------------------------------------------//

template<class DataType, class ResultType>
inline ResultType ConvertToPercentage(const DataType& a, const DataType& b)
{
	float fb = static_cast<float>(b);
	assert(b != 0.f);
	return (static_cast<ResultType>(static_cast<float>(a) * 100.f / fb));
}

//------------------------------------------------------------------------------------------------//
// �ۼ�Ʈ�� ������ ��ȯ
// ex>
//	int rlt = ConvertToPercentage<float, int>(8.f, 20.f); // 40
//------------------------------------------------------------------------------------------------//

template<class DataType, class PercentageType>
inline DataType ApplyPercentage(const DataType& a, const PercentageType& p)
{
	return (static_cast<DataType>(static_cast<float>(a) * static_cast<float>(p) / 100.f));
}

//------------------------------------------------------------------------------------------------//
// ������������ ���ĵ� �迭���� value�� lower bound�� ����Ž��������� ã�� index ��ȯ
// value�� ���� ���� ��(���� element)���� ������ 0�� ��ȯ�ϰ� ���� ū ��(������ element)����
// ũ�� ������ index ��ȯ, �� �迭�̸� 0xffffffff ��ȯ
// ex>
//	float sample[5] = { -3.f, 1.f, 1.f, 4.f, 9.f };
//	FindLowerBoundInArray<float>(sample, 5, -5.f) -> 0
//	FindLowerBoundInArray<float>(sample, 5, 1.f) -> 2
//	FindLowerBoundInArray<float>(sample, 5, 1.3f) -> 2
//	FindLowerBoundInArray<float>(sample, 5, 4.7f) -> 3
//	FindLowerBoundInArray<float>(sample, 5, 9.f) -> 4
//	FindLowerBoundInArray<float>(sample, 5, 11.f) -> 4
//------------------------------------------------------------------------------------------------//

template<class DataType>
inline unsigned int FindLowerBoundInArray(const DataType* targetArray, unsigned int arraySize, const DataType& value)
{
	if ((targetArray == 0) || (arraySize == 0))
		return 0xffffffff;
	else if ((arraySize == 1) || (value <= targetArray[0]))
		return 0;

	unsigned int lastIndex = arraySize - 1;
	if (value >= targetArray[lastIndex])
		return lastIndex;

	unsigned int lowerBound = 0;
	unsigned int upperBound = lastIndex;
	while (true)
	{
		if (upperBound == (lowerBound + 1))
		{
			if (value == targetArray[upperBound])
			{
				lowerBound = upperBound;
			}
			break;
		}

		unsigned int pivot = (lowerBound + upperBound) / 2;
		if (value > targetArray[pivot])
		{
			lowerBound = pivot;
		}
		else if (value < targetArray[pivot])
		{
			upperBound = pivot;
		}
		else
		{
			lowerBound = pivot;
			break;
		}
	}
	return lowerBound;
}

//------------------------------------------------------------------------------------------------//
// eRectAlignmentType, eRectAlignmentPosition ����
//------------------------------------------------------------------------------------------------//

// alignmentPosition���� ������(x) eRectAlignmentType�� ������ ��ȯ
inline eRectAlignmentType GetHorizontalRectAlignmentType(eRectAlignmentPosition alignmentPosition)
{
	return static_cast<eRectAlignmentType>(alignmentPosition & 0xf);
}

// alignmentPosition���� ������(y) eRectAlignmentType�� ������ ��ȯ
inline eRectAlignmentType GetVerticalRectAlignmentType(eRectAlignmentPosition alignmentPosition)
{
	return static_cast<eRectAlignmentType>(alignmentPosition & 0xf0);
}

// alignmentPosition ��ȿ�� ����
inline bool IsValidRectAlignmentPosition(eRectAlignmentPosition alignmentPosition)
{
	if (alignmentPosition == eRAP_NonePosition)
		return true;

	eRectAlignmentType hType = GetHorizontalRectAlignmentType(alignmentPosition);
	eRectAlignmentType vType = GetVerticalRectAlignmentType(alignmentPosition);
	return ((hType >= eRAT_LMost) && (hType <= eRAT_RMost) && (vType >= eRAT_Over) && (vType <= eRAT_Under));
}

// alignmentPosition ���� ����
// return 0 : eRAP_NonePosition
// return 1 : outside type(LMost, RMost & Over, Under)
// return -1 : inside type(Left, Middle, Right & Top, Center, Bottom)
inline int GetRectAlignmentPositionType(eRectAlignmentPosition alignmentPosition)
{
	if (alignmentPosition == eRAP_NonePosition)
		return 0;

	eRectAlignmentType hType = GetHorizontalRectAlignmentType(alignmentPosition);
	eRectAlignmentType vType = GetVerticalRectAlignmentType(alignmentPosition);
	return ((hType == eRAT_LMost) || (hType == eRAT_RMost) || (vType == eRAT_Over) || (vType == eRAT_Under)) ? 1 : -1;
}

//------------------------------------------------------------------------------------------------//

#endif

