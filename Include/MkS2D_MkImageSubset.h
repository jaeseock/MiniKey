#pragma once


//------------------------------------------------------------------------------------------------//
// �̹����� �Ϻθ� subset���� ����
// �ϳ��� �̹����� ���� �κ����� �ɰ� �����ϱ� ���� ����
//
// MkDataNode�� �ʱ�ȭ
// ex>
//	// �Ϲ� ����
//	Node "Face"
//	{
//		int2 Position = (101, 0);
//		int2 Size = (101, 101);
//	}
//
//	// ���� ����. Position���� Size ũ�⸸ŭ�� �̹����� Quilt ������ŭ �����Ǿ� �ִٰ� ����
//	// �̸��� ���� + �ε���(ex> "Face_0", ..., "Face_44")
//	Node "Face_"
//	{
//		int2 Position = (0, 0);
//		int2 Size = (101, 101);
//		int Quilt = 45;
//	}
//
//	// ���� ����. Position���� ���� ũ���� �̹����� Table.x * Table.y ���·� �迭�Ǿ� �ִٰ� ����
//	// �̸��� ���� + ���̺� ���¿� ���� postfix
//	Node "NormalWindow_"
//	{
//		int2 Position = (0, 0);
//		int2 Size = (101, 101);
//		int2 Table = (3, 1);	// L(left), M(middle), R(right)
//		int2 Table = (1, 3);	// T(top), C(center), B(bottom)
//		int2 Table = (3, 3);	// LT(left-top), MT(middle-top), RT(right-top),
//								// LC(left-center), MC(middle-center), RC(right-center),
//								// LB(left-bottom), MB(middle-bottom), RB(right-bottom)
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"
#include "MkCore_MkHashStr.h"


class MkDataNode;

class MkImageSubset
{
public:

	void SetUp(const MkUInt2& imageSize, const MkDataNode* node);

	void GetSubsetInfo(const MkHashStr& name, MkFloat2& localSize, MkFloat2 (&uv)[MkFloatRect::eMaxPointName]) const;

	void Clear(void);

	MkImageSubset& operator = (const MkImageSubset& source);

	MkImageSubset() {}
	virtual ~MkImageSubset() { Clear(); }

public:

	typedef struct _Subset
	{
		MkFloat2 originalSize;
		MkFloat2 uv[MkFloatRect::eMaxPointName];
	}
	Subset;

	inline const Subset* __GetDefaultSubsetPtr(void) const { return &m_DefaultSubset; }
	inline void __CopySubsets(MkHashMap<MkHashStr, Subset>& target) const { m_Subsets.CopyTo(target); }

protected:

	void _RegisterSubset(const MkHashStr& name, const MkFloat2& fpos, const MkFloat2& fsize, const MkFloat2& fuv);

protected:

	MkHashMap<MkHashStr, Subset> m_Subsets;
	Subset m_DefaultSubset;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkImageSubset::Subset)
