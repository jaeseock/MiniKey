#pragma once


//------------------------------------------------------------------------------------------------//
// �ϳ��� �̹����� �������� �κ����� �ɰ� ����ϱ� ���� ����
// - Subset�� static image
// - Sequence�� time table�� ���� Subset list, �� �̹��� ��ü �ִϸ��̼���
// �ܺ� ���������� Subset�� Sequence�� ���� �� �ǹ̰� ����
//
// ex> subset
//	Node "PRIFIX(single�� ��� NAME)"
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
//		bool Loop = yes; // (opt)���� ����. �⺻���� true
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"
#include "MkCore_MkMap.h"
#include "MkCore_MkHashStr.h"


class MkDataNode;

class MkImageInfo
{
public:

	typedef struct _Subset
	{
		MkFloat2 rectSize;
		MkFloat2 uv[MkFloatRect::eMaxPointName];
	}
	Subset;

	typedef struct _Sequence
	{
		double totalRange;
		MkMap<double, MkHashStr> tracks;
		bool loop;
	}
	Sequence;

	// �ʱ�ȭ
	void SetUp(const MkInt2& imageSize, const MkDataNode* node);

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

	// ��� ������ subset, sequence name���� ���� ��ȯ
	inline bool IsValidName(const MkHashStr& subsetOrSequenceName) const { return (m_Subsets.Exist(subsetOrSequenceName) || m_Sequences.Exist(subsetOrSequenceName)); }

	// �׷�� ��ȯ
	inline const MkHashStr& GetGroup(void) const { return m_Group; }

	// ��� subset name ��ȯ
	inline unsigned int GetAllSubsets(MkArray<MkHashStr>& buffer) const { return m_Subsets.GetKeyList(buffer); }

	// ��� sequence name ��ȯ
	inline unsigned int GetAllSequences(MkArray<MkHashStr>& buffer) const { return m_Sequences.GetKeyList(buffer); }

	// ����
	void Clear(void);

	// deep copy
	//MkImageInfo& operator = (const MkImageInfo& source);

	MkImageInfo() {}
	virtual ~MkImageInfo() { Clear(); }

	//inline void __CopyTo(MkHashMap<MkHashStr, Subset>& target) const { m_Subsets.CopyTo(target); }
	//inline void __CopyTo(MkHashMap<MkHashStr, Sequence>& target) const { m_Sequences.CopyTo(target); }

protected:

	void _RegisterSubset(const MkHashStr& name, const MkFloat2& subsetSize, const MkInt2& pivot, const MkFloat2& imgSize, const MkFloat2& uvSize);

protected:

	MkHashStr m_Group;
	
	MkHashMap<MkHashStr, Subset> m_Subsets;
	MkHashMap<MkHashStr, Sequence> m_Sequences;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkImageInfo::Subset)
