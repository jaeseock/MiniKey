#pragma once


//------------------------------------------------------------------------------------------------//
// spread button node : MkBaseWindowNode
//
// ���� ��Ģ
// 0. ���� �ε��� ���� �ִ� �����̾�� ��
// 1. ��Ʈ ��ư���κ��� ���۵Ǵ� tree�� �ϳ��� �׷��̶� �� ��, ���� �ϳ��� �׷츸 ���� ���¸� �����ؾ� ��
// 2. �θ� ��ư�� �������� �����̾�� �ڽ� ��ư�� ���� �� �� ����
// 3. �׷쿡 ���� ��ư�� ����(unique key)�� �ش� �׷쿡�� �����ؾ� ��
// 4. ���� ��ư���� ���ÿ� ���� ���� �� ����
// 5. ��Ʈ ��ư�� �θ� ������ ���� �ȿ� �����ϰ� ���� ��ư�� ��Ʈ ��ư�� ũ�⸦ ����
//   (�׷��� ���� ���� ��� ��ư�� ������ �� �浹 üũ�� ������ ����)
//
// ���� ������ �����ѵ�, �Ϲ������� ���� ������� ������ ���Ե����� spread button�� ��� �����Ǵٺ���
// ���� �������� ������ �Ѿ�Ⱑ ����. �� ��� ��ǲ �̺�Ʈ �� ���� ������ ������ ����Ƿ� ������
// �����Ͽ��� �� �ʿ伺�� ����
// ���� ���� ��Ģ ���� �۷ι��� ������ ������ �����Ƿ� ������ window event manager�� ����Ͽ� ����
// �����ϵ��� ��
//
// root button�� ���Ե��� ���� ���� sub button�鸸���ε� Save()&Load()�� ���������� �̷� ��� ���� ��Ģ ��
// 3, 5 �� ���� �� ���ɼ��� �����Ƿ� �˾Ƽ� �� ���߾� �־�� ��
//
// ex>
//	MkSpreadButtonNode* rbNode = new MkSpreadButtonNode(L"SB");
//	bgNode->AttachChildNode(rbNode);
//	rbNode->CreateSelectionRootTypeButton(L"Default", MkFloat2(100.f, 20.f), MkSpreadButtonNode::eDownward);
//	MkBaseWindowNode::ItemTagInfo tagInfo;
//	tagInfo.iconPath = L"Default\\window_mgr.png";
//	tagInfo.iconSubset = L"SampleIcon";
//	rbNode->SetItemTag(tagInfo);
//	rbNode->SetLocalPosition(MkVec3(10.f, 30.f, -0.1f));
//
//	rbNode->AddItem(L"0", tagInfo);
//
//	MkSpreadButtonNode* g1 = rbNode->AddItem(L"1", tagInfo);
//	g1->AddItem(L"1-0", tagInfo);
//	g1->AddItem(L"1-1", tagInfo);
//	g1->AddItem(L"1-2", tagInfo);
//
//	rbNode->AddItem(L"2", tagInfo);
//
//	MkSpreadButtonNode* g3 = rbNode->AddItem(L"3", tagInfo);
//	g3->AddItem(L"3-0", tagInfo);
//	MkSpreadButtonNode* g31 = g3->AddItem(L"3-1", tagInfo);
//	g31->AddItem(L"3-1-0", tagInfo);
//
//	rbNode->SetTargetItem(L"1-1");
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkSpreadButtonNode : public MkBaseWindowNode
{
public:

	enum eSpreadButtonType // ��ư ����. �߰��� ����� �뿪�� ����
	{
		// root button type
		eSeletionRoot = 0, // ���õ� ��ư�� ǥ���ϴ� ��Ʈ ��ư(ex> �Ϲ� list box)
		eStaticRoot, // ǥ�ó����� ������ �ʴ� ��Ʈ(ex> ������ ���� ��ư)

		// sub button type
		eChildListButton = 0xff // �Ϲ� ���� ��ư
	};

	enum eOpeningDirection // ���� ����Ʈ ���� ����
	{
		eRightside = 0, // ���� ����
		eUpward, // ���� ����
		eDownward // ���� ����
	};

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_SpreadButtonNode; }

	MkSpreadButtonNode* GetRootListButton(void) const;
	MkSpreadButtonNode* GetRootListButton(int& depthFromRoot) const;

	//------------------------------------------------------------------------------------------------//
	// button control
	//------------------------------------------------------------------------------------------------//

	// eSeletionRoot ��ư ����
	bool CreateSelectionRootTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection);

	// eStaticRoot ��ư ����
	bool CreateStaticRootTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection, const ItemTagInfo& tagInfo);

	// ���� item �߰�(����)
	MkSpreadButtonNode* AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo);

	// ��� ������ ������� item�� ã�� ��ȯ
	MkSpreadButtonNode* GetItem(const MkHashStr& uniqueKey);

	// root button���� �ش�Ǵ� ������� ��� unique key�� ����
	// (NOTE) eSeletionRoot�� ��� �ش� item�� �����ϰ� �־�� ��
	bool SetTargetItem(const MkHashStr& uniqueKey);
	bool SetTargetItem(const MkSpreadButtonNode* targetNode);

	// root button���� �ش�Ǵ� ������� ��� unique key ����
	bool ClearTargetItem(void);

	// ��� ������ ������� item�� ã�� ����
	// ���� root button�� Ÿ������ �����Ǿ� �ִ� ��ư�̶�� ���� ����
	bool RemoveItem(const MkHashStr& uniqueKey);

	// item tag �ݿ�
	bool SetItemTag(const ItemTagInfo& tagInfo);

	// item tag ��ȯ
	inline const ItemTagInfo& GetItemTagInfo(void) const { return m_ItemTagInfo; }

	// ���� item�� ���� �� ��� ����Ʈ ����
	void OpenAllItems(void);

	// ���� �ִ� ����Ʈ�� ���� �� ��� ���� �������� ��� ����
	bool CloseAllItems(void);

	inline eSpreadButtonType GetSpreadButtonType(void) const { return m_ButtonType; }
	inline eOpeningDirection GetOpeningDirection(void) const { return m_OpeningDirection; }

	inline bool IsRootSpreadButton(void) const { return ((m_ButtonType == eSeletionRoot) || (m_ButtonType == eStaticRoot)); }

	inline const MkHashStr& GetTargetItemKey(void) const { return m_TargetUniqueKey; }

	inline const MkFloatRect& GetItemWorldAABR(void) const { return m_ItemWorldAABR; }

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// MkDataNode�� ����. MkSceneNode()�� ���� ��Ģ�� ����
	virtual void Load(const MkDataNode& node);

	// MkDataNode�� ���
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	virtual bool HitEventMousePress(unsigned int button, const MkFloat2& position);

	//------------------------------------------------------------------------------------------------//

	MkSpreadButtonNode(const MkHashStr& name);
	virtual ~MkSpreadButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	bool __CreateListTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection);

	bool __CheckUniqueKey(const MkHashStr& uniqueKey);

	void __UpdateItemRegion(void);

	inline void __ClearIconPartOfItemTag(void) { m_ItemTagInfo.iconPath.Clear(); m_ItemTagInfo.iconSubset.Clear(); }

protected:

	bool _CreateTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eSpreadButtonType buttonType, eOpeningDirection openingDirection);

protected:

	eSpreadButtonType m_ButtonType;
	eOpeningDirection m_OpeningDirection;

	ItemTagInfo m_ItemTagInfo;

	MkArray<MkHashStr> m_ItemSequence;

	MkHashStr m_TargetUniqueKey;

	MkFloatRect m_ItemWorldAABR;

	bool m_ItemsAreOpened;
};
