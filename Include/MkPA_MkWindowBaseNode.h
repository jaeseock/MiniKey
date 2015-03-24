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

	// key input
	enum eKeyInputType
	{
		eKIT_Press = 0,
		eKIT_Release
	};

	typedef struct _KeyInputEvent
	{
		eKeyInputType type;
		int keyCode;
	}
	KeyInputEvent;

	virtual void UpdateKeyInput(const MkArray<KeyInputEvent>& eventList) {}

	// cursor 상태 통지
	// cursor를 소유하고 있거나 이전 프레임에 소유했었다면 window manager node에 의해 호출됨
	virtual void UpdateCursorState
		(
		const MkInt2& position, // cursor의 위치
		const MkInt2& movement, // 이전 프레임의 cursor 위치와의 변동값(이전 프레임의 cursor 위치 = position - movement)
		bool cursorInside, // cursor 소유여부
		eCursorState leftCS,
		eCursorState middleCS,
		eCursorState rightCS,
		int wheelDelta // wheel의 변동값
		);

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

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// enable. default는 true
	void SetEnable(bool enable);
	inline bool GetEnable(void) const { return m_Attribute[ePA_SNA_Enable]; }

	// cursor dragging으로 이동 가능 여부. default는 false
	inline void SetMovableByDragging(bool enable) { m_Attribute.Assign(ePA_SNA_MovableByDragging, enable); }
	inline bool GetMovableByDragging(void) const { return m_Attribute[ePA_SNA_MovableByDragging]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument);
	
	//------------------------------------------------------------------------------------------------//

	virtual void Clear(void);

	MkWindowBaseNode(const MkHashStr& name);
	virtual ~MkWindowBaseNode() { Clear(); }

protected:

	inline bool _IsDualForm(void) { return (GetFormType() == MkWindowThemeFormData::eFT_DualUnit); }
	inline bool _IsQuadForm(void) { return (GetFormType() == MkWindowThemeFormData::eFT_QuadUnit); }

protected:

	MkWindowThemeData::eFrameType m_WindowFrameType;

public:
	//static const MkHashStr CaptionTagNodeName;
};
