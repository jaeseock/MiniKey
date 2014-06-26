#pragma once


//------------------------------------------------------------------------------------------------//
// list button node : MkBaseWindowNode
// 
// ���� ��Ģ
// - ���� �ε��� ���� �ִ� �����̾�� ��
// - ��Ʈ ��ư���κ��� ���۵Ǵ� tree�� �ϳ��� �׷��̶� �� ��, ���� �ϳ��� �׷츸 ���� ���¸� �����ؾ� ��
// - �θ� ��ư�� �������� �����̾�� �ڽ� ��ư�� ���� �� �� ����
// - ���� ��ư���� ���ÿ� ���� ���� �� ����
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkSpreadButtonNode : public MkBaseWindowNode
{
public:

	enum eSpreadButtonType // ��ư ����
	{
		eSeletionRoot = 0, // ���õ� ��ư�� ǥ���ϴ� ��Ʈ ��ư(ex> �Ϲ� list box)
		eStaticRoot, // ǥ�ó����� ������ �ʴ� ��Ʈ(ex> ������ ���� ��ư)
		eChildListButton // �Ϲ� ���� ��ư
	};

	enum eOpeningDirection // ���� ����Ʈ ���� ����
	{
		eRightside = 0, // ���� ����
		eUpward, // ���� ����
		eDownward // ���� ����
	};

	typedef struct _ItemTagInfo
	{
		MkPathName iconPath;
		MkHashStr iconSubset;
		MkStr captionStr; // captionList���� �켱���� ����
		MkArray<MkHashStr> captionList;
	}
	ItemTagInfo;

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

	// eChildListButton ��ư ����
	bool CreateListTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection);

	// ���� item �߰�. initialItem�� true�� �Ҽӵ� root button���� Ÿ������ ���� ��
	MkSpreadButtonNode* AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem = false);

	// ��� ����(�ش� ��� ������)�� ������� item�� ã�� ��ȯ
	MkSpreadButtonNode* GetItem(const MkHashStr& uniqueKey);

	// ��� ����(�ش� ��� ������)�� ������� item�� ã�� ����
	// ���� root button�� Ÿ������ �����Ǿ� �ִ� ��ư�̶�� ���� ����
	bool RemoveItem(const MkHashStr& uniqueKey);

	// item tag �ݿ�
	bool SetItemTag(const ItemTagInfo& tagInfo);

	// item tag ��ȯ
	inline const ItemTagInfo& GetItemTagInfo(void) const { return m_ItemTagInfo; }

	// ���� item�� ���� �� ��� ����Ʈ ��
	void OpenAllItems(void);

	// ���� �ִ� ����Ʈ�� ���� �� ��� ���� �������� ��� ����
	bool CloseAllItems(void);

	inline eSpreadButtonType GetSpreadButtonType(void) const { return m_ButtonType; }
	inline eOpeningDirection GetOpeningDirection(void) const { return m_OpeningDirection; }

	inline bool IsRootSpreadButton(void) const { return ((m_ButtonType == eSeletionRoot) || (m_ButtonType == eStaticRoot)); }

	inline const MkHashStr& GetTargetUniqueKey(void) const { return m_TargetUniqueKey; }

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

	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot);

	//------------------------------------------------------------------------------------------------//

	MkSpreadButtonNode(const MkHashStr& name);
	virtual ~MkSpreadButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	bool __CheckUniqueKey(const MkHashStr& uniqueKey);

	inline void __SetTargetUniqueKey(const MkHashStr& uniqueKey) { m_TargetUniqueKey = uniqueKey; }

	void __UpdateItemRegion(void);

protected:

	bool _CreateTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eSpreadButtonType buttonType, eOpeningDirection openingDirection);

	void _FlushItemRegion(void);

protected:

	eSpreadButtonType m_ButtonType;
	eOpeningDirection m_OpeningDirection;

	ItemTagInfo m_ItemTagInfo;

	MkArray<MkHashStr> m_ItemSequence;

	MkHashStr m_TargetUniqueKey;

	MkFloatRect m_ItemWorldAABR;

	bool m_ItemsAreOpened;
};
