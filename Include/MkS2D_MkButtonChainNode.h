#pragma once


//------------------------------------------------------------------------------------------------//
// list button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkButtonChainNode : public MkBaseWindowNode
{
public:

	enum eListButtonType // ��ư ����
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

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_ButtonChainNode; }

	MkButtonChainNode* GetRootListButton(void) const;

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
	MkButtonChainNode* AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem = false);

	// item tag �ݿ�
	bool SetItemTag(const ItemTagInfo& tagInfo);

	// item tag ��ȯ
	inline const ItemTagInfo& GetItemTagInfo(void) const { return m_ItemTagInfo; }

	void AlignChildItems(void);

	void OpenAllItems(void);
	void CloseAllItems(void);

	inline eListButtonType GetListButtonType(void) const { return m_ButtonType; }
	inline eOpeningDirection GetOpeningDirection(void) const { return m_OpeningDirection; }

	inline bool IsRootListButton(void) const { return ((m_ButtonType == eSeletionRoot) || (m_ButtonType == eStaticRoot)); }

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

	MkButtonChainNode(const MkHashStr& name);
	virtual ~MkButtonChainNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	bool __CheckUniqueKey(const MkHashStr& uniqueKey);

	inline void __SetTargetUniqueKey(const MkHashStr& uniqueKey) { m_TargetUniqueKey = uniqueKey; }

protected:

	bool _CreateTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eListButtonType buttonType, eOpeningDirection openingDirection);

protected:

	eListButtonType m_ButtonType;
	eOpeningDirection m_OpeningDirection;

	ItemTagInfo m_ItemTagInfo;

	MkHashStr m_TargetUniqueKey;

	MkArray<MkHashStr> m_ItemSequence;

	bool m_ItemsAreOpened;
};
