#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system�� action part�� base class
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

	// cursor ���� ����
	// cursor�� �����ϰ� �ְų� ���� �����ӿ� �����߾��ٸ� window manager node�� ���� ȣ���
	virtual void UpdateCursorState
		(
		const MkInt2& position, // cursor�� ��ġ
		const MkInt2& movement, // ���� �������� cursor ��ġ���� ������(���� �������� cursor ��ġ = position - movement)
		bool cursorInside, // cursor ��������
		eCursorState leftCS,
		eCursorState middleCS,
		eCursorState rightCS,
		int wheelDelta // wheel�� ������
		);

	// activation : call by SendNodeCommandTypeEvent()
	virtual void Activate(void);
	virtual void Deactivate(void);

	// focus : call by SendNodeCommandTypeEvent()
	virtual void OnFocus(void);
	virtual void LostFocus(void);

	// window path
	// root window node(window manager node�� ��ϵ� ���� ���� node) ���� �� node������ ��θ� path�� �־� ��ȯ
	void GetWindowPath(MkArray<MkHashStr>& path) const;

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// enable. default�� true
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
