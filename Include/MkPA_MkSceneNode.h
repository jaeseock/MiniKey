#pragma once


//------------------------------------------------------------------------------------------------//
// scene node
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingleTypeTreePattern.h"
#include "MkCore_MkPairArray.h"

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

	// world
	inline const MkFloat2& GetWorldPosition(void) const { return m_Transform.GetWorldPosition(); }
	inline float GetWorldDepth(void) const { return m_Transform.GetWorldDepth(); }
	inline float GetWorldRotation(void) const { return m_Transform.GetWorldRotation(); }
	inline float GetWorldScale(void) const { return m_Transform.GetWorldScale(); }
	inline float GetWorldAlpha(void) const { return m_Transform.GetWorldAlpha(); }

	// identify
	inline void IdentifyTransform(void) { m_Transform.Clear(); }

	// total AABR
	inline const MkFloatRect& GetTotalAABR(void) const { return m_TotalAABR; }

	//------------------------------------------------------------------------------------------------//
	// panel 관리
	//------------------------------------------------------------------------------------------------//

	// panel 존재 여부
	inline bool PanelExist(const MkHashStr& name) const { return m_Panels.Exist(name); }

	// panel 반환. 없으면 생성해 반환
	inline MkPanel* GetPanel(const MkHashStr& name) { return m_Panels.Exist(name) ? &m_Panels[name] : &m_Panels.Create(name); }

	// panel 이름 리스트 반환
	inline unsigned int GetPanelNameList(MkArray<MkHashStr>& buffer) const { return m_Panels.GetKeyList(buffer); }

	// panel 삭제
	inline void DeletePanel(const MkHashStr& name) { if (m_Panels.Exist(name)) { m_Panels.Erase(name); } }
	inline void DeleteAllPanels(void) { m_Panels.Clear(); }

	// 기존 설정된 deco string이 있다면 재로딩
	//void RestoreDecoString(void);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// visible
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

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

	//void _ApplyBuildingTemplateToSave(MkDataNode& node, const MkHashStr& templateName);

protected:

	//------------------------------------------------------------------------------------------------//

	// transform
	MkSceneTransform m_Transform;

	MkFloatRect m_PanelAABR; // 직계 panel들만 대상
	MkFloatRect m_TotalAABR; // m_PanelAABR + 하위 모든 자식 노드들의 AABR 합산

	MkHashMap<MkHashStr, MkPanel> m_Panels; // name, panel

	bool m_Visible;
};
