#pragma once


//------------------------------------------------------------------------------------------------//
// window visual pattern node
// window system의 base node
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkSceneNode.h"


class MkVisualPatternNode : public MkSceneNode
{
public:

	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_VisualPatternNode; }

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client size 적용
	virtual void SetClientSize(const MkFloat2& clientSize) { m_ClientRect.size = clientSize; }

	// client rect 반환
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window size 반환
	inline const MkFloat2& GetWindowSize(void) const { return m_WindowSize; }

	//------------------------------------------------------------------------------------------------//
	// 정렬
	// (NOTE) 호출 전 유효한 window rect가 설정 되어 있는 상태이어야 함
	//------------------------------------------------------------------------------------------------//

	// 유효한 target rect의 alignmentPosition에 위치하도록 local position을 갱신
	void SnapTo(const MkFloatRect& targetRect, eRectAlignmentPosition alignmentPosition);

	// 부모가 MkVisualPatternNode 및 하위 class일 경우 부모 clirent rect의 alignmentPosition에 위치하도록 local position을 갱신
	// 부모가 없거나 있더라도 MkSceneNode일 경우 무시
	void SnapToParentClientRect(eRectAlignmentPosition alignmentPosition);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeAttribute
	{
		eAT_AcceptInput = eAT_SceneNodeBandwidth, // 입력 허용 여부

		eAT_VisualPatternNodeBandwidth = eAT_SceneNodeBandwidth + 4 // 4bit 대역폭 확보
	};

	// 입력 허용여부. default는 false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(eAT_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[eAT_AcceptInput]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

protected:

	enum eVisualPatternNodeEventType
	{
		eET_VisualPatternNodeBandwidth = 0 // event 없음
	};

	//------------------------------------------------------------------------------------------------//

	MkVisualPatternNode(const MkHashStr& name) : MkSceneNode(name) {}
	virtual ~MkVisualPatternNode() {}

protected:

	MkFloatRect m_ClientRect; // size : 보존, position : 휘발
	MkFloat2 m_WindowSize; // 휘발

public:

	static const MkStr NamePrefix;
};
