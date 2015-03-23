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

	inline void SetDepthBandwidth(float depth) { m_DepthBandwidth = depth; }
	inline float GetDepthBandwidth(void) const { return m_DepthBandwidth; }

	//------------------------------------------------------------------------------------------------//
	// window 등록/삭제
	// (NOTE) Update()중 호출 금지
	//------------------------------------------------------------------------------------------------//

	enum eLayerType
	{
		eLT_Low = 0, // 하위 layer. 가장 밑단에 위치
		eLT_Normal, // 일반 layer
		eLT_High // 상위 layer. 가장 윗단에 위치
	};

	bool AttachWindow(MkWindowBaseNode* windowNode, eLayerType layerType = eLT_Normal);
	bool DeleteWindow(const MkHashStr& windowName);

	//------------------------------------------------------------------------------------------------//
	// 활성화/비활성화
	// (NOTE) 지정은 다음 프레임때 반영 됨. Update()중 호출 될 수도 있기 때문
	//------------------------------------------------------------------------------------------------//

	// 현재 프레임의 활성화 여부 반환
	bool IsActivating(const MkHashStr& windowName) const;

	// 다음 프레임의 활성화 지정. modal 선언 가능
	void ActivateWindow(const MkHashStr& windowName, bool modal = false);

	// 다음 프레임의 비활성화 지정
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

	// 자신과 하위 모든 node들 중 ePA_SNA_AcceptInput 속성을 가진 node들을 대상으로 picking을 실행해 검출된 node/path를
	// 소유하고 있는 MkWindowBaseNode들을 반환
	// (NOTE) 규칙 및 argument 설명은 MkSceneNode::PickPanel() 참조
	bool _PickWindowBaseNode(MkArray<MkWindowBaseNode*>& buffer, const MkFloat2& worldPoint) const;
	bool _PickWindowBaseNode(MkArray<MkWindowBaseNode*>& nodeBuffer, MkArray< MkArray<MkHashStr> >& pathBuffer, const MkFloat2& worldPoint) const;

protected:

	//------------------------------------------------------------------------------------------------//
	// 설정 정보
	//------------------------------------------------------------------------------------------------//

	float m_DepthBandwidth;
	
	// 비활성화 윈도우 목록
	MkArray<MkHashStr> m_DeactivatingWindows;

	// 활성화 윈도우 목록
	MkMap<eLayerType, MkArray<MkHashStr> > m_ActivatingWindows;
	MkHashStr m_ModalWindow;

	// 등록된 root window list
	MkHashMap<MkHashStr, _RootWindowInfo> m_RootWindowList;

	//------------------------------------------------------------------------------------------------//
	// 휘발성 정보
	//------------------------------------------------------------------------------------------------//

	bool m_UpdateLock;

	// 다음 프레임에 반영 될 활성화/비활성화 event
	MkArray<_ActivationEvent> m_ActivationEvent;

	// window path
	// 대상 노드(MkWindowBaseNode*)를 그대로 가지고 있으면 편리하기는 하지만 하위 node들의 attach, detach시마다
	// 모니터링을 통해 정보를 갱신해 주어야 함. 따라서 Update()시에만 사용하는 휘발성 참조이므로 path를 통해
	// 유효성을 갱신해 사용하기로 함
	MkArray< MkArray<MkHashStr> > m_CursorOwnedWindowPath;
	MkArray< MkArray<MkHashStr> > m_KeyInputTargetWindowPath;
	MkArray< MkArray<MkHashStr> > m_LeftCursorDraggingNodePath;

public:

	//static const MkStr NamePrefix;
};
