#pragma once


//------------------------------------------------------------------------------------------------//
// scene node
// scene�� �����ϴ� �ּ� ���� ��ü
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingleTypeTreePattern.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkValueDecision.h"
#include "MkCore_MkPairArray.h"

#include "MkS2D_MkSRect.h"


class MkSceneNode : public MkSingleTypeTreePattern<MkSceneNode>
{
public:

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// MkDataNode�� ����. ���� �������� �����ϸ� ���
	// ���� ��ü �� �̸��� ��ġ�� �ʴ� �������� ��ü�� �״�� ����(transform, alpha �� �ڽĿ��� ������ �ִ� ��Ҵ� ���� ��)
	// ������ ������ ���ؼ��� Update() ȣ�� �ʿ�
	virtual void Load(const MkDataNode& node);

	// MkDataNode�� ���
	virtual void Save(MkDataNode& node);

	// window preset ����
	MkSceneNode* CreateWindowPreset(const MkHashStr& theme, eS2D_WindowPresetComponent component, const MkFloat2& bodySize);

	//------------------------------------------------------------------------------------------------//
	// transform
	//------------------------------------------------------------------------------------------------//

	// set & get local position
	inline void SetLocalPosition(const MkVec3& position) { m_LocalPosition = position; }
	inline const MkVec3& GetLocalPosition(void) const { return m_LocalPosition.GetBuffer(); }

	inline void SetLocalPosition(const MkFloat2& position) { SetLocalPosition(MkVec3(position.x, position.y, m_LocalPosition.GetBuffer().z)); }

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
	// SRect ����
	//------------------------------------------------------------------------------------------------//

	// SRect ����
	MkSRect* CreateSRect(const MkHashStr& name);
	MkSRect* CreateSRect(const MkHashStr& name, const MkBaseTexturePtr& texture, const MkHashStr& subsetName, const MkFloat2& position);

	// SRect ��ȯ
	MkSRect* GetSRect(const MkHashStr& name);

	// SRect ����
	bool DeleteSRect(const MkHashStr& name);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	// �ڽŰ� ��� ���� ��� ���� ����
	void Update(void);

	// Update() + WindowNode ���� ����
	void Update(const MkFloatRect& rootRegion);

	// ����
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

	//------------------------------------------------------------------------------------------------//
	// MkBaseWindowNode interface
	//------------------------------------------------------------------------------------------------//

	virtual void __UpdateWindow(const MkFloatRect& rootRegion);

protected:

	//------------------------------------------------------------------------------------------------//

	typedef struct __WindowPreset
	{
		MkHashStr theme;
		eS2D_WindowPresetComponent component;
		MkInt2 bodySize;
	}
	_WindowPreset;

	void _ApplyBuildingTemplateToSave(MkDataNode& node, const MkHashStr& templateName);

	bool _AddWindowTypeImageSet(const MkHashStr& imageSet, const MkFloat2& bodySize, MkSceneNode* targetNode);
	bool _AddWindowTypeTitleImageSet(const MkArray<MkHashStr>& imageSet, const MkFloat2& bodySize, MkSceneNode* targetNode);
	bool _AddWindowTypeStateImageSet(const MkArray<MkHashStr>& imageSet, const MkFloat2& bodySize, MkSceneNode* targetNode);

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

	MkFloatRect m_NodeOnlyAABR; // world transform�� ����� m_SRects�� ���
	MkFloatRect m_WorldAABR; // �ڽŰ� ���� ��� �ڽ� ������ m_NodeOnlyAABR �ջ�

	MkHashMap<MkHashStr, MkSRect> m_SRects; // name, SRect

	MkHashMap<MkHashStr, _WindowPreset> m_Presets; // preset name(MkWindowPreset::GetWindowPresetComponentKeyword()), preset data
};
