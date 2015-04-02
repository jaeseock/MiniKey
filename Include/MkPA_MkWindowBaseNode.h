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

	// cursor input
	// �� �����ӿ��� cursor�� �����ϰ� �ְų� ���� �����ӿ� �����߾��ٸ� window manager node�� ���� ȣ���
	virtual void UpdateCursorInput
		(
		const MkInt2& position, // cursor�� ��ġ
		const MkInt2& movement, // ���� �������� cursor ��ġ���� ������(���� �������� cursor ��ġ = position - movement)
		bool cursorInside, // cursor ��������
		eButtonState leftBS,
		eButtonState middleBS,
		eButtonState rightBS,
		int wheelDelta // wheel�� ������
		);

	// key input
	// �ش� window system���� key input target���� �����Ǿ� ������ window manager node�� ���� ȣ���
	// x(0:released, 1:pressed), y(key code)
	virtual void UpdateKeyInput(const MkArray<MkInt2>& eventList) {}

	// activation : call by SendNodeCommandTypeEvent()
	virtual void Activate(void);
	virtual void Deactivate(void);

	// focus : call by SendNodeCommandTypeEvent()
	virtual void OnFocus(void);
	virtual void LostFocus(void);

	// window path
	// root window node(window manager node�� ��ϵ� ���� ���� node) ���� �� node������ ��θ� path�� �־� ��ȯ
	void GetWindowPath(MkArray<MkHashStr>& path) const;

	// window frame type ��ȯ
	MkWindowThemeData::eFrameType GetWindowFrameType(void) const { return m_WindowFrameType; }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// enable. default�� true
	void SetEnable(bool enable);
	inline bool GetEnable(void) const { return m_Attribute[ePA_SNA_Enable]; }

	// cursor dragging���� �̵� ���� ����. default�� false
	inline void SetMovableByDragging(bool enable) { m_Attribute.Assign(ePA_SNA_MovableByDragging, enable); }
	inline bool GetMovableByDragging(void) const { return m_Attribute[ePA_SNA_MovableByDragging]; }

	// ePA_SNA_MovableByDragging�� �����Ǿ� ���� �� �θ��� client rect ���� ����. default�� false
	inline void SetLockinRegionIsParentClient(bool enable) { m_Attribute.Assign(ePA_SNA_LockinRegionIsParentClient, enable); }
	inline bool GetLockinRegionIsParentClient(void) const { return m_Attribute[ePA_SNA_LockinRegionIsParentClient]; }

	// ePA_SNA_MovableByDragging�� �����Ǿ� ���� �� �θ��� window rect ���� ����. default�� false
	inline void SetLockinRegionIsParentWindow(bool enable) { m_Attribute.Assign(ePA_SNA_LockinRegionIsParentWindow, enable); }
	inline bool GetLockinRegionIsParentWindow(void) const { return m_Attribute[ePA_SNA_LockinRegionIsParentWindow]; }

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

	void _StartCursorReport(ePA_SceneNodeEvent evt, const MkInt2& position);

protected:

	MkWindowThemeData::eFrameType m_WindowFrameType;

public:

	static const MkHashStr ArgKey_CursorLocalPosition;
	static const MkHashStr ArgKey_WheelDelta;
};
