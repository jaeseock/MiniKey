#pragma once


//------------------------------------------------------------------------------------------------//
// scene node
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingleTypeTreePattern.h"
#include "MkCore_MkPairArray.h"
#include "MkCore_MkEventQueuePattern.h"

#include "MkPA_MkPanel.h"


class MkSceneNode : public MkSingleTypeTreePattern<MkSceneNode>
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_SceneNode; }

	// MkDataNode로 구성. 기존 설정값이 존재하면 덮어씀
	// 기존 객체 중 이름이 겹치지 않는 독자적인 객체는 그대로 놔둠(transform, alpha 등 자식에게 영향을 주는 요소는 적용 됨)
	// 완전한 적용을 위해서는 Update() 호출 필요
	//virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	//virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// transform
	//------------------------------------------------------------------------------------------------//

	// local
	inline void SetLocalPosition(const MkFloat2& position) { m_Transform.SetLocalPosition(position); }
	inline const MkFloat2& GetLocalPosition(void) const { return m_Transform.GetLocalPosition(); }

	inline void SetLocalDepth(float depth) { m_Transform.SetLocalDepth(depth); }
	inline float GetLocalDepth(void) const { return m_Transform.GetLocalDepth(); }

	inline void SetLocalRotation(float rotation) { m_Transform.SetLocalRotation(rotation); }
	inline float GetLocalRotation(void) const { return m_Transform.GetLocalRotation(); }

	inline void SetLocalScale(float scale) { m_Transform.SetLocalScale(scale); }
	inline float GetLocalScale(void) const { return m_Transform.GetLocalScale(); }

	inline void SetLocalAlpha(float alpha) { m_Transform.SetLocalAlpha(alpha); }
	inline float GetLocalAlpha(void) const { return m_Transform.GetLocalAlpha(); }

	// clear local
	inline void ClearLocalTransform(void) { m_Transform.ClearLocalTransform(); }

	// world
	inline const MkFloat2& GetWorldPosition(void) const { return m_Transform.GetWorldPosition(); }
	inline float GetWorldDepth(void) const { return m_Transform.GetWorldDepth(); }
	inline float GetWorldRotation(void) const { return m_Transform.GetWorldRotation(); }
	inline float GetWorldScale(void) const { return m_Transform.GetWorldScale(); }
	inline float GetWorldAlpha(void) const { return m_Transform.GetWorldAlpha(); }

	// get AABR
	inline const MkFloatRect& GetTotalAABR(void) const { return m_TotalAABR; }

	//------------------------------------------------------------------------------------------------//
	// panel 관리
	// - single panel : 일반적인 panel
	// - masking panel : 대상 노드를 특정 영역만큼 잘라 그려주는 panel
	//------------------------------------------------------------------------------------------------//

	// panel 존재 여부
	inline bool PanelExist(const MkHashStr& name) const { return m_Panels.Exist(name); }

	// single panel 생성. 같은 이름의 panel이 이미 존재하면 삭제 후 재생성
	MkPanel& CreatePanel(const MkHashStr& name);

	// targetNode를 masking하는 panel 생성. 같은 이름의 panel이 이미 존재하면 삭제 후 재생성
	// (NOTE) targetNode는 현 노드가 속한 계층군과 관계 없는 독립적인 노드이어야 함
	//        만약 같은 계층군에 속해 있을 경우 transform이 꼬이거나 무한 루프에 빠질 수 있음
	// (NOTE) panel size는 반드시 유효한 크기이어야 함
	MkPanel& CreatePanel(const MkHashStr& name, const MkSceneNode* targetNode, const MkInt2& panelSize);

	// panel 반환. 없으면 NULL 반환
	inline MkPanel* GetPanel(const MkHashStr& name) { return m_Panels.Exist(name) ? &m_Panels[name] : NULL; }

	// panel 이름 리스트 반환
	inline unsigned int GetPanelNameList(MkArray<MkHashStr>& buffer) const { return m_Panels.GetKeyList(buffer); }

	// panel 삭제
	inline void DeletePanel(const MkHashStr& name) { if (m_Panels.Exist(name)) { m_Panels.Erase(name); } }

	// 모든 panel 삭제
	inline void DeleteAllPanels(void) { m_Panels.Clear(); }

	// 자신과 모든 하위 노드 중 attrCondition을 만족하는(CheckInclusion) 노드들을 대상으로,
	// startDepth이상의 거리에서 startDepth와 가장 가깝고 worldPoint와 충돌하는 MkPanel을 buffer에 넣어 반환
	// (NOTE) 대상 노드(자신)는 최소한 한 번 이상 Update()를 통해 world transform이 갱신된 상태이어야 함
	bool PickPanel(MkArray<MkPanel*>& buffer, const MkFloat2& worldPoint, float startDepth = 0.f, const MkBitField32& attrCondition = MkBitField32::EMPTY) const;
	
	// 기존 설정된 deco string이 있다면 재로딩
	//void RestoreDecoString(void);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	enum eSceneNodeAttribute
	{
		eAT_Visible = 0,

		eAT_SceneNodeBandwidth = 4 // 4bit 대역폭 확보
	};

	// visible. default는 true
	inline void SetVisible(bool visible) { m_Attribute.Assign(eAT_Visible, visible); }
	inline bool GetVisible(void) const { return m_Attribute[eAT_Visible]; }

	//------------------------------------------------------------------------------------------------//
	// 정렬
	// (NOTE) 호출 전 Update()가 실행 된 상태여야 하며 호출 후 다시 Update()를 실행 할 필요 없음
	//------------------------------------------------------------------------------------------------//

	// anchorRect를 기준으로 정렬
	//void AlignPosition(const MkFloatRect& anchorRect, eRectAlignmentPosition alignment, const MkInt2& border);

	// anchorNode의 world AABR 기준으로 정렬
	//void AlignPosition(const MkSceneNode* anchorNode, eRectAlignmentPosition alignment, const MkInt2& border);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	// 자신과 모든 하위 노드 정보 갱신
	void Update(double currTime = 0.);

	// 해제
	virtual void Clear(void);

	MkSceneNode(const MkHashStr& name);
	virtual ~MkSceneNode() { Clear(); }

public:

	//------------------------------------------------------------------------------------------------//

	//static void __GenerateBuildingTemplate(void);

	const MkSceneTransform* __GetTransformPtr(void) const { return &m_Transform; }

	void __GetAllValidPanels(const MkFloatRect& cameraAABR, MkPairArray<float, const MkPanel*>& buffer) const;

protected:

	//------------------------------------------------------------------------------------------------//
	// 하위 node(scene node 계열)와의 event pushing을 위한 interface
	// scene node는 가장 상위의 class이기 때문에 별도의 event type이 필요하지는 않음
	//------------------------------------------------------------------------------------------------//
	typedef MkEventUnitPack1<int, MkHashStr> _NodeEvent;

public:
	virtual void __SendNodeEvent(const _NodeEvent& evt);

protected:

	//------------------------------------------------------------------------------------------------//

	//void _ApplyBuildingTemplateToSave(MkDataNode& node, const MkHashStr& templateName);

protected:

	//------------------------------------------------------------------------------------------------//

	MkSceneTransform m_Transform;

	MkFloatRect m_PanelAABR; // 직계 panel들만 대상
	MkFloatRect m_TotalAABR; // m_PanelAABR + 하위 모든 자식 노드들의 AABR 합산

	MkHashMap<MkHashStr, MkPanel> m_Panels; // name, panel

	// flag
	MkBitField32 m_Attribute;
};
