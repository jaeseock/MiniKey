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

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// enable. default는 true
	void SetEnable(bool enable);
	inline bool GetEnable(void) const { return m_Attribute[ePA_SNA_Enable]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode& argument);

	//------------------------------------------------------------------------------------------------//

	MkWindowBaseNode(const MkHashStr& name);
	virtual ~MkWindowBaseNode() {}

protected:

	inline bool _IsDualForm(void) { return (MkWindowThemeData::GetFormTypeOfComponent(GetComponentType()) == MkWindowThemeFormData::eFT_DualUnit); }
	inline bool _IsQuadForm(void) { return (MkWindowThemeData::GetFormTypeOfComponent(GetComponentType()) == MkWindowThemeFormData::eFT_QuadUnit); }

public:

	//static const MkHashStr IconTagNodeName;
	//static const MkHashStr CaptionTagNodeName;
};
