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
//	lbNode->AddItem(L"������", L"������");
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

	// item �߰�. ���� key�� item�� �����ϸ� ���� �� �߰�
	MkWindowTagNode* AddItem(const MkHashStr& uniqueKey, const MkStr& message);

	// item ����
	bool RemoveItem(const MkHashStr& uniqueKey);

	// ��� item ����
	void ClearAllItems(void);

	// �ش� item�� tag node ��ȯ
	MkWindowTagNode* GetItemTag(const MkHashStr& uniqueKey) const;

	// ��� item�� ���� ��ȯ
	inline unsigned int GetItemCount(void) const { return m_ItemData.GetSize(); }

	// ��� item�� key ��ȯ
	inline unsigned int GetItemKeyList(MkArray<MkHashStr>& keyList) const { return m_ItemData.GetKeyList(keyList); }

	// ǥ�� �� item ���� ����/��ȯ
	// (NOTE) keyList���� key�� �̹� �����ϰ� �ִ� item�̾�� ��
	// (NOTE) �ݴ�� �̹� �����ϰ� �ִ� item������ keyList���� ���Ե��� ���� �� ����(filter�ε� Ȱ�� ����)
	void SetItemSequence(const MkArray<MkHashStr>& keyList);
	const MkArray<MkHashStr>& GetItemSequence(void) const { return m_ItemSequence; }

	// ���� sequence�� key ���� ����
	void SortItemSequenceInAscendingOrder(void); // �������� (ex> 1, 2, 3, ...)
	void SortItemSequenceInDescendingOrder(void); // �������� (ex> 10, 9, 8, ...)

	// ��� item�� ������� key prefix filtering
	// prefix�� MkStr::EMPTY�� ��� item�� ��� ����
	// (NOTE) ���� sequence�� �Ҹ��
	// (NOTE) ����� ���ĵ��� ���� �����̹Ƿ� �߰� ������ ���� ��� ������ ���� �ʿ�
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
