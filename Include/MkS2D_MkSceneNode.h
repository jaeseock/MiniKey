#pragma once


//------------------------------------------------------------------------------------------------//
// scene node
// scene을 구성하는 최소 단위 객체
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingleTypeTreePattern.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkValueDecision.h"
#include "MkCore_MkPairArray.h"

#include "MkS2D_MkSRect.h"


class MkSceneNode : public MkSingleTypeTreePattern<MkSceneNode>
{
public:

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_SceneNode; }

	//------------------------------------------------------------------------------------------------//
	// 구성
	//------------------------------------------------------------------------------------------------//

	// MkDataNode로 구성. 기존 설정값이 존재하면 덮어씀
	// 기존 객체 중 이름이 겹치지 않는 독자적인 객체는 그대로 놔둠(transform, alpha 등 자식에게 영향을 주는 요소는 적용 됨)
	// 완전한 적용을 위해서는 Update() 호출 필요
	virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// transform
	//------------------------------------------------------------------------------------------------//

	// set & get local position
	inline void SetLocalPosition(const MkVec3& position) { m_LocalPosition = position; }
	inline void SetLocalPosition(const MkFloat2& position) { SetLocalPosition(MkVec3(position.x, position.y, m_LocalPosition.GetBuffer().z)); }
	inline const MkVec3& GetLocalPosition(void) const { return m_LocalPosition.GetBuffer(); }

	void SetLocalAsWorldPosition(const MkFloat2& worldPosition, bool update = false);

	inline void SetLocalDepth(float depth) { SetLocalPosition(MkVec3(m_LocalPosition.GetBuffer().x, m_LocalPosition.GetBuffer().y, depth)); }
	inline float GetLocalDepth(void) const { return m_LocalPosition.GetBuffer().z; }
	
	// set & get local rotation
	void SetLocalRotation(float rotation);
	inline float GetLocalRotation(void) const { return m_LocalRotation.GetBuffer(); }

	// set & get local scale
	inline void SetLocalScale(float scale) { m_LocalScale = (scale < 0.f) ? 0.f : scale; }
	inline float GetLocalScale(void) const { return m_LocalScale.GetBuffer(); }

	// get world transform
	inline const MkVec3& GetWorldPosition(void) const { return m_WorldPosition.GetDecision(); }
	inline float GetWorldRotation(void) const { return m_WorldRotation.GetDecision(); }
	inline float GetWorldScale(void) const { return m_WorldScale.GetDecision(); }

	// get world AABR
	inline const MkFloatRect& GetWorldAABR(void) const { return m_WorldAABR; }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// set & get alpha
	void SetAlpha(float alpha);
	inline float GetAlpha(void) const { return m_Alpha.GetDecision(); }

	// set & get visible
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	//------------------------------------------------------------------------------------------------//
	// SRect 관리
	//------------------------------------------------------------------------------------------------//

	// SRect 생성
	MkSRect* CreateSRect(const MkHashStr& name);

	// SRect 존재 여부
	inline bool ExistSRect(const MkHashStr& name) const { return m_SRects.Exist(name); }

	// SRect 반환
	MkSRect* GetSRect(const MkHashStr& name);

	// SRect 이름 리스트 반환
	inline unsigned int GetSRectNameList(MkArray<MkHashStr>& buffer) const { return m_SRects.GetKeyList(buffer); }

	// SRect 삭제
	bool DeleteSRect(const MkHashStr& name);
	void DeleteAllSRects(void);

	// 기존 설정된 deco string이 있다면 재로딩
	void RestoreDecoString(void);

	//------------------------------------------------------------------------------------------------//
	// 정렬
	// (NOTE) 호출 전 Update()가 실행 된 상태여야 하며 호출 후 다시 Update()를 실행 할 필요 없음
	//------------------------------------------------------------------------------------------------//

	// anchorRect를 기준으로 정렬
	void AlignPosition(const MkFloatRect& anchorRect, eRectAlignmentPosition alignment, const MkInt2& border);

	// anchorNode의 world AABR 기준으로 정렬
	void AlignPosition(const MkSceneNode* anchorNode, eRectAlignmentPosition alignment, const MkInt2& border);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	// 자신과 모든 하위 노드 정보 갱신
	// (NOTE) 호출된 노드에서만 실행 됨
	void UpdateAll(void);

	// 자신 혹은 상위에서 UpdateAll() 호출시 자신과 모든 하위 노드에서 transform update 전 실행 됨
	virtual void UpdateNode(void) {}

	// 해제
	virtual void Clear(void);

	MkSceneNode(const MkHashStr& name);
	virtual ~MkSceneNode() { Clear(); }

public:

	//------------------------------------------------------------------------------------------------//

	static void __GenerateBuildingTemplate(void);

	inline const MkValueDecision<MkVec3>& __GetWorldPosition(void) const { return m_WorldPosition; }
	inline const MkValueDecision<float>& __GetWorldRotation(void) const { return m_WorldRotation; }
	inline const MkValueDecision<float>& __GetWorldScale(void) const { return m_WorldScale; }

	void __UpdateTransform(void);
	void __UpdateWorldAABR(void);

	void __GetAllValidSRects(const MkFloatRect& cameraAABR, MkPairArray<float, const MkSRect*>& buffer) const;

protected:

	//------------------------------------------------------------------------------------------------//

	void _ApplyBuildingTemplateToSave(MkDataNode& node, const MkHashStr& templateName);

	void _UpdateNodeTransform(const MkValueDecision<MkVec3>& worldPosition, const MkValueDecision<float>& worldRotation, const MkValueDecision<float>& worldScale);
	void _UpdateNodeTransform(void);

protected:

	//------------------------------------------------------------------------------------------------//

	MkValueDecision<MkVec3> m_LocalPosition;
	MkValueDecision<MkVec3> m_WorldPosition;

	MkValueDecision<float> m_LocalRotation;
	MkValueDecision<float> m_WorldRotation;

	MkValueDecision<float> m_LocalScale;
	MkValueDecision<float> m_WorldScale;

	MkValueDecision<float> m_Alpha;

	bool m_Visible;

	MkFloatRect m_NodeOnlyAABR; // world transform이 적용된 m_SRects가 대상
	MkFloatRect m_WorldAABR; // 자신과 하위 모든 자식 노드들의 m_NodeOnlyAABR 합산

	MkHashMap<MkHashStr, MkSRect> m_SRects; // name, SRect
};
