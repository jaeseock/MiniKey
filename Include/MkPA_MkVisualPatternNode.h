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
	// transform
	//------------------------------------------------------------------------------------------------//

	virtual void SetLocalPosition(const MkFloat2& position);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// client size 적용
	virtual void SetClientSize(const MkFloat2& clientSize) { m_WindowSize = m_ClientRect.size = clientSize; }

	// client rect 반환
	inline const MkFloatRect& GetClientRect(void) const { return m_ClientRect; }

	// window size 반환
	inline const MkFloat2& GetWindowSize(void) const { return m_WindowSize; }

	//------------------------------------------------------------------------------------------------//
	// 정렬
	// (NOTE) 호출 전 유효한 window rect가 설정 되어 있는 상태이어야 함
	//------------------------------------------------------------------------------------------------//

	// 정렬 위치 설정
	// UpdateAlignmentPosition()이 호출되면 targetRect의 position에 정렬된 뒤 offset 반영
	// 기본 값은 eRAP_NonePosition, MkFloat2::Zero
	void SetAlignmentPosition(eRectAlignmentPosition position, const MkFloat2& offset = MkFloat2::Zero);

	// 유효한 target rect의 대해 설정된 position, offset을 반영
	// (NOTE) eAT_RestrictedWithinParentClient 설정시 주의(부모 외부로 정렬 될 경우 강제로 부모의 client rect에 맞추어질 수 있음)
	void UpdateAlignmentPosition(const MkFloatRect& targetRect);

	// 부모가 MkVisualPatternNode 및 하위 class일 경우 부모의 client rect를 target rect로 삼아 설정대로 정렬
	void UpdateAlignmentPosition(void);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eVisualPatternNodeAttribute
	{
		// 입력 허용 여부
		eAT_AcceptInput = eAT_SceneNodeBandwidth,

		// 위치를 부모의 client rect 안으로 제한
		eAT_RestrictedWithinParentClient,

		eAT_VisualPatternNodeBandwidth = eAT_SceneNodeBandwidth + 4 // 4bit 대역폭 확보
	};

	// 입력 허용여부. default는 false
	inline void SetAcceptInput(bool enable) { m_Attribute.Assign(eAT_AcceptInput, enable); }
	inline bool GetAcceptInput(void) const { return m_Attribute[eAT_AcceptInput]; }

	// 이동을 부모의 client rect 안으로 제한. default는 false
	// 부모가 MkVisualPatternNode 및 하위 class일 경우만 동작하며 없거나 있더라도 MkSceneNode일 경우 무시
	// (NOTE) 자신의 window rect가 부모의 client rect보다 크지 않아야 정상 동작
	inline void SetRestrictedWithinParentClient(bool enable) { m_Attribute.Assign(eAT_RestrictedWithinParentClient, enable); }
	inline bool GetRestrictedWithinParentClient(void) const { return m_Attribute[eAT_RestrictedWithinParentClient]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

protected:

	enum eVisualPatternNodeEventType
	{
		// 정렬 위치 갱신
		eET_UpdateAlignmentPosition = 0,

		eET_VisualPatternNodeBandwidth,
	};

	virtual void __SendNodeEvent(const _NodeEvent& evt);

	//------------------------------------------------------------------------------------------------//

	MkVisualPatternNode(const MkHashStr& name);
	virtual ~MkVisualPatternNode() {}

protected:

	// region
	MkFloatRect m_ClientRect; // size : 보존, position : 휘발
	MkFloat2 m_WindowSize; // 휘발

	// alignment
	eRectAlignmentPosition m_AlignmentPosition; // 보존
	MkFloat2 m_AlignmentOffset; // 보존

public:

	static const MkStr NamePrefix;
};
