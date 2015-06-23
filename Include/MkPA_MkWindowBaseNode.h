#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system의 action part의 base class
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalDefinition.h"
#include "MkPA_MkWindowThemedNode.h"


class MkWindowBaseNode : public MkWindowThemedNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowBaseNode; }

	// alloc child instance
	static MkWindowBaseNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// window system interface
	//------------------------------------------------------------------------------------------------//

	// cursor input
	// 현 프레임에서 cursor를 소유하고 있거나 이전 프레임에 소유했었다면 window manager node에 의해 호출됨
	virtual void UpdateCursorInput
		(
		const MkInt2& position, // cursor의 위치
		const MkInt2& movement, // 이전 프레임의 cursor 위치와의 변동값(이전 프레임의 cursor 위치 = position - movement)
		bool cursorInside, // cursor 소유여부
		eButtonState leftBS,
		eButtonState middleBS,
		eButtonState rightBS,
		int wheelDelta // wheel의 변동값
		);

	// key input
	// 해당 window system에서 key input target으로 설정되어 있으면 window manager node에 의해 호출됨
	// x(0:released, 1:pressed), y(key code)
	virtual void UpdateKeyInput(const MkArray<MkInt2>& eventList) {}

	// activation : call by SendNodeCommandTypeEvent()
	virtual void Activate(void);
	virtual void Deactivate(void);

	// focus : call by SendNodeCommandTypeEvent()
	virtual void OnFocus(void);
	virtual void LostFocus(void);

	// window path
	// root window node(window manager node에 등록된 직계 하위 node) 부터 현 node가지의 경로를 path에 넣어 반환
	void GetWindowPath(MkArray<MkHashStr>& path) const;

	// window frame type 반환
	MkWindowThemeData::eFrameType GetWindowFrameType(void) const { return m_WindowFrameType; }

	// 특정 form type 여부 반환
	inline bool IsDualForm(void) { return (GetFormType() == MkWindowThemeFormData::eFT_DualUnit); }
	inline bool IsQuadForm(void) { return (GetFormType() == MkWindowThemeFormData::eFT_QuadUnit); }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// enable. default는 true
	void SetEnable(bool enable);
	inline bool GetEnable(void) const { return m_Attribute[ePA_SNA_Enable]; }

	// cursor dragging으로 이동 가능 여부. default는 false
	inline void SetMovableByDragging(bool enable) { m_Attribute.Assign(ePA_SNA_MovableByDragging, enable); }
	inline bool GetMovableByDragging(void) const { return m_Attribute[ePA_SNA_MovableByDragging]; }

	// 직계 자식들에게 ePA_SNA_MovableByDragging가 설정되어 있을 시 제한 영역. NULL이면 무시
	virtual const MkFloatRect* GetDraggingMovementLock(void) const { return NULL; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument);
	
	//------------------------------------------------------------------------------------------------//

	virtual void Clear(void);

	MKDEF_DECLARE_SCENE_OBJECT_HEADER; // MkSceneObject

	MkWindowBaseNode(const MkHashStr& name);
	virtual ~MkWindowBaseNode() { Clear(); }

protected:

	void _StartCursorReport(ePA_SceneNodeEvent evt, const MkInt2& position);

protected:

	MkWindowThemeData::eFrameType m_WindowFrameType;

	bool m_CursorInside;

public:

	static const MkHashStr ArgKey_CursorLocalPosition;
	static const MkHashStr ArgKey_WheelDelta;
	static const MkHashStr ArgKey_ExclusiveWindow;
	static const MkHashStr ArgKey_ExclusiveException;

	static const MkHashStr ObjKey_WindowFrameType;
};
