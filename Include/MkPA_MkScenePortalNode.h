#pragma once


//------------------------------------------------------------------------------------------------//
// scene portal node
// �������� �ٸ� scene�� �������ִ� node
// ������ node(dest node)�� ǥ�� ������ �������� region���� ũ�� scroll bar�� �ڵ����� ������
// dest node�� MkWindowManagerNode �Ļ� ��ü�� ��� �������� window system ���� ����
// (NOTE) dest node�� scene portal node�� ���� scene graph�� �������� node�̾�� ��
//
// ex>
//	// window A ����
//	MkTitleBarControlNode* winA = MkTitleBarControlNode::CreateChildNode(NULL, L"TitleBar");
//	winA->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 0.f, true);
//	winA->SetIcon(MkWindowThemeData::eIT_Notice);
//	...
//
//	// window system ������ window A ���
//	MkWindowManagerNode* subWinMgr = MkWindowManagerNode::CreateChildNode(NULL, L"WinMgr");
//	subWinMgr->AttachWindow(winA, MkWindowManagerNode::eLT_Normal);
//	subWinMgr->ActivateWindow(L"TitleBar");
//
//	// main window system ������ window���� �������� ����� ���
//	// �� �� bodyFrame�̶�� node�� �ִٰ� ����
//	MkWindowManagerNode* mainWinMgr = MkWindowManagerNode::CreateChildNode(NULL, L"WinMgr");
//	...
//
//	// scene portal node ������ subWinMgr�� ������� ����
//	MkScenePortalNode* scenePortal = MkScenePortalNode::CreateChildNode(bodyFrame, L"ScenePortal");
//	scenePortal->SetScenePortal(MkWindowThemeData::DefaultThemeName, MkFloat2(250.f, 180.f), subWinMgr);
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkScrollBarControlNode;

class MkScenePortalNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_ScenePortalNode; }

	// alloc child instance
	static MkScenePortalNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// portal interface
	//------------------------------------------------------------------------------------------------//

	void SetScenePortal(const MkHashStr& themeName, const MkFloat2& region, MkSceneNode* destNode);

	// ��� node�� ����
	// ������ node�� Update()�� �ش� scene portal node�� å�� ���Ƿ� ������ Update()�� ȣ�� �� �ʿ� ����
	// ǥ�� ������ ��� node�� �Ļ��� class�� ���� �ٸ�
	// - MkVisualPatternNode �Ļ� ��ü�� window size
	// - �� �ܴ� total AABR�� size
	// (NOTE) ȣ�� �� SetScenePortal()�� �ùٸ��� ȣ��� �����̾�� ��
	// (NOTE) destNode�� scene portal node�� ���� scene graph�� �������� node�̾�� ��
	void SetDestinationNode(MkSceneNode* destNode);

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	virtual void Update(double currTime = 0.);

	virtual void Clear(void);

	MkScenePortalNode(const MkHashStr& name);
	virtual ~MkScenePortalNode() { Clear(); }

protected:

	void _UpdateScrollBar(MkScrollBarControlNode* node, int destSize);

protected:

	MkSceneNode* m_DestinationNode;

public:

	static const MkHashStr MaskingPanelName;

	static const MkHashStr HScrollBarName;
	static const MkHashStr VScrollBarName;
};
