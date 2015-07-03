#pragma once


//------------------------------------------------------------------------------------------------//
// window dispositioner
// ex>
//	MkBodyFrameControlNode* bodyFrame...
//	MkWindowTagNode* tagNode...
//	MkWindowBaseNode* okBtn...
//	MkWindowBaseNode* cancelBtn...
//
//	...
//
//	MkWindowDispositioner windowDispositioner;
//
//	windowDispositioner.AddNewLine(); // 빈 영역
//	windowDispositioner.AddDummyToCurrentLine(titleNode->CalculateWindowSize());
//
//	windowDispositioner.AddNewLine(MkWindowDispositioner::eLHA_Center); // text tag는 중앙 정렬
//	windowDispositioner.AddRectToCurrentLine(tagNode->GetNodeName(), tagNode->CalculateWindowSize());
//
//	windowDispositioner.AddNewLine(MkWindowDispositioner::eLHA_Center); // button set도 중앙 정렬
//	windowDispositioner.AddRectToCurrentLine(okBtn->GetNodeName(), okBtn->CalculateWindowSize());
//	windowDispositioner.AddRectToCurrentLine(cancelBtn->GetNodeName(), cancelBtn->CalculateWindowSize());
//
//	windowDispositioner.Calculate(); // dispositioner 계산
//
//	MkFloat2 clientSize = windowDispositioner.GetRegion(); // body frame size
//	bodyFrame->SetBodyFrame(m_ThemeName, bodyType, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, clientSize);
//
//	// 하위 node 위치 반영
//	windowDispositioner.ApplyPositionToNode(tagNode);
//	windowDispositioner.ApplyPositionToNode(okBtn);
//	windowDispositioner.ApplyPositionToNode(cancelBtn);
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkRect.h"


class MkVisualPatternNode;

class MkWindowDispositioner
{
public:

	//------------------------------------------------------------------------------------------------//

	// 전체 영역의 추가 가로/세로 값
	inline void SetMargin(const MkFloat2& margin) { m_Margin = margin; }
	inline const MkFloat2& GetMargin(void) const { return m_Margin; }

	// rect간 거리
	inline void SetOffset(const MkFloat2& offset) { m_Offset = offset; }
	inline const MkFloat2& GetOffset(void) const { return m_Offset; }

	//------------------------------------------------------------------------------------------------//

	enum eLineHorizontalAlign
	{
		eLHA_Left = 0, // default
		eLHA_Center,
		eLHA_Right
	};

	enum eLineVerticalAlign
	{
		eLVA_Top = 0,
		eLVA_Center, // default
		eLVA_Bottom
	};

	// 새 라인 추가. 반환값은 지정된 라인 번호
	unsigned int AddNewLine(eLineHorizontalAlign horizontalAlign = eLHA_Left, eLineVerticalAlign verticalAlign = eLVA_Center);

	// 현재 라인에 rect를 등록
	bool AddRectToCurrentLine(const MkHashStr& name, const MkFloat2& size);

	// 현재 라인에 dummy를 등록
	bool AddDummyToCurrentLine(const MkFloat2& size);

	// 등록 완료 선언. 계산 시작
	void Calculate(void);

	// 계산 완료된 전체 영역 반환
	inline const MkFloat2& GetRegion(void) const { return m_Region; }

	// 해당 rect의 계산 결과 반환
	const MkFloatRect& GetRect(const MkHashStr& name) const;

	// 해당 rect의 계산 결과 반영
	bool ApplyPositionToNode(MkVisualPatternNode* node) const;

	// 초기화
	void Clear(void);
	
	MkWindowDispositioner();
	~MkWindowDispositioner() {}

protected:

	typedef struct __LineInfo
	{
		// given
		MkArray<MkHashStr> rects;

		eLineHorizontalAlign horizontalAlign;
		eLineVerticalAlign verticalAlign;

		// calculation
		MkFloatRect region;
	}
	_LineInfo;

protected:

	// given
	MkFloat2 m_Margin;
	MkFloat2 m_Offset;

	MkHashMap<MkHashStr, MkFloatRect> m_RectTable;
	MkArray<_LineInfo> m_Lines;

	// result
	MkFloat2 m_Region;
};
