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

	// MkDataNode�� ����. ���� �������� �����ϸ� ���
	// ���� ��ü �� �̸��� ��ġ�� �ʴ� �������� ��ü�� �״�� ����(transform, alpha �� �ڽĿ��� ������ �ִ� ��Ҵ� ���� ��)
	// ������ ������ ���ؼ��� Update() ȣ�� �ʿ�
	//virtual void Load(const MkDataNode& node);

	// MkDataNode�� ���
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
	// panel ����
	//------------------------------------------------------------------------------------------------//

	// panel ���� ����
	inline bool PanelExist(const MkHashStr& name) const { return m_Panels.Exist(name); }

	// panel ��ȯ. ������ ������ ��ȯ
	inline MkPanel* GetPanel(const MkHashStr& name) { return m_Panels.Exist(name) ? &m_Panels[name] : &m_Panels.Create(name); }

	// panel �̸� ����Ʈ ��ȯ
	inline unsigned int GetPanelNameList(MkArray<MkHashStr>& buffer) const { return m_Panels.GetKeyList(buffer); }

	// panel ����
	inline void DeletePanel(const MkHashStr& name) { if (m_Panels.Exist(name)) { m_Panels.Erase(name); } }
	inline void DeleteAllPanels(void) { m_Panels.Clear(); }

	// ���� ������ deco string�� �ִٸ� ��ε�
	//void RestoreDecoString(void);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// visible
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	//------------------------------------------------------------------------------------------------//
	// ����
	// (NOTE) ȣ�� �� Update()�� ���� �� ���¿��� �ϸ� ȣ�� �� �ٽ� Update()�� ���� �� �ʿ� ����
	//------------------------------------------------------------------------------------------------//

	// anchorRect�� �������� ����
	//void AlignPosition(const MkFloatRect& anchorRect, eRectAlignmentPosition alignment, const MkInt2& border);

	// anchorNode�� world AABR �������� ����
	//void AlignPosition(const MkSceneNode* anchorNode, eRectAlignmentPosition alignment, const MkInt2& border);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	// �ڽŰ� ��� ���� ��� ���� ����
	void Update(double currTime = 0.);

	// ����
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

	MkFloatRect m_PanelAABR; // ���� panel�鸸 ���
	MkFloatRect m_TotalAABR; // m_PanelAABR + ���� ��� �ڽ� ������ AABR �ջ�

	MkHashMap<MkHashStr, MkPanel> m_Panels; // name, panel

	bool m_Visible;
};
