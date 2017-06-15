#ifndef __MINIKEY_CORE_MKRECT_H__
#define __MINIKEY_CORE_MKRECT_H__

//------------------------------------------------------------------------------------------------//
// 2d rect
// top-down(일반 윈도우 좌표계)과 down-top(게임 좌표계) 모두 혼용 가능하지만,
// GetAbsolutePosition(), GetSnapPosition() 호출시에는 해당 좌표계를 지정해 주어야 함
//
// - x축은 left -> right
// - top-down 좌표계에서는 좌상이 원점
// - down-top 좌표계에서는 좌하가 원점
//------------------------------------------------------------------------------------------------//

#include <math.h>
#include <float.h>
#include "MkCore_MkLinearSection.h"
#include "MkCore_MkType2.h"


template <class DataType>
class MkRect
{
public:

	enum ePointName
	{
		eLeftTop = 0,
		eRightTop,
		eLeftBottom,
		eRightBottom,

		eMaxPointName
	};

public:

	inline MkRect() { Clear(); }

	inline MkRect(const DataType& xPos, const DataType& yPos, const DataType& xSize, const DataType& ySize)
	{
		position.x = xPos;
		position.y = yPos;
		size.x = xSize;
		size.y = ySize;
	}

	inline MkRect(const MkType2<DataType>& positionIn, const MkType2<DataType>& sizeIn)
	{
		position = positionIn;
		size = sizeIn;
	}

	inline MkRect(const MkType2<DataType>& sizeIn)
	{
		position.Clear();
		size = sizeIn;
	}

	inline MkRect(const MkRect& rect)
	{
		position = rect.position;
		size = rect.size;
	}

	inline MkRect& operator = (const MkRect& rect)
	{
		position = rect.position;
		size = rect.size;
		return *this;
	}

	//------------------------------------------------------------------------------------------------//

	inline bool operator == (const MkRect& rect) const { return ((position == rect.position) && (size == rect.size)); }
	inline bool operator != (const MkRect& rect) const { return ((position != rect.position) || (size != rect.size)); }

	//------------------------------------------------------------------------------------------------//

	// topDown이 true일 경우 y축을 top->down으로 간주(일반 윈도우 좌표계). false일 경우 down->top으로 간주
	inline MkType2<DataType> GetAbsolutePosition(ePointName pointName, bool topDown = false) const
	{
		MkType2<DataType> pt = position;
		if (topDown)
		{
			switch (pointName)
			{
			case eLeftTop: break;
			case eRightTop: pt.x += size.x; break;
			case eLeftBottom: pt.y += size.y; break;
			case eRightBottom: pt += size; break;
			}
		}
		else
		{
			switch (pointName)
			{
			case eLeftTop: pt.y += size.y; break;
			case eRightTop: pt += size; break;
			case eLeftBottom: break;
			case eRightBottom: pt.x += size.x; break;
			}
		}
		return pt;
	}

	// 크기가 존재하는지 여부 반환
	inline bool SizeIsZero(void) const { return ((size.x == static_cast<DataType>(0)) || (size.y == static_cast<DataType>(0))); }
	inline bool SizeIsNotZero(void) const { return ((size.x > static_cast<DataType>(0)) && (size.y > static_cast<DataType>(0))); }

	// 중점 반환
	inline MkType2<DataType> GetCenterPosition(void) const { return (position + size * 0.5f); }

	// 초기화
	inline void Clear(void)
	{
		position.Clear();
		size.Clear();
	}

	// 자신과 rect를 모두 포괄하는 사각형으로 갱신
	// ex> MkRect(0, 0, 100, 200).UpdateToUnion(MkRect(80, 70, 60, 150)) -> MkRect(0, 0, 140, 220)
	void UpdateToUnion(const MkRect& rect)
	{
		if (SizeIsZero())
		{
			*this = rect;
		}
		else if (rect.SizeIsNotZero())
		{
			size += position;
			position.CompareAndKeepMin(rect.position);
			size.CompareAndKeepMax(rect.position + rect.size);
			size -= position;
		}
	}

	// 자신과 rect를 모두 포괄하는 사각형을 반환
	MkRect GetUnion(const MkRect& rect) const
	{
		MkRect r = *this;
		r.UpdateToUnion(rect);
		return r;
	}

	// 자신 내부에 pt가 속하는지 여부 반환
	bool CheckIntersection(const MkType2<DataType>& pt) const
	{
		MkType2<DataType> max = position + size;
		return ((pt.x >= position.x) && (pt.y >= position.y) && (pt.x <= max.x) && (pt.y <= max.y));
	}

	// 자신 내부에 pt가 속하는지 여부 반환. max쪽 경계선은 제외
	bool CheckGridIntersection(const MkType2<DataType>& pt) const
	{
		MkType2<DataType> max = position + size;
		return ((pt.x >= position.x) && (pt.y >= position.y) && (pt.x < max.x) && (pt.y < max.y));
	}

	// 자신과 rect간 겹침 여부 반환
	bool CheckIntersection(const MkRect& rect) const
	{
		MkRect tmp(position - rect.size, size + rect.size);
		return tmp.CheckIntersection(rect.position);
	}

	// 자신과 rect간 겹침 여부와 겹치는 영역 반환
	// ex> MkRect(0, 0, 100, 200).GetIntersection(MkRect(80, 70, 60, 150)) -> true, MkRect(80, 70, 20, 130)
	bool GetIntersection(const MkRect& rect, MkRect& intersection) const
	{
		bool ok = CheckIntersection(rect);
		if (ok)
		{
			MkLinearSection<DataType> ix =
				MkLinearSection<DataType>(position.x, size.x).GetIntersection(MkLinearSection<DataType>(rect.position.x, rect.size.x));

			MkLinearSection<DataType> iy =
				MkLinearSection<DataType>(position.y, size.y).GetIntersection(MkLinearSection<DataType>(rect.position.y, rect.size.y));

			intersection.position.x = ix.GetPosition();
			intersection.position.y = iy.GetPosition();
			intersection.size.x = ix.GetSize();
			intersection.size.y = iy.GetSize();
		}
		return ok;
	}

	// rect의 크기가 자신보다 작을 경우 자신의 영역에서 rect가 벗어나지 못하도록 제한된 새 위치 반환
	// ex> MkRect(0, 0, 200, 300).Confine(MkRect(-50, 250, 100, 100)) -> MkType2(0, 200)
	inline MkType2<DataType> Confine(const MkRect& rect) const
	{
		DataType x = MkLinearSection<DataType>(position.x, size.x).Confine(MkLinearSection<DataType>(rect.position.x, rect.size.x));
		DataType y = MkLinearSection<DataType>(position.y, size.y).Confine(MkLinearSection<DataType>(rect.position.y, rect.size.y));
		return MkType2<DataType>(x, y);
	}

	// rect가 자신과 겹치면 겹치지 않도록 밀어내고 변경 여부를 반환
	// 밀어내는 방향은 중점간의 방향을 기준으로 함
	bool SqueezeOut(MkRect& rect) const
	{
		MkRect intersection;
		bool ok = (GetIntersection(rect, intersection) && intersection.size.IsPositive());
		if (ok)
		{
			// 벡터 연산때문에 실수 계산이 반드시 필요하다
			MkType2<DataType> toTarget = rect.GetCenterPosition() - GetCenterPosition();
			MkFloat2 direction(static_cast<float>(toTarget.x), static_cast<float>(toTarget.y));

			// 방향 단위벡터를 구함. 중점이 겹치면 (0.f, -1.f)를, 아니면 자신 중점부터 rect 중점까지의 벡터를 사용
			float length = sqrt(direction.x * direction.x + direction.y * direction.y);
			direction = (length == 0.f) ? MkFloat2(0.f, -1.f) : (direction / length);

			// 이동할 벡터 결정
			float xRate = (direction.x == 0) ? FLT_MAX : abs(static_cast<float>(intersection.size.x) / direction.x); // x축 배율
			float yRate = (direction.y == 0) ? FLT_MAX : abs(static_cast<float>(intersection.size.y) / direction.y); // y축 배율
			direction *= GetMin<float>(xRate, yRate);
			MkType2<DataType> movement(static_cast<DataType>(direction.x), static_cast<DataType>(direction.y));

			// 이산학상 제로일 수 있으므로 체크
			ok = !movement.IsZero();
			if (ok)
			{
				rect.position += movement;
			}
		}
		return ok;
	}

	// 자신을 기준으로 rect가 alignmentPosition 위치에 border만큼 떨어져 정렬될 위치를 반환
	// topDown이 true일 경우 y축을 top->down으로 간주(일반 윈도우 좌표계). false일 경우 down->top으로 간주
	MkType2<DataType> GetSnapPosition(const MkRect& rect, eRectAlignmentPosition alignmentPosition, const MkType2<DataType>& border, bool topDown = false) const
	{
		MkType2<DataType> newPos = rect.position;
		if ((alignmentPosition != eRAP_NonePosition) && IsValidRectAlignmentPosition(alignmentPosition))
		{
			switch (GetHorizontalRectAlignmentType(alignmentPosition))
			{
			case eRAT_LMost: newPos.x = position.x - rect.size.x - border.x; break;
			case eRAT_Left: newPos.x = position.x + border.x; break;
			case eRAT_Middle: newPos.x = position.x + ((size.x - rect.size.x) / static_cast<DataType>(2)); break;
			case eRAT_Right: newPos.x = position.x + size.x - rect.size.x - border.x; break;
			case eRAT_RMost: newPos.x = position.x + size.x + border.x; break;
			}

			if (topDown)
			{
				switch (GetVerticalRectAlignmentType(alignmentPosition))
				{
				case eRAT_Over: newPos.y = position.y - rect.size.y - border.y; break;
				case eRAT_Top: newPos.y = position.y + border.y; break;
				case eRAT_Center: newPos.y = position.y + ((size.y - rect.size.y) / static_cast<DataType>(2)); break;
				case eRAT_Bottom: newPos.y = position.y + size.y - rect.size.y - border.y; break;
				case eRAT_Under: newPos.y = position.y + size.y + border.y; break;
				}
			}
			else
			{
				switch (GetVerticalRectAlignmentType(alignmentPosition))
				{
				case eRAT_Over: newPos.y = position.y + size.y + border.y; break;
				case eRAT_Top: newPos.y = position.y + size.y - rect.size.y - border.y; break;
				case eRAT_Center: newPos.y = position.y + ((size.y - rect.size.y) / static_cast<DataType>(2)); break;
				case eRAT_Bottom: newPos.y = position.y + border.y; break;
				case eRAT_Under: newPos.y = position.y - rect.size.y - border.y; break;
				}
			}
		}
		return newPos;
	}

	inline MkType2<DataType> GetSnapPosition
		(const MkType2<DataType>& rectSize, eRectAlignmentPosition alignmentPosition, const MkType2<DataType>& border, bool topDown = false) const
	{
		return GetSnapPosition(MkRect<DataType>(MkType2<DataType>::Zero, rectSize), alignmentPosition, border, topDown);
	}

public:

	MkType2<DataType> position;
	MkType2<DataType> size;

	static const MkRect<DataType> EMPTY;
};

//------------------------------------------------------------------------------------------------//

typedef MkRect<int> MkIntRect; // & MkInt2
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkIntRect)

typedef MkRect<__int64> MkInt64Rect; // & MkInt642
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkInt64Rect)

typedef MkRect<float> MkFloatRect; // & MkFloat2
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkFloatRect)

typedef MkRect<double> MkDoubleRect; // & MkDouble2
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkDoubleRect)

//------------------------------------------------------------------------------------------------//

#endif
