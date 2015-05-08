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
	// ���� ����
	//------------------------------------------------------------------------------------------------//

	// ���� ��������� ��ġ �� ���� �뿪��. default�� 1000.f
	inline void SetDepthBandwidth(float depth) { m_DepthBandwidth = depth; }
	inline float GetDepthBandwidth(void) const { return m_DepthBandwidth; }

	// �ش� system�� ����� ����. default�� display resolution
	inline void SetTargetRegion(const MkInt2& region) { m_TargetRegion = region; }
	inline const MkInt2& GetTargetRegion(void) const { return m_TargetRegion; }

	//------------------------------------------------------------------------------------------------//
	// window ���/����
	// (NOTE) Update()�� ȣ�� ����
	//------------------------------------------------------------------------------------------------//

	enum eLayerType
	{
		eLT_Low = 0, // ���� layer. ���� �شܿ� ��ġ
		eLT_Normal, // �Ϲ� layer
		eLT_High // ���� layer. modal�� �����ϰ� ���� ���ܿ� ��ġ
	};

	bool AttachWindow(MkWindowBaseNode* windowNode, eLayerType layerType = eLT_Normal);
	bool DeleteWindow(const MkHashStr& windowName);

	//------------------------------------------------------------------------------------------------//
	// Ȱ��ȭ/��Ȱ��ȭ
	// (NOTE) ������ ���� �����Ӷ� �ݿ� ��. Update()�� ȣ�� �� ���� �ֱ� ����
	//------------------------------------------------------------------------------------------------//

	// ���� �������� Ȱ��ȭ ���� ��ȯ
	bool IsActivating(const MkHashStr& windowName) const;

	// ���� �������� Ȱ��ȭ ����. modal ���� ����
	void ActivateWindow(const MkHashStr& windowName, bool modal = false);

	// ���� �������� ��Ȱ��ȭ ����
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

	MkWindowManagerNode(const MkHashStr& name);
	virtual ~MkWindowManagerNode() { Clear(); }

protected:

	typedef struct __RootWindowInfo
	{
		MkWindowBaseNode* node;
		eLayerType layerType;
	}
	_RootWindowInfo;

	typedef struct __ActivationEvent
	{
		MkHashStr windowName;
		bool activate;
		bool modal;
	}
	_ActivationEvent;

	void _UpdateSceneNodePath(MkArray< MkArray<MkHashStr> >& targetPath, MkArray<MkSceneNode*>& nodeBuffer);
	void _UpdateWindowPath(MkArray< MkArray<MkHashStr> >& targetPath, MkArray<MkWindowBaseNode*>& nodeBuffer);

	// �ڽŰ� ���� ��� node�� �� ePA_SNA_AcceptInput �Ӽ��� ���� node���� ������� picking�� ������ ����� node/path��
	// �����ϰ� �ִ� MkWindowBaseNode���� ��ȯ
	// (NOTE) ��Ģ �� argument ������ MkSceneNode::PickPanel() ����
	bool _PickWindowBaseNode(MkArray<MkWindowBaseNode*>& buffer, const MkFloat2& worldPoint) const;
	bool _PickWindowBaseNode(MkArray<MkWindowBaseNode*>& nodeBuffer, MkArray< MkArray<MkHashStr> >& pathBuffer, const MkFloat2& worldPoint) const;

	void _SendCursorDraggingEvent(MkArray< MkArray<MkHashStr> >& pathBuffer, int type); // type : 0(begin), 1(end)

protected:

	//------------------------------------------------------------------------------------------------//
	// ���� ����
	//------------------------------------------------------------------------------------------------//

	float m_DepthBandwidth;
	MkInt2 m_TargetRegion;
	
	// ��Ȱ��ȭ ������ ���
	MkArray<MkHashStr> m_DeactivatingWindows;

	// Ȱ��ȭ ������ ���
	MkMap<eLayerType, MkArray<MkHashStr> > m_ActivatingWindows;
	MkHashStr m_ModalWindow;

	// ��ϵ� root window list
	MkHashMap<MkHashStr, _RootWindowInfo> m_RootWindowList;

	//------------------------------------------------------------------------------------------------//
	// �ֹ߼� ����
	//------------------------------------------------------------------------------------------------//

	bool m_UpdateLock;

	// ���� �����ӿ� �ݿ� �� Ȱ��ȭ/��Ȱ��ȭ event
	MkArray<_ActivationEvent> m_ActivationEvent;

	// input
	MkInt2 m_InputPivotPosition;

	// window path
	// ��� ���(MkWindowBaseNode*)�� �״�� ������ ������ ���ϱ�� ������ ���� node���� attach, detach�ø���
	// ����͸��� ���� ������ ������ �־�� ��. ���� Update()�ÿ��� ����ϴ� �ֹ߼� �����̹Ƿ� path�� ����
	// ��ȿ���� ������ ����ϱ�� ��
	MkArray< MkArray<MkHashStr> > m_CursorOwnedWindowPath;
	MkArray< MkArray<MkHashStr> > m_KeyInputTargetWindowPath;
	MkArray< MkArray<MkHashStr> > m_LeftCursorDraggingNodePath;

public:

	//static const MkStr NamePrefix;
};
