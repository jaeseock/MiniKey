#pragma once


//------------------------------------------------------------------------------------------------//
// tab window node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkTabWindowNode : public MkBaseWindowNode
{
public:

	enum eTabAlignment
	{
		eLeftToRight = 0,
		eRightToLeft
	};

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_TabWindowNode; }

	//------------------------------------------------------------------------------------------------//
	// button control
	//------------------------------------------------------------------------------------------------//

	// root ����
	bool CreateTabRoot(const MkHashStr& themeName, eTabAlignment tabAlighment, const MkFloat2& tabButtonSize, const MkFloat2& tabBodySize);

	// tab �߰�. ��ȯ���� �ش� tab�� ������ ����
	MkBaseWindowNode* AddTab(const MkHashStr& tabName, const ItemTagInfo& tagInfo);

	// �ش� tab�� ������ ���� ��ȯ
	MkBaseWindowNode* GetWindowNodeOfTab(const MkHashStr& tabName);

	// �ش� tab ����
	bool SelectTab(const MkHashStr& tabName);

	// �ش� tab ��� ���� ���� ����
	bool SetTabEnable(const MkHashStr& tabName, bool enable);

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

	virtual void UseWindowEvent(WindowEvent& evt);

	//------------------------------------------------------------------------------------------------//

	virtual void Clear(void);

	MkTabWindowNode(const MkHashStr& name);
	virtual ~MkTabWindowNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	void _SetTabState(const MkHashStr& tabName, bool front);
	void _MoveTargetTabToFront(const MkHashStr& tabName);

protected:

	eTabAlignment m_TabAlighment;
	MkFloat2 m_TabButtonSize;
	MkFloat2 m_TabBodySize;
	MkArray<MkHashStr> m_TabList;

	unsigned int m_TabCapacity;
	MkHashMap<MkHashStr, MkBaseWindowNode*> m_Tabs;
	MkHashStr m_CurrentFrontTab;
};
