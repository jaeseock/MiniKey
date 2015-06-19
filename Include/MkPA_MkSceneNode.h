#pragma once


//------------------------------------------------------------------------------------------------//
// scene node
// �� �״�� scene node
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

	// �ڽ� Ȥ�� ����� �߿��� nodeType �Ļ��� ������Ű�� ���� ����� ��ü�� ��ȯ
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
	// panel ����
	// - single panel : �Ϲ����� panel
	// - masking panel : ��� ��带 Ư�� ������ŭ �߶� �׷��ִ� panel
	//------------------------------------------------------------------------------------------------//

	// panel ���� ����
	inline bool PanelExist(const MkHashStr& name) const { return m_Panels.Exist(name); }

	// single panel ����. ���� �̸��� panel�� �̹� �����ϸ� ���� �� �����
	MkPanel& CreatePanel(const MkHashStr& name);

	// targetNode�� masking�ϴ� panel ����. ���� �̸��� panel�� �̹� �����ϸ� ���� �� �����
	// (NOTE) targetNode�� �� ��尡 ���� �������� ���� ���� �������� ����̾�� ��
	//        ���� ���� �������� ���� ���� ��� transform�� ���̰ų� ���� ������ ���� �� ����
	// (NOTE) panel size�� �ݵ�� ��ȿ�� ũ���̾�� ��
	MkPanel& CreatePanel(const MkHashStr& name, const MkSceneNode* targetNode, const MkInt2& panelSize);

	// panel ��ȯ. ������ NULL ��ȯ
	inline MkPanel* GetPanel(const MkHashStr& name) { return m_Panels.Exist(name) ? &m_Panels[name] : NULL; }
	inline const MkPanel* GetPanel(const MkHashStr& name) const { return m_Panels.Exist(name) ? &m_Panels[name] : NULL; }

	// panel �̸� ����Ʈ ��ȯ
	inline unsigned int GetPanelNameList(MkArray<MkHashStr>& buffer) const { return m_Panels.GetKeyList(buffer); }

	// panel ����
	inline void DeletePanel(const MkHashStr& name) { if (m_Panels.Exist(name)) { m_Panels.Erase(name); } }

	// ��� panel ����
	inline void DeleteAllPanels(void) { m_Panels.Clear(); }

	// �ڽŰ� ��� ���� ��� �� visible�̰� attrCondition�� �����ϴ�(CheckInclusion) ������ �������,
	// startDepth�̻��� �Ÿ����� startDepth�� ���� ������ worldPoint�� �浹�ϴ� MkPanel�� buffer�� �־� ��ȯ
	// (NOTE) ��� ���(�ڽ�)�� �ּ��� �� �� �̻� Update()�� ���� world transform�� ���ŵ� �����̾�� ��
	bool PickPanel(MkArray<MkPanel*>& buffer, const MkFloat2& worldPoint, float startDepth = 0.f, const MkBitField32& attrCondition = MkBitField32::EMPTY) const;

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	// visible. default�� true
	inline void SetVisible(bool visible) { m_Attribute.Assign(ePA_SNA_Visible, visible); }
	inline bool GetVisible(void) const { return m_Attribute[ePA_SNA_Visible]; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	// �������� ������ �������� �̺�Ʈ(root -> leaf)
	virtual void SendNodeCommandTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument);

	// �������� ������ �ö���� �̺�Ʈ(leaf -> root)
	// path���� ȣ���� ���۵� node name���� ȣ�� ������ �������� ��� ����(�������� �ش� node�� ã������ ���������� ����)
	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	// reporting ����
	void StartNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	// �ڽŰ� ��� ���� ��� ���� ����
	virtual void Update(double currTime = 0.);

	// ����
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
	MkFloatRect m_PanelAABR; // ���� panel�鸸 ���
	MkFloatRect m_TotalAABR; // m_PanelAABR + ���� ��� �ڽ� ������ AABR �ջ�

	// sub panels
	MkHashMap<MkHashStr, MkPanel> m_Panels; // name, panel

public:

	static const MkHashStr ArgKey_DragMovement;

	// save control �� key
	static const MkHashStr SystemKey_PanelExceptions; // �ش� ����Ʈ�� panel ��� ��󿡼� ����
	static const MkHashStr SystemKey_NodeExceptions; // �ش� ����Ʈ�� node ��� ��󿡼� ����

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