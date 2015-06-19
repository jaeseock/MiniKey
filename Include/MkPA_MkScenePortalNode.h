#pragma once


//------------------------------------------------------------------------------------------------//
// scene portal node
// 독립적인 다른 scene을 연결해주는 node
// 지정된 node(dest node)의 표시 영역이 생성시의 region보다 크면 scroll bar를 자동으로 생성함
// dest node가 MkWindowManagerNode 파생 객체일 경우 독자적인 window system 동작 보장
// (NOTE) dest node는 scene portal node가 속한 scene graph와 독립적인 node이어야 함
//
//------------------------------------------------------------------------------------------------//
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
// ex> 위의 예는 아래와 동일
//	// main window system 생성해 window들을 마구마구 만들어 등록
//	// 그 중 bodyFrame이라는 node가 있다고 가정
//	MkWindowManagerNode* mainWinMgr = MkWindowManagerNode::CreateChildNode(NULL, L"WinMgr");
//	...
//
//	// scene portal node 생성해 subWinMgr을 생성해 대상으로 지정하고 반환
//	MkScenePortalNode* scenePortal = MkScenePortalNode::CreateChildNode(bodyFrame, L"ScenePortal");
//	MkWindowManagerNode* subWinMgr = scenePortal->CreateScenePortal(MkWindowThemeData::DefaultThemeName, MkFloat2(250.f, 180.f));
//
//	// window A 생성
//	MkTitleBarControlNode* winA = MkTitleBarControlNode::CreateChildNode(NULL, L"TitleBar");
//	winA->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 0.f, true);
//	winA->SetIcon(MkWindowThemeData::eIT_Notice);
//	...
//
//	// subWinMgr에 window A 등록
//	subWinMgr->AttachWindow(winA, MkWindowManagerNode::eLT_Normal);
//	subWinMgr->ActivateWindow(L"TitleBar");
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkWindowManagerNode;
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

	// 대상 node를 지정해 초기화
	// (NOTE) destNode의 주의사항은 SetDestinationNode()의 주의사항 참조
	void SetScenePortal(const MkHashStr& themeName, const MkFloat2& region, MkSceneNode* destNode);

	// 대상 node를 지정
	// 지정된 node의 Update()는 해당 scene portal node가 책임 지므로 별도로 Update()를 호출 할 필요 없음
	// 표시 영역은 대상 node가 파생된 class에 따라 다름
	// - MkVisualPatternNode 파생 객체는 window size
	// - 그 외는 total AABR의 size
	// (NOTE) 호출 전 SetScenePortal()가 올바르게 호출된 상태이어야 함
	// (NOTE) destNode는 scene portal node가 속한 scene graph와 독립적인 node이어야 함
	// (NOTE) 이전 CreateScenePortal()로 만들어진 자체적인 window mgr을 가지고 있었다면 삭제 됨
	// (NOTE) 출력/저장 시 destNode 관련 정보는 입출력되지 않음(로딩시 별도로 SetDestinationNode()을 호출해 주어야 함)
	void SetDestinationNode(MkSceneNode* destNode);

	// 대상 node 반환
	inline MkSceneNode* GetDestinationNode(void) { return m_DestinationNode; }

	// 자체적인 window manager를 만들어 대상 node 지정 후 반환
	// (NOTE) 호출 전 window mgr/destination node가 존재하면 삭제(window mgr), 초기화(destination node) 후 재생성
	// (NOTE) 출력/저장 시 자체 window manager 정보까지 모두 입출력됨
	MkWindowManagerNode* CreateScenePortal(const MkHashStr& themeName, const MkFloat2& region);

	// 자체 window manager 반환
	inline MkWindowManagerNode* GetDestWindowManagerNode(void) { return m_DestWindowManagerNode; }

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

	MkScenePortalNode(const MkHashStr& name);
	virtual ~MkScenePortalNode() { Clear(); }

protected:

	void _UpdateScrollBar(MkScrollBarControlNode* node, int destSize);

protected:

	MkSceneNode* m_DestinationNode;

	MkWindowManagerNode* m_DestWindowManagerNode;

public:

	static const MkHashStr MaskingPanelName;
	static const MkHashStr SampleBackgroundName;

	static const MkHashStr HScrollBarName;
	static const MkHashStr VScrollBarName;

	static const MkHashStr ObjKey_DestWindowMgr;
};
