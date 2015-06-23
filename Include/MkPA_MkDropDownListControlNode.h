#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : drop down list
// 소수의 item list 선택에 적합
//
// ex>
//	MkDropDownListControlNode* ddList = MkDropDownListControlNode::CreateChildNode(bodyFrame, L"DropDown");
//	ddList->SetDropDownList(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 200.f, 6);
//	ddList->AddItem(L"0", L"아도~ 겐!");
//	ddList->AddItem(L"1", L"아따따따류~ 겐!");
//	ddList->AddItem(L"2", L"꺕꺕꺕꺕꺕, 꺕꺕!");
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
	// MkListBoxcontrolNode의 일부만 wrapping
	//------------------------------------------------------------------------------------------------//

	// item 추가. 동일 key의 item이 존재하면 삭제 후 추가
	void AddItem(const MkHashStr& uniqueKey, const MkStr& message);

	// item 삭제
	bool RemoveItem(const MkHashStr& uniqueKey);

	// 모든 item 삭제
	void ClearAllItems(void);

	// 모든 item의 갯수 반환
	unsigned int GetItemCount(void) const;

	// 모든 item의 key 반환
	unsigned int GetItemKeyList(MkArray<MkHashStr>& keyList) const;

	// 대상 item 지정/반환
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

	// 해제
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
