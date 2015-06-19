#pragma once


//------------------------------------------------------------------------------------------------//
// scene node
// 말 그대로 scene node
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingleTypeTreePattern.h"
#include "MkCore_MkPairArray.h"

#include "MkPA_MkPanel.h"


class MkDataNode;

class MkSceneNode : public MkSingleTypeTreePattern<MkSceneNode>, public MkSceneObject
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_SceneNode; }

	// hierarchy
	bool IsDerivedFrom(ePA_SceneNodeType nodeType) const;
	bool IsDerivedFrom(const MkSceneNode* instance) const;

	// 자신 혹은 조상들 중에서 nodeType 파생을 만족시키는 가장 가까운 객체를 반환
	MkSceneNode* FindNearestDerivedNode(ePA_SceneNodeType nodeType);

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
	inline const MkPanel* GetPanel(const MkHashStr& name) const { return m_Panels.Exist(name) ? &m_Panels[name] : NULL; }

	// panel 이름 리스트 반환
	inline unsigned int GetPanelNameList(MkArray<MkHashStr>& buffer) const { return m_Panels.GetKeyList(buffer); }

	// panel 삭제
	inline void DeletePanel(const MkHashStr& name) { if (m_Panels.Exist(name)) { m_Panels.Erase(name); } }

	// 모든 panel 삭제
	inline void DeleteAllPanels(void) { m_Panels.Clear(); }

	// 자신과 모든 하위 노드 중 visible이고 attrCondition을 만족하는(CheckInclusion) 노드들을 대상으로,
	// startDepth이상의 거리에서 startDepth와 가장 가깝고 worldPoint와 충돌하는 MkPanel을 buffer에 넣어 반환
	// (NOTE) 대상 노드(자신)는 최소한 한 번 이상 Update()를 통해 world transform이 갱신된 상태이어야 함
	bool PickPanel(MkArray<MkPanel*>& buffer, const MkFloat2& worldPoint, float startDepth = 0.f, const MkBitField32& attrCondition = MkBitField32::EMPTY) const;

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// visible. default는 true
	inline void SetVisible(bool visible) { m_Attribute.Assign(ePA_SNA_Visible, visible); }
	inline bool GetVisible(void) const { return m_Attribute[ePA_SNA_Visible]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	// 상위에서 하위로 내려가는 이벤트(root -> leaf)
	virtual void SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument);

	// 하위에서 상위로 올라오는 이벤트(leaf -> root)
	// path에는 호출이 시작된 node name부터 호출 순서의 역순으로 담겨 있음(상위에서 해당 node를 찾으려면 순방향으로 참조)
	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	// reporting 시작
	void StartNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	// 자신과 모든 하위 노드 정보 갱신
	virtual void Update(double currTime = 0.);

	// 해제
	virtual void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Load(const MkDataNode& node);
	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	//------------------------------------------------------------------------------------------------//

	MkSceneNode(const MkHashStr& name);
	virtual ~MkSceneNode() { Clear(); }

public:

	const MkSceneTransform* __GetTransformPtr(void) const { return &m_Transform; }

	void __GetAllValidPanels(const MkFloatRect& cameraAABR, MkPairArray<float, const MkPanel*>& buffer) const;

protected:

	static void _AddExceptionList(MkDataNode& node, const MkHashStr& expKey, const MkArray<MkHashStr>& expList);

protected:

	// flag
	MkBitField32 m_Attribute;

	// transform
	MkSceneTransform m_Transform;

	// axis aligned bounding box
	MkFloatRect m_PanelAABR; // 직계 panel들만 대상
	MkFloatRect m_TotalAABR; // m_PanelAABR + 하위 모든 자식 노드들의 AABR 합산

	// sub panels
	MkHashMap<MkHashStr, MkPanel> m_Panels; // name, panel

public:

	static const MkHashStr ArgKey_DragMovement;

	// save control 용 key
	static const MkHashStr SystemKey_PanelExceptions; // 해당 리스트를 panel 출력 대상에서 제외
	static const MkHashStr SystemKey_NodeExceptions; // 해당 리스트를 node 출력 대상에서 제외

	static const MkHashStr RootKey_Panels;
	static const MkHashStr RootKey_SubNodes;

	static const MkHashStr ObjKey_Attribute;
};

//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_SceneNodeDerivedInstanceOp
{
public:

	static DataType* Alloc(MkSceneNode* parentNode, const MkHashStr& childNodeName)
	{
		if ((parentNode != NULL) && parentNode->ChildExist(childNodeName))
			return NULL;

		DataType* node = new DataType(childNodeName);
		if ((node != NULL) && (parentNode != NULL))
		{
			parentNode->AttachChildNode(node);
		}
		return node;
	}
};

//------------------------------------------------------------------------------------------------//