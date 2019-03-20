#pragma once


//------------------------------------------------------------------------------------------------//
// �ϳ��� �̹����� �������� �κ����� �ɰ� ����ϱ� ���� ����
// - Subset�� static image
// - Sequence�� time table�� ���� Subset list, �� �̹��� ��ü �ִϸ��̼���
// �ܺ� ���������� Subset�� Sequence�� ���� �� �ǹ̰� ����
//
// ex> group
//	str __#Group = "Sample"; // (opt)�׷��. �ڿ� �������� ��Ī
//
// ex> subset
//	Node "PREFIX(single�� ��� NAME)"
//	{
//		int2 Position = (0, 0); // (opt)��ġ. �⺻���� (0, 0)
//		int2 Size = (101, 101); // ũ��
//		int2 Table = (1, 1); // (opt)���̺��� ���(xy). �⺻���� (1, 1). �⺻���� (1, 1)�� �ƴ� ���(������ subset �ǹ�) PRIFIX �ڿ� numbering�� ����
//		int2 Offset = (101, 101); // (opt)Table���� ������ subset�� �ǹ��� ��� ���� subset Ž������ �̵� �Ÿ�. �����̵��� x, �����̵��� y ���. �⺻���� Size
//	}
//
// ex> sequence
//	Node "NAME"
//	{
//		float TotalRange = 4.5; // �� �ð� ������ ����(sec)
//		str SubsetList = "P_0" / "P_1" / "P_2"; // subset list
//		//str SubsetList = "~" / "P_" / "0" / "2"; // ���� ����Ʈ�� �̷� �����ε� ǥ�� ����. ������ ("~" / "PREFIX" / "1st FIRST_INDEX" / "1st LAST_INDEX" / "2nd FIRST_INDEX" / "2nd LAST_INDEX" / ...)
//		float TimeList = 0 / 1.5 / 3; // (opt)subset list�� 1:1 �����Ǵ� ���� �ð�. ������ Range�� Subset ������ŭ �����ϰ� �ɰ��� ���
//		str Pivot = "LB"; // (opt) �� ����. �⺻���� LB(left-bottom). "LT", "LC", "LB", "MT", "MC", "MB", "RT", "RC", "RB" 9������ ����
//		bool Loop = yes; // (opt)���� ����. �⺻���� true
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"
#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


class MkDataNode;

class MkImageInfo
{
public:

	typedef struct _Subset
	{
		MkInt2 position;
		MkFloat2 rectSize;
		MkFloat2 uv[MkFloatRect::eMaxPointName];
	}
	Subset;

	typedef struct _Sequence
	{
		double totalRange;
		MkMap<double, MkHashStr> tracks;
		eRectAlignmentPosition pivot;
		bool loop;
	}
	Sequence;

	// �ʱ�ȭ
	void SetUp(const MkInt2& imageSize, const MkDataNode* node);

	// ����
	bool SaveToFile(const MkPathName& filePath, bool textFormat = false) const;

	// �⺻(MkHashStr::EMPTY) subset info ����
	void ResetDefaultSubsetInfo(const MkInt2& srcSize, const MkInt2& realSize);

	// �ش� sequence�� pointer ��ȯ
	const Sequence* GetSequencePtr(const MkHashStr& sequenceName) const;

	// �ش� sequence�� elapsed ������ �´� subset name�� ��ȯ
	// ���� ��Ȳ�̸� EMPTY ��ȯ
	const MkHashStr& GetCurrentSubsetName(const MkHashStr& sequenceName, double elapsed = 0.) const;

	// �ش� sequence�� elapsed ������ �´� subset pointer�� ��ȯ
	// ���� ��Ȳ�̸� NULL ��ȯ
	const Subset* GetCurrentSubsetPtr(const MkHashStr& subsetOrSequenceName, double elapsed = 0.) const;

	// ���� sequence�� �ش��ϴ� pivot�� ��ȯ(�⺻�� eRAP_LeftBottom)
	eRectAlignmentPosition GetCurrentPivot(const MkHashStr& subsetOrSequenceName) const;

	// ��� ������ subset, sequence name���� ���� ��ȯ
	inline bool IsValidName(const MkHashStr& subsetOrSequenceName) const { return (m_Subsets.Exist(subsetOrSequenceName) || m_Sequences.Exist(subsetOrSequenceName)); }

	// �׷�� ��ȯ
	inline const MkHashStr& GetGroup(void) const { return m_Group; }

	// subset ���� ��ȯ
	inline const MkHashMap<MkHashStr, Subset>& GetSubsets(void) const { return m_Subsets; }

	// sequence ���� ��ȯ
	inline const MkHashMap<MkHashStr, Sequence>& GetSequences(void) const { return m_Sequences; }

	// ����
	void Clear(void);

	// deep copy
	//MkImageInfo& operator = (const MkImageInfo& source);

	MkImageInfo() {}
	virtual ~MkImageInfo() { Clear(); }

	//inline void __CopyTo(MkHashMap<MkHashStr, Subset>& target) const { m_Subsets.CopyTo(target); }
	//inline void __CopyTo(MkHashMap<MkHashStr, Sequence>& target) const { m_Sequences.CopyTo(target); }

	// �ܺ� ����
	void SetGroup(const MkHashStr& name);

	void UpdateSubset(const MkHashStr& name, const MkIntRect& subsetRect, const MkInt2& imgSize);
	void UpdateSubset(const MkHashMap<MkHashStr, MkIntRect>& subsets, const MkInt2& imgSize);
	void ChangeSubset(const MkHashStr& lastName, const MkHashStr& currName);
	void RemoveSubset(const MkHashStr& name);

	void UpdateSequence(const MkHashStr& name, const Sequence& sequence);
	void UpdateSequence(const MkHashMap<MkHashStr, Sequence>& sequences);
	void ChangeSequence(const MkHashStr& lastName, const MkHashStr& currName);
	void RemoveSequence(const MkHashStr& name);

protected:

	void _CreateEmptySubset(const MkFloat2& imgSize);
	void _UpdateSubset(const MkHashStr& name, const MkFloat2& subsetSize, const MkInt2& pivot, const MkFloat2& imgSize, const MkFloat2& uvSize);

protected:

	MkHashStr m_Group;
	
	MkHashMap<MkHashStr, Subset> m_Subsets;
	MkHashMap<MkHashStr, Sequence> m_Sequences;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkImageInfo::Subset)
