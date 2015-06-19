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
	// 갱신 이전 정보 설정
	//------------------------------------------------------------------------------------------------//

	// 하위 윈도우들이 위치 할 깊이 대역폭. default는 1000.f
	inline void SetDepthBandwidth(float depth) { m_DepthBandwidth = depth; }
	inline float GetDepthBandwidth(void) const { return m_DepthBandwidth; }

	// 해당 system이 사용할 영역. default는 display resolution
	inline void SetTargetRegion(const MkInt2& region) { m_TargetRegion = region; }
	inline const MkInt2& GetTargetRegion(void) const { return m_TargetRegion; }

	// scene portal node 하위 manager 여부
	inline void SetScenePortalBind(bool enable) { m_ScenePortalBind = enable; }

	// 해당 프레임 input 관련 갱신 금지
	inline void ValidateInputAtThisFrame(void) { m_ValidInputAtThisFrame = true; }

	//------------------------------------------------------------------------------------------------//
	// window 등록/삭제
	// (NOTE) Update()중 호출 금지
	//------------------------------------------------------------------------------------------------//

	bool AttachWindow(MkWindowBaseNode* windowNode);
	bool DeleteWindow(const MkHashStr& windowName);

	//------------------------------------------------------------------------------------------------//
	// 활성화/비활성화
	// (NOTE) 지정은 다음 프레임때 반영 됨. Update()중 호출 될 수도 있기 때문
	//------------------------------------------------------------------------------------------------//

	// 현재 프레임의 활성화 여부 반환
	bool IsActivating(const MkHashStr& windowName) const;

	// 다음 프레임의 활성화 지정
	// modal 선언 가능. 단 이미 동작중인 modal window가 있을 경우 무시됨
	void ActivateWindow(const MkHashStr& windowName, bool modal = false);

	// 다음 프레임의 비활성화 지정
	void DeactivateWindow(const MkHashStr& windowName);

	//------------------------------------------------------------------------------------------------//
	// cursor pivot
	// 기본적으로 window system은 root와 input 좌표계가 동일하다고 간주하고 만들어지기 때문에 cursor의
	// world position을 그대로 사용 할 수 있지만, 만약 그렇지 않다면(서로 다른 좌표계라면) pivot을 수정해
	// cursor의 world position을 변환 해 주어야 함
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

	// targetNode 및 하위 모든 node들 중 ePA_SNA_AcceptInput 속성을 가진 node들을 대상으로 picking을 실행해 검출된 node/path를
	// 소유하고 있는 MkWindowBaseNode들을 반환
	// (NOTE) 규칙 및 argument 설명은 MkSceneNode::PickPanel() 참조
	bool _PickWindowBaseNode(const MkSceneNode* targetNode, MkArray<MkWindowBaseNode*>& buffer, const MkFloat2& worldPoint) const;
	bool _PickWindowBaseNode(const MkSceneNode* targetNode, MkArray<MkWindowBaseNode*>& nodeBuffer, MkArray< MkArray<MkHashStr> >& pathBuffer, const MkFloat2& worldPoint) const;

	void _SendCursorDraggingEvent(MkArray< MkArray<MkHashStr> >& pathBuffer, int type); // type : 0(begin), 1(end)

	void _CheckAndRegisterWindowNode(const MkHashStr& name);

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

	bool m_UpdateLock;

	// 대상 영역 지정
	MkInt2 m_TargetRegion;

	// scene portal node 하위 manager인지 여부
	bool m_ScenePortalBind;

	// scene portal node 하위 manager이면 갱신시 input 처리 여부
	bool m_ValidInputAtThisFrame;

	// 다음 프레임에 반영 될 활성화/비활성화 event
	MkArray<_ActivationEvent> m_ActivationEvent;

	// input
	MkInt2 m_InputPivotPosition;

	// 현 최상단 윈도우
	MkHashStr m_CurrentFocusWindow;

	// window path
	// 대상 노드(MkWindowBaseNode*)를 그대로 가지고 있으면 편리하기는 하지만 하위 node들의 attach, detach시마다
	// 모니터링을 통해 정보를 갱신해 주어야 함. 따라서 Update()시에만 사용하는 휘발성 참조이므로 path를 통해
	// 유효성을 갱신해 사용하기로 함
	MkArray< MkArray<MkHashStr> > m_CursorOwnedWindowPath;
	MkArray< MkArray<MkHashStr> > m_KeyInputTargetWindowPath;
	MkArray< MkArray<MkHashStr> > m_LeftCursorDraggingNodePath;

public:

	static const MkHashStr ModalEffectNodeName;

	static const MkHashStr ObjKey_DepthBandwidth;
	static const MkHashStr ObjKey_DeactivatingWindows;
	static const MkHashStr ObjKey_ActivatingWindows;
	static const MkHashStr ObjKey_ModalWindow;
};
