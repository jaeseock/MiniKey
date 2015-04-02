#pragma once


//------------------------------------------------------------------------------------------------//
// scene portal node
// 독립적인 다른 scene을 연결해주는 node
// 지정된 node(dest node)의 표시 영역이 생성시의 region보다 크면 scroll bar를 자동으로 생성함
// dest node가 MkWindowManagerNode 파생 객체일 경우 독자적인 window system 동작 보장
// (NOTE) dest node는 scene portal node가 속한 scene graph와 독립적인 node이어야 함
//
// ex>
//	// window A 생성
//	MkTitleBarControlNode* winA = MkTitleBarControlNode::CreateChildNode(NULL, L"TitleBar");
//	winA->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 0.f, true);
//	winA->SetIcon(MkWindowThemeData::eIT_Notice);
//	...
//
//	// window system 생성해 window A 등록
//	MkWindowManagerNode* subWinMgr = MkWindowManagerNode::CreateChildNode(NULL, L"WinMgr");
//	subWinMgr->AttachWindow(winA, MkWindowManagerNode::eLT_Normal);
//	subWinMgr->ActivateWindow(L"TitleBar");
//
//	// main window system 생성해 window들을 마구마구 만들어 등록
//	// 그 중 bodyFrame이라는 node가 있다고 가정
//	MkWindowManagerNode* mainWinMgr = MkWindowManagerNode::CreateChildNode(NULL, L"WinMgr");
//	...
//
//	// scene portal node 생성해 subWinMgr을 대상으로 지정
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

	// 대상 node를 지정
	// 지정된 node의 Update()는 해당 scene portal node가 책임 지므로 별도로 Update()를 호출 할 필요 없음
	// 표시 영역은 대상 node가 파생된 class에 따라 다름
	// - MkVisualPatternNode 파생 객체는 window size
	// - 그 외는 total AABR의 size
	// (NOTE) 호출 전 SetScenePortal()가 올바르게 호출된 상태이어야 함
	// (NOTE) destNode는 scene portal node가 속한 scene graph와 독립적인 node이어야 함
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
