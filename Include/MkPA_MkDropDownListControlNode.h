#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : drop down list
// �Ҽ��� item list ���ÿ� ����
//
// ex>
//	MkDropDownListControlNode* ddList = MkDropDownListControlNode::CreateChildNode(bodyFrame, L"DropDown");
//	ddList->SetDropDownList(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 6);
//	ddList->AddItem(L"0", L"�Ƶ�~ ��!");
//	ddList->AddItem(L"1", L"�Ƶ�������~ ��!");
//	ddList->AddItem(L"2", L"����������, ����!");
//	ddList->SetTargetItemKey(L"1");
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkWindowTagNode;
class MkListBoxControlNode;

class MkDropDownListControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_DropDownListControlNode; }

	// alloc child instance
	static MkDropDownListControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// control interface
	//------------------------------------------------------------------------------------------------//

	void SetDropDownList(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int onePageItemSize);

	//------------------------------------------------------------------------------------------------//
	// item management
	// MkListBoxcontrolNode�� �Ϻθ� wrapping
	//------------------------------------------------------------------------------------------------//

	// item �߰�. ���� key�� item�� �����ϸ� ���� �� �߰�
	void AddItem(const MkHashStr& uniqueKey, const MkStr& message);

	// item ����
	bool RemoveItem(const MkHashStr& uniqueKey);

	// ��� item ����
	void ClearAllItems(void);

	// ��� item�� ���� ��ȯ
	unsigned int GetItemCount(void) const;

	// ��� item�� key ��ȯ
	unsigned int GetItemKeyList(MkArray<MkHashStr>& keyList) const;

	// ��� item ����/��ȯ
	void SetTargetItemKey(const MkHashStr& uniqueKey);
	inline const MkHashStr& GetTargetItemKey(void) const { return m_TargetItemKey; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	virtual void LoadComplete(const MkDataNode& node);

	// ����
	virtual void Clear(void);

	MkDropDownListControlNode(const MkHashStr& name);
	virtual ~MkDropDownListControlNode() {}

protected:

	void _BuildSelectionAndDropDownButton(void);

	void _SetTextToSelectionTag(const MkWindowTagNode* tagNode);

protected:

	int m_MaxOnePageItemSize;
	MkListBoxControlNode* m_ListBoxControlNode;

	MkHashStr m_TargetItemKey;

public:

	static const MkHashStr SelectionNodeName;
	static const MkHashStr DropDownBtnNodeName;
	static const MkHashStr ListBoxNodeName;

	static const MkHashStr ObjKey_MaxOnePageItemSize;
	static const MkHashStr ObjKey_TargetItemKey;
};
