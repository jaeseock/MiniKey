#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system�� action part�� base class
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalDefinition.h"
#include "MkPA_MkWindowThemedNode.h"


class MkWindowManagerNode;

class MkWindowBaseNode : public MkWindowThemedNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowBaseNode; }

	// alloc child instance
	static MkWindowBaseNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	// ��ϵǾ� �ִ� window manager instance ��ȯ
	MkWindowManagerNode* GetWindowManagerNode(void);

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

	// Ư�� form type ���� ��ȯ
	inline bool IsDualForm(void) { return (GetFormType() == MkWindowThemeFormData::eFT_DualUnit); }
	inline bool IsQuadForm(void) { return (GetFormType() == MkWindowThemeFormData::eFT_QuadUnit); }

	//------------------------------------------------------------------------------------------------//
	// call back
	// cursor click event �߻��� ��� window�� ���� : ePA_SNE_Cursor(L/M/R)Btn(Pressed/Released/DBClicked)
	// ȣ�� ������ SendNodeReportTypeEvent(cursor event) ȣ�� ����
	// (NOTE) call back target�� caller�� ���� window manager�� ��ϵǾ� �־�� ��
	//------------------------------------------------------------------------------------------------//

	// ��� window ��� ����/��ȯ
	inline void SetCallBackTargetWindowPath(const MkArray<MkHashStr>& targetPath) { m_CallBackTargetWindowPath = targetPath; }
	inline const MkArray<MkHashStr>& GetCallBackTargetWindowPath(void) const { return m_CallBackTargetWindowPath; }

	// cursor click event �߻� ����
	virtual void CallBackOperation(ePA_SceneNodeEvent evt, const MkArray<MkHashStr>& callerPath) {}

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// enable. default�� true
	void SetEnable(bool enable);
	inline bool GetEnable(void) const { return m_Attribute[ePA_SNA_Enable]; }

	// cursor dragging���� �̵� ���� ����. default�� false
	inline void SetMovableByDragging(bool enable) { m_Attribute.Assign(ePA_SNA_MovableByDragging, enable); }
	inline bool GetMovableByDragging(void) const { return m_Attribute[ePA_SNA_MovableByDragging]; }

	// ���� �ڽĵ鿡�� ePA_SNA_MovableByDragging�� �����Ǿ� ���� �� ���� ����. NULL�̸� ����
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

	// �ֹ߼� ����
	bool m_CursorInside;

	MkArray<MkHashStr> m_CallBackTargetWindowPath;

public:

	static const MkHashStr ArgKey_CursorLocalPosition;
	static const MkHashStr ArgKey_WheelDelta;
	static const MkHashStr ArgKey_ExclusiveWindow;
	static const MkHashStr ArgKey_ExclusiveException;
};
