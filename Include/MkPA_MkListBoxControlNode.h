#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : list box
//
// ex>
//	MkListBoxControlNode* lbNode = MkListBoxControlNode::CreateChildNode(bodyFrame, L"ListBox");
//	lbNode->SetListBox(MkWindowThemeData::DefaultThemeName, 6, 200.f, MkWindowThemeData::eFT_Small);
//	lbNode->SetAlignmentPosition(eRAP_LeftTop);
//	lbNode->SetAlignmentOffset(MkFloat2(10.f, -30.f));
//	lbNode->SetLocalDepth(-1.f);
//	lbNode->AddItem(L"가나다", L"가나다");
//	lbNode->AddItem(L"aaa", L"aaa");
//	lbNode->AddItem(L"bbb", L"bbb");
//	lbNode->SortItemSequenceInAscendingOrder();
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkWindowTagNode;

class MkListBoxControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_ListBoxControlNode; }

	// alloc child instance
	static MkListBoxControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// list box interface
	//------------------------------------------------------------------------------------------------//

	void SetListBox(const MkHashStr& themeName, int onePageItemSize, float itemWidth, MkWindowThemeData::eFrameType frameType);

	void SetOnePageItemSize(int onePageItemSize);

	inline int GetOnePageItemSize(void) const { return m_OnePageItemSize; }

	//------------------------------------------------------------------------------------------------//
	// item management
	//------------------------------------------------------------------------------------------------//

	// item 추가. 동일 key의 item이 존재하면 삭제 후 추가
	MkWindowTagNode* AddItem(const MkHashStr& uniqueKey, const MkStr& message);

	// item 삭제
	bool RemoveItem(const MkHashStr& uniqueKey);

	// 모든 item 삭제
	void ClearAllItems(void);

	// 해당 item의 tag node 반환
	MkWindowTagNode* GetItemTag(const MkHashStr& uniqueKey) const;

	// 모든 item의 갯수 반환
	inline unsigned int GetItemCount(void) const { return m_ItemData.GetSize(); }

	// 모든 item의 key 반환
	inline unsigned int GetItemKeyList(MkArray<MkHashStr>& keyList) const { return m_ItemData.GetKeyList(keyList); }

	// 표시 할 item 순서 설정/반환
	// (NOTE) keyList안의 key는 이미 존재하고 있는 item이어야 함
	// (NOTE) 반대로 이미 존재하고 있는 item이지만 keyList에는 포함되지 않을 수 있음(filter로도 활용 가능)
	void SetItemSequence(const MkArray<MkHashStr>& keyList);
	const MkArray<MkHashStr>& GetItemSequence(void) const { return m_ItemSequence; }

	// 현재 sequence를 key 기준 정렬
	void SortItemSequenceInAscendingOrder(void); // 오름차순 (ex> 1, 2, 3, ...)
	void SortItemSequenceInDescendingOrder(void); // 내림차순 (ex> 10, 9, 8, ...)

	// 모든 item을 대상으로 key prefix filtering
	// prefix가 MkStr::EMPTY면 모든 item을 대상 지정
	// (NOTE) 기존 sequence는 소멸됨
	// (NOTE) 결과는 정렬되지 않은 상태이므로 추가 정렬을 원할 경우 별도의 정렬 필요
	void UpdateItemSequenceByKeyPrefix(const MkStr& prefix);

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	virtual void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	MkListBoxControlNode(const MkHashStr& name);
	virtual ~MkListBoxControlNode() { Clear(); }

protected:

	//------------------------------------------------------------------------------------------------//
	// update command
	//------------------------------------------------------------------------------------------------//

	enum eListBoxControlNodeUpdateCommand
	{
		eUC_ItemList = eUC_WindowThemedNodeBandwidth,
		eUC_ItemButton,

		eUC_ListBoxControlNodeBandwidth
	};

	virtual void _ExcuteUpdateCommand(void);

protected:

	typedef struct __BtnData
	{
		MkHashStr btnName;
		MkHashStr linkedItemName;
	}
	_BtnData;

	typedef struct __ItemData
	{
		MkWindowTagNode* tagNode;
		int linkedBtnIndex;
	}
	_ItemData;

	void _UpdateItemList(void);
	void _UpdateItemButton(void);

	void _SetOnePageItemSize(int onePageItemSize);

	void _DetachItemTag(_BtnData& btnData);
	void _DetachItemTag(_ItemData& itemData);
	void _DetachItemTag(_BtnData& btnData, _ItemData& itemData);

	void _AttachItemTag(int btnIndex, const MkHashStr& uniqueKey);

	int _GetItemButtonIndex(const MkHashStr& name) const;

public:

	int m_OnePageItemSize;
	float m_ItemWidth;
	
	// button pool data
	MkArray<_BtnData> m_ButtonPoolData;

	// item data
	MkHashMap<MkHashStr, _ItemData> m_ItemData;

	// item sequence
	MkArray<MkHashStr> m_ItemSequence;

	int m_CurrentItemPosition;

public:

	static const MkHashStr ItemTagNodeName;
	static const MkHashStr VScrollBarName;

	static const MkHashStr ArgKey_Item;

	static const MkHashStr ObjKey_OnePageItemSize;
	static const MkHashStr ObjKey_ItemWidth;
	static const MkHashStr ObjKey_ItemKeyList;
	static const MkHashStr ObjKey_ItemMsgList;
	static const MkHashStr ObjKey_ItemSequence;
};
