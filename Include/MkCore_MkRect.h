#ifndef __MINIKEY_CORE_MKRECT_H__
#define __MINIKEY_CORE_MKRECT_H__

//------------------------------------------------------------------------------------------------//
// 2d rect
// top-down(�Ϲ� ������ ��ǥ��)�� down-top(���� ��ǥ��) ��� ȥ�� ����������,
// GetAbsolutePosition(), GetSnapPosition() ȣ��ÿ��� �ش� ��ǥ�踦 ������ �־�� ��
//
// - x���� left -> right
// - top-down ��ǥ�迡���� �»��� ����
// - down-top ��ǥ�迡���� ���ϰ� ����
//------------------------------------------------------------------------------------------------//

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

	// topDown�� true�� ��� y���� top->down���� ����(�Ϲ� ������ ��ǥ��). false�� ��� down->top���� ����
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

	// ũ�Ⱑ �����ϴ��� ���� ��ȯ
	inline bool SizeIsZero(void) const { return ((size.x == static_cast<DataType>(0)) || (size.y == static_cast<DataType>(0))); }
	inline bool SizeIsNotZero(void) const { return ((size.x > static_cast<DataType>(0)) && (size.y > static_cast<DataType>(0))); }

	// �ʱ�ȭ
	inline void Clear(void)
	{
		position.Clear();
		size.Clear();
	}

	// �ڽŰ� rect�� ��� �����ϴ� �簢������ ����
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

	// �ڽŰ� rect�� ��� �����ϴ� �簢���� ��ȯ
	MkRect GetUnion(const MkRect& rect) const
	{
		MkRect r = *this;
		r.UpdateToUnion(rect);
		return r;
	}

	// �ڽ� ���ο� pt�� ���ϴ��� ���� ��ȯ
	bool CheckIntersection(const MkType2<DataType>& pt) const
	{
		MkType2<DataType> max = position + size;
		return ((pt.x >= position.x) && (pt.y >= position.y) && (pt.x <= max.x) && (pt.y <= max.y));
	}

	// �ڽ� ���ο� pt�� ���ϴ��� ���� ��ȯ. max�� ��輱�� ����
	bool CheckGridIntersection(const MkType2<DataType>& pt) const
	{
		MkType2<DataType> max = position + size;
		return ((pt.x >= position.x) && (pt.y >= position.y) && (pt.x < max.x) && (pt.y < max.y));
	}

	// �ڽŰ� rect�� ��ħ ���� ��ȯ
	bool CheckIntersection(const MkRect& rect) const
	{
		MkRect tmp(position - rect.size, size + rect.size);
		return tmp.CheckIntersection(rect.position);
	}

	// �ڽŰ� rect�� ��ħ ���ο� ��ġ�� ���� ��ȯ
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

	// rect�� ũ�Ⱑ �ڽź��� ���� ��� �ڽ��� �������� rect�� ����� ���ϵ��� ���ѵ� �� ��ġ ��ȯ
	// ex> MkRect(0, 0, 200, 300).Confine(MkRect(-50, 250, 100, 100)) -> MkType2(0, 200)
	inline MkType2<DataType> Confine(const MkRect& rect) const
	{
		DataType x = MkLinearSection<DataType>(position.x, size.x).Confine(MkLinearSection<DataType>(rect.position.x, rect.size.x));
		DataType y = MkLinearSection<DataType>(position.y, size.y).Confine(MkLinearSection<DataType>(rect.position.y, rect.size.y));
		return MkType2<DataType>(x, y);
	}

	// �ڽ��� �������� rect�� alignmentPosition ��ġ�� border��ŭ ������ ���ĵ� ��ġ�� ��ȯ
	// topDown�� true�� ��� y���� top->down���� ����(�Ϲ� ������ ��ǥ��). false�� ��� down->top���� ����
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
