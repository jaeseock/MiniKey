#pragma once


//------------------------------------------------------------------------------------------------//
// list button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkListButtonNode : public MkBaseWindowNode
{
public:

	enum eListButtonType // ��ư ����
	{
		eSeletionRoot = 0, // ���õ� ��ư�� ǥ���ϴ� ��Ʈ ��ư(ex> �Ϲ� list button)
		eConstantRoot, // ǥ�ó����� ������ �ʴ� ��Ʈ(ex> ������ ���� ��ư)
		eChildListButton // �Ϲ� ��ư
	};

	enum eOpeningDirection // ���� ����Ʈ ���� ����
	{
		eRightside = 0, // ���� ����
		eDownward, // ���� ����
		eUpward // ���� ����
	};

	typedef struct _ItemTagInfo
	{
		MkPathName iconPath;
		MkHashStr iconSubset;
		MkStr caption;
	}
	ItemTagInfo;

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_ListButtonNode; }

	MkListButtonNode* GetRootListButton(void) const;

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// MkDataNode�� ����. MkSceneNode()�� ���� ��Ģ�� ����
	virtual void Load(const MkDataNode& node);

	// MkDataNode�� ���
	virtual void Save(MkDataNode& node);

	bool CreateListTypeButton
		(const MkHashStr& themeName, const MkFloat2& windowSize, eListButtonType buttonType, eOpeningDirection openingDirection);

	MkListButtonNode* AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem = false);

	bool SetItemTag(const ItemTagInfo& tagInfo);

	inline const ItemTagInfo& GetItemTagInfo(void) const { return m_ItemTagInfo; }

	void AlignChildItems(void);

	inline eListButtonType GetListButtonType(void) const { return m_ButtonType; }
	inline eOpeningDirection GetOpeningDirection(void) const { return m_OpeningDirection; }

	inline bool IsRootListButton(void) const { return ((m_ButtonType == eSeletionRoot) || (m_ButtonType == eConstantRoot)); }

	void OpenAllItems(void);
	void CloseAllItems(void);

	//------------------------------------------------------------------------------------------------//

	virtual void SetPresetThemeName(const MkHashStr& themeName);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot);

	MkListButtonNode(const MkHashStr& name);
	virtual ~MkListButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	inline void __SetTargetUniqueKey(const MkHashStr& uniqueKey) { m_TargetUniqueKey = uniqueKey; }

protected:

	eListButtonType m_ButtonType;
	eOpeningDirection m_OpeningDirection;

	ItemTagInfo m_ItemTagInfo;

	MkHashStr m_TargetUniqueKey;

	MkArray<MkHashStr> m_ChildButtonSequence;

	bool m_ItemsAreOpened;
};
