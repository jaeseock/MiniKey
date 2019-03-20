#pragma once


//------------------------------------------------------------------------------------------------//
// 하나의 이미지를 여러개의 부분으로 쪼개 사용하기 위한 정보
// - Subset은 static image
// - Sequence는 time table에 대한 Subset list, 즉 이미지 교체 애니메이션임
// 외부 시점에서는 Subset과 Sequence는 구분 할 의미가 없음
//
// ex> group
//	str __#Group = "Sample"; // (opt)그룹명. 자원 관리시의 명칭
//
// ex> subset
//	Node "PREFIX(single일 경우 NAME)"
//	{
//		int2 Position = (0, 0); // (opt)위치. 기본값은 (0, 0)
//		int2 Size = (101, 101); // 크기
//		int2 Table = (1, 1); // (opt)테이블의 행렬(xy). 기본값은 (1, 1). 기본값이 (1, 1)이 아닐 경우(복수의 subset 의미) PRIFIX 뒤에 numbering이 붙음
//		int2 Offset = (101, 101); // (opt)Table에서 복수의 subset을 의미할 경우 다음 subset 탐색시의 이동 거리. 수평이동시 x, 수직이동시 y 사용. 기본값은 Size
//	}
//
// ex> sequence
//	Node "NAME"
//	{
//		float TotalRange = 4.5; // 총 시간 구간의 길이(sec)
//		str SubsetList = "P_0" / "P_1" / "P_2"; // subset list
//		//str SubsetList = "~" / "P_" / "0" / "2"; // 위의 리스트를 이런 식으로도 표현 가능. 형식은 ("~" / "PREFIX" / "1st FIRST_INDEX" / "1st LAST_INDEX" / "2nd FIRST_INDEX" / "2nd LAST_INDEX" / ...)
//		float TimeList = 0 / 1.5 / 3; // (opt)subset list와 1:1 대응되는 등장 시간. 없으면 Range를 Subset 갯수만큼 균일하게 쪼개서 사용
//		str Pivot = "LB"; // (opt) 축 기준. 기본값은 LB(left-bottom). "LT", "LC", "LB", "MT", "MC", "MB", "RT", "RC", "RB" 9가지가 존재
//		bool Loop = yes; // (opt)루핑 여부. 기본값은 true
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

	// 초기화
	void SetUp(const MkInt2& imageSize, const MkDataNode* node);

	// 저장
	bool SaveToFile(const MkPathName& filePath, bool textFormat = false) const;

	// 기본(MkHashStr::EMPTY) subset info 변경
	void ResetDefaultSubsetInfo(const MkInt2& srcSize, const MkInt2& realSize);

	// 해당 sequence의 pointer 반환
	const Sequence* GetSequencePtr(const MkHashStr& sequenceName) const;

	// 해당 sequence의 elapsed 구간에 맞는 subset name을 반환
	// 에러 상황이면 EMPTY 반환
	const MkHashStr& GetCurrentSubsetName(const MkHashStr& sequenceName, double elapsed = 0.) const;

	// 해당 sequence의 elapsed 구간에 맞는 subset pointer를 반환
	// 에러 상황이면 NULL 반환
	const Subset* GetCurrentSubsetPtr(const MkHashStr& subsetOrSequenceName, double elapsed = 0.) const;

	// 현재 sequence에 해당하는 pivot을 반환(기본은 eRAP_LeftBottom)
	eRectAlignmentPosition GetCurrentPivot(const MkHashStr& subsetOrSequenceName) const;

	// 사용 가능한 subset, sequence name인지 여부 반환
	inline bool IsValidName(const MkHashStr& subsetOrSequenceName) const { return (m_Subsets.Exist(subsetOrSequenceName) || m_Sequences.Exist(subsetOrSequenceName)); }

	// 그룹명 반환
	inline const MkHashStr& GetGroup(void) const { return m_Group; }

	// subset 참조 반환
	inline const MkHashMap<MkHashStr, Subset>& GetSubsets(void) const { return m_Subsets; }

	// sequence 참조 반환
	inline const MkHashMap<MkHashStr, Sequence>& GetSequences(void) const { return m_Sequences; }

	// 해제
	void Clear(void);

	// deep copy
	//MkImageInfo& operator = (const MkImageInfo& source);

	MkImageInfo() {}
	virtual ~MkImageInfo() { Clear(); }

	//inline void __CopyTo(MkHashMap<MkHashStr, Subset>& target) const { m_Subsets.CopyTo(target); }
	//inline void __CopyTo(MkHashMap<MkHashStr, Sequence>& target) const { m_Sequences.CopyTo(target); }

	// 외부 조작
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
