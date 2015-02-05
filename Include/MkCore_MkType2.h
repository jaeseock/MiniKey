#ifndef __MINIKEY_CORE_MKTYPE2_H__
#define __MINIKEY_CORE_MKTYPE2_H__

//------------------------------------------------------------------------------------------------//
// 2d type
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkContainerDefinition.h"


template <class DataType>
class MkType2
{
public:

	inline MkType2() { Clear(); }

	inline MkType2(const DataType& px, const DataType& py)
	{
		x = px;
		y = py;
	}

	inline MkType2(const MkType2& pt)
	{
		x = pt.x;
		y = pt.y;
	}

	inline MkType2& operator = (const MkType2& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	
	inline bool operator == (const MkType2& pt) const { return ((x == pt.x) && (y == pt.y)); }
	inline bool operator != (const MkType2& pt) const { return ((x != pt.x) || (y != pt.y)); }

	inline MkType2 operator + (const MkType2& pt) const { return MkType2(x + pt.x, y + pt.y); }
	inline MkType2 operator - (const MkType2& pt) const { return MkType2(x - pt.x, y - pt.y); }
	inline MkType2 operator * (const DataType& value) const { return MkType2(x * value, y * value); }
	inline friend MkType2 operator * (const DataType& value, const MkType2& pt) { return MkType2(value * pt.x, value * pt.y); }
	inline MkType2 operator / (const DataType& value) const { return MkType2(x / value, y / value); } // divided by zero 예외처리 없음
	inline MkType2 operator - () const { return MkType2(-x, -y); }

	inline MkType2& operator += (const MkType2& pt)
	{
		x += pt.x;
		y += pt.y;
		return *this;
	}

	inline MkType2& operator -= (const MkType2& pt)
	{
		x -= pt.x;
		y -= pt.y;
		return *this;
	}

	inline MkType2& operator *= (const DataType& value)
	{
		x *= value;
		y *= value;
		return *this;
	}

	inline MkType2& operator /= (const DataType& value)
	{
		x /= value; // divided by zero 예외처리 없음
		y /= value;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	
	inline void Clear(void)
	{
		x = static_cast<DataType>(0);
		y = static_cast<DataType>(0);
	}

	inline bool IsZero(void) const
	{
		return ((x == static_cast<DataType>(0)) && (y == static_cast<DataType>(0)));
	}

	// pt와 비교해 작은쪽을 저장
	inline void CompareAndKeepMin(const MkType2& pt)
	{
		if (x > pt.x) x = pt.x;
		if (y > pt.y) y = pt.y;
	}

	// pt와 비교해 큰쪽을 저장
	inline void CompareAndKeepMax(const MkType2& pt)
	{
		if (x < pt.x) x = pt.x;
		if (y < pt.y) y = pt.y;
	}

public:

	DataType x, y;
};

//------------------------------------------------------------------------------------------------//

typedef MkType2<short> MkShort2;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkShort2)

typedef MkType2<unsigned short> MkUShort2;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkUShort2)

typedef MkType2<int> MkInt2;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkInt2)

typedef MkType2<unsigned int> MkUInt2;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkUInt2)

typedef MkType2<long> MkLong2;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkLong2)

typedef MkType2<unsigned long> MkULong2;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkULong2)

typedef MkType2<__int64> MkInt642;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkInt642)

typedef MkType2<unsigned __int64> MkUInt642;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkUInt642)

typedef MkType2<float> MkFloat2;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkFloat2)

typedef MkType2<double> MkDouble2;
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkDouble2)

//------------------------------------------------------------------------------------------------//

#endif
