#pragma once


//------------------------------------------------------------------------------------------------//
// window visual pattern node
// window system�� visual part�� base class
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkSceneNode.h"


class MkWindowBaseNode;

class MkWindowManagerNode : public MkSceneNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_WindowManagerNode; }

	// alloc child instance
	static MkWindowManagerNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// ���� ���� ���� ����
	//------------------------------------------------------------------------------------------------//

	// ���� ��������� ��ġ �� ���� �뿪��. default�� 1000.f
	inline void SetDepthBandwidth(float depth) { m_DepthBandwidth = depth; }
	inline float GetDepthBandwidth(void) const { return m_DepthBandwidth; }

	// �ش� system�� ����� ����. default�� display resolution
	inline void SetTargetRegion(const MkInt2& region) { m_TargetRegion = region; }
	inline const MkInt2& GetTargetRegion(void) const { return m_TargetRegion; }

	// scene portal node ���� manager ����
	inline void SetScenePortalBind(bool enable) { m_ScenePortalBind = enable; }

	// �ش� ������ input ���� ���� ����
	inline void ValidateInputAtThisFrame(void) { m_ValidInputAtThisFrame = true; }

	//------------------------------------------------------------------------------------------------//
	// window ���/����
	//------------------------------------------------------------------------------------------------//

	bool AttachWindow(MkWindowBaseNode* windowNode);

	bool DeleteWindow(const MkHashStr& windowName);

	//------------------------------------------------------------------------------------------------//
	// Ȱ��ȭ/��Ȱ��ȭ
	//------------------------------------------------------------------------------------------------//

	// ���� �������� Ȱ��ȭ ���� ��ȯ
	bool IsActivating(const MkHashStr& windowName) const;

	// �ֻ��� ������ ���� ��ȯ(modal �̰ų� ù��° activating window)
	bool IsFrontWindow(const MkHashStr& windowName) const;

	// Ȱ��ȭ ����
	// modal ���� ����. �� �̹� �������� modal window�� ���� ��� ���õ�
	// (NOTE) ���� Update() ȣ�� �� �ݿ�
	void ActivateWindow(const MkHashStr& windowName, bool modal = false);

	// ��Ȱ��ȭ ����
	// (NOTE) ���� Update() ȣ�� �� �ݿ�
	void DeactivateWindow(const MkHashStr& windowName);

	//------------------------------------------------------------------------------------------------//
	// cursor pivot
	// �⺻������ window system�� root�� input ��ǥ�谡 �����ϴٰ� �����ϰ� ��������� ������ cursor��
	// world position�� �״�� ��� �� �� ������, ���� �׷��� �ʴٸ�(���� �ٸ� ��ǥ����) pivot�� ������
	// cursor�� world position�� ��ȯ �� �־�� ��
	//------------------------------------------------------------------------------------------------//

	inline void SetInputPivotPosition(const MkInt2& pivotPosition) { m_InputPivotPosition = pivotPosition; }
	inline const MkInt2& GetInputPivotPosition(void) const { return m_InputPivotPosition; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	virtual void Update(double currTime = 0.);

	virtual void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	virtual void LoadComplete(const MkDataNode& node);

	MkWindowManagerNode(const MkHashStr& name);
	virtual ~MkWindowManagerNode() { Clear(); }

protected:

	typedef struct __ActivationEvent
	{
		MkHashStr windowName;
		bool activate;
		bool modal;
	}
	_ActivationEvent;

	void _UpdateSceneNodePath(MkArray< MkArray<MkHashStr> >& targetPath, MkArray<MkSceneNode*>& nodeBuffer);
	void _UpdateWindowPath(MkArray< MkArray<MkHashStr> >& targetPath, MkArray<MkWindowBaseNode*>& nodeBuffer);

	// targetNode �� ���� ��� node�� �� ePA_SNA_AcceptInput �Ӽ��� ���� node���� ������� picking�� ������ ����� node/path��
	// �����ϰ� �ִ� MkWindowBaseNode���� ��ȯ
	// (NOTE) ��Ģ �� argument ������ MkSceneNode::PickPanel() ����
	bool _PickWindowBaseNode(const MkSceneNode* targetNode, MkArray<MkWindowBaseNode*>& buffer, const MkFloat2& worldPoint) const;
	bool _PickWindowBaseNode(const MkSceneNode* targetNode, MkArray<MkWindowBaseNode*>& nodeBuffer, MkArray< MkArray<MkHashStr> >& pathBuffer, const MkFloat2& worldPoint) const;

	void _SendCursorDraggingEvent(MkArray< MkArray<MkHashStr> >& pathBuffer, int type); // type : 0(begin), 1(end)

	void _CheckAndRegisterWindowNode(const MkHashStr& name);

	void _CloseExclusiveOpenningWindow(void);

protected:

	//------------------------------------------------------------------------------------------------//
	// ���� ����
	//------------------------------------------------------------------------------------------------//

	// ���� �뿪��
	float m_DepthBandwidth;
	
	// Ȱ��ȭ ������ ���
	MkArray<MkHashStr> m_ActivatingWindows; // front(0), ... rear ������ ����
	MkHashStr m_ModalWindow;
	
	// ��ϵ� root window list
	MkHashMap<MkHashStr, MkWindowBaseNode*> m_RootWindowList;

	//------------------------------------------------------------------------------------------------//
	// �ֹ߼� ����
	//------------------------------------------------------------------------------------------------//

	bool m_AttachWindowLock;
	bool m_DeleteWindowLock;

	// ��� ���� ����
	MkInt2 m_TargetRegion;

	// scene portal node ���� manager���� ����
	bool m_ScenePortalBind;

	// scene portal node ���� manager�̸� ���Ž� input ó�� ����
	bool m_ValidInputAtThisFrame;

	// ���� �����ӿ� ���� �� window
	MkArray<MkHashStr> m_DeletingWindow;

	// ���� �����ӿ� �ݿ� �� Ȱ��ȭ/��Ȱ��ȭ event
	MkArray<_ActivationEvent> m_ActivationEvent;

	// input
	MkInt2 m_InputPivotPosition;

	// �� �ֻ�� ������
	MkHashStr m_CurrentFocusWindow;

	// window path
	// ��� ���(MkWindowBaseNode*)�� �״�� ������ ������ ���ϱ�� ������ ���� node���� attach, detach�ø���
	// ����͸��� ���� ������ ������ �־�� ��. ���� Update()�ÿ��� ����ϴ� �ֹ߼� �����̹Ƿ� path�� ����
	// ��ȿ���� ������ ����ϱ�� ��
	MkArray< MkArray<MkHashStr> > m_CursorOwnedWindowPath; // ���� cursor�� ������ �ִ� ������ ����Ʈ
	MkArray< MkArray<MkHashStr> > m_KeyInputTargetWindowPath; // key �Է��� ���� �� ������ ����Ʈ
	MkArray< MkArray<MkHashStr> > m_LeftCursorDraggingNodePath; // left cursor dragging ���� ������ ����Ʈ

	MkArray<MkHashStr> m_ExclusiveOpenningWindow; // ��Ÿ�� ���������� ������
	MkArray<MkHashStr> m_ExclusiveWindowException; // ��Ÿ�� ���������� ������ ���� ���˽� ���� ���

public:

	static const MkHashStr ModalEffectNodeName;

	static const MkHashStr ObjKey_DepthBandwidth;
	static const MkHashStr ObjKey_DeactivatingWindows;
	static const MkHashStr ObjKey_ActivatingWindows;
	static const MkHashStr ObjKey_ModalWindow;
};
