#pragma once


//------------------------------------------------------------------------------------------------//
// window visual pattern node
// window system의 visual part의 base class
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
	// 정보 설정
	//------------------------------------------------------------------------------------------------//

	// 하위 윈도우들이 위치 할 깊이 대역폭. default는 1000.f
	inline void SetDepthBandwidth(float depth) { m_DepthBandwidth = depth; }
	inline float GetDepthBandwidth(void) const { return m_DepthBandwidth; }

	//------------------------------------------------------------------------------------------------//
	// window 등록/삭제
	//------------------------------------------------------------------------------------------------//

	bool AttachWindow(MkWindowBaseNode* windowNode);

	bool DeleteWindow(const MkHashStr& windowName);

	//------------------------------------------------------------------------------------------------//
	// 활성화/비활성화
	//------------------------------------------------------------------------------------------------//

	// 현재 프레임의 활성화 여부 반환
	bool IsActivating(const MkHashStr& windowName) const;

	// 최상위 윈도우 여부 반환(modal 이거나 첫번째 activating window)
	MkHashStr GetFrontWindowName(void) const;

	// 포커스 윈도우명 반환. 없으면 공문자열
	MkHashStr GetFocusWindowName(void) const;

	// 활성화 지정
	// modal 선언 가능. 단 이미 동작중인 modal window가 있을 경우 무시됨
	// (NOTE) 다음 Update() 호출 시 반영
	void ActivateWindow(const MkHashStr& windowName, bool modal = false);

	// 비활성화 지정
	// (NOTE) 다음 Update() 호출 시 반영
	void DeactivateWindow(const MkHashStr& windowName);

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

	// scene portal을 부모로 두었을 경우 갱신 관련 정보 설정
	void __SetScenePortalLegacy(bool cursorInside, const MkIntRect& systemRect);

	const MkInt2& __GetSystemRectSize(void) const { return m_SystemRect.size; }

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

	// targetNode 및 하위 모든 node들 중 ePA_SNA_AcceptInput 속성을 가진 node들을 대상으로 picking을 실행해 검출된 node/path를
	// 소유하고 있는 MkWindowBaseNode들을 반환
	// (NOTE) 규칙 및 argument 설명은 MkSceneNode::PickPanel() 참조
	bool _PickWindowBaseNode(const MkSceneNode* targetNode, MkArray<MkWindowBaseNode*>& buffer, const MkFloat2& worldPoint) const;
	bool _PickWindowBaseNode(const MkSceneNode* targetNode, MkArray<MkWindowBaseNode*>& nodeBuffer, MkArray< MkArray<MkHashStr> >& pathBuffer, const MkFloat2& worldPoint) const;

	void _SendCursorDraggingEvent(MkArray< MkArray<MkHashStr> >& pathBuffer, int type); // type : 0(begin), 1(end)

	void _CheckAndRegisterWindowNode(const MkHashStr& name);

	void _CloseExclusiveOpenningWindow(void);

protected:

	//------------------------------------------------------------------------------------------------//
	// 설정 정보
	//------------------------------------------------------------------------------------------------//

	// 깊이 대역폭
	float m_DepthBandwidth;
	
	// 활성화 윈도우 목록
	MkArray<MkHashStr> m_ActivatingWindows; // front(0), ... rear 순으로 정렬
	MkHashStr m_ModalWindow;
	
	// 등록된 root window list
	MkHashMap<MkHashStr, MkWindowBaseNode*> m_RootWindowList;

	//------------------------------------------------------------------------------------------------//
	// 휘발성 정보
	//------------------------------------------------------------------------------------------------//

	bool m_AttachWindowLock;
	bool m_DeleteWindowLock;

	// 대상 영역 지정
	MkIntRect m_SystemRect;
	MkInt2 m_LastSystemSize;

	// 부모 scene portal로부터 갱신 정보를 받았는지 여부
	bool m_HasScenePortalLegacy;

	// 부모 scene portal로부터 갱신 정보를 받았을 때 input 처리 여부(scene portal의 cursor 포함 여부)
	bool m_CursorIsInsideOfScenePortal;

	// 다음 프레임에 삭제 될 window
	MkArray<MkHashStr> m_DeletingWindow;

	// 다음 프레임에 반영 될 활성화/비활성화 event
	MkArray<_ActivationEvent> m_ActivationEvent;

	// 현 최상단 윈도우
	MkHashStr m_CurrentFocusWindow;

	// window path
	// 대상 노드(MkWindowBaseNode*)를 그대로 가지고 있으면 편리하기는 하지만 하위 node들의 attach, detach시마다
	// 모니터링을 통해 정보를 갱신해 주어야 함. 따라서 Update()시에만 사용하는 휘발성 참조이므로 path를 통해
	// 유효성을 갱신해 사용하기로 함
	MkArray< MkArray<MkHashStr> > m_CursorOwnedWindowPath; // 현재 cursor를 가지고 있는 윈도우 리스트
	MkArray< MkArray<MkHashStr> > m_KeyInputTargetWindowPath; // key 입력이 적용 될 윈도우 리스트
	MkArray< MkArray<MkHashStr> > m_LeftCursorDraggingNodePath; // left cursor dragging 중인 윈도우 리스트

	MkArray<MkHashStr> m_ExclusiveOpenningWindow; // 배타적 열림상태인 윈도우
	MkArray<MkHashStr> m_ExclusiveWindowException; // 배타적 열림상태인 윈도우 닫힘 점검시 예외 경로

public:

	static const MkHashStr ModalEffectNodeName;

	static const MkHashStr ObjKey_DepthBandwidth;
	static const MkHashStr ObjKey_DeactivatingWindows;
	static const MkHashStr ObjKey_ActivatingWindows;
	static const MkHashStr ObjKey_ModalWindow;
};
