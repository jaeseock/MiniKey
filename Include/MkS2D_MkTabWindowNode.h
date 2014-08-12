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
		eLeftside = 0,
		eRightside
	};

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_TabWindowNode; }

	//------------------------------------------------------------------------------------------------//
	// button control
	//------------------------------------------------------------------------------------------------//

	// root ����
	bool CreateTabRoot(const MkHashStr& themeName, eTabAlignment tabAlighment, const MkFloat2& tabButtonSize, const MkFloat2& tabBodySize);

	// tab button size ����
	bool SetTabButtonSize(const MkFloat2& tabButtonSize);

	// tab button size ��ȯ
	const MkFloat2& GetTabButtonSize(void) const { return m_TabButtonSize; }

	// tab body size ����
	bool SetTabBodySize(const MkFloat2& tabBodySize);

	// tab body size ��ȯ
	const MkFloat2& GetTabBodySize(void) const { return m_TabBodySize; }

	// tab �߰�. ��ȯ���� �ش� tab�� ������ ����
	// customWindow�� ���� ��ü�� ��� ���� �� ������ �� ��� ��ȯ���� ����(NULL�ϰ�� MkBaseWindowNode*�� ������ ��ȯ)
	MkBaseWindowNode* AddTab(const MkHashStr& tabName, const ItemTagInfo& tagInfo, MkBaseWindowNode* customWindow = NULL);

	// �ش� tab�� ������ ���� ��ȯ
	MkBaseWindowNode* GetWindowNodeOfTab(const MkHashStr& tabName);

	// �ش� tab ����
	bool SelectTab(const MkHashStr& tabName);

	// �ش� tab ��� ���� ���� ����
	bool SetTabEnable(const MkHashStr& tabName, bool enable);

	// ���� �������� tab ��ȯ
	const MkHashStr& GetCurrentFrontTab(void) const { return m_CurrentFrontTab; }

	// �ش� tab�� ���� ��ȯ. �������� �ʴ� tab�̶�� MKDEF_ARRAY_ERROR ��ȯ
	unsigned int GetTabSequence(const MkHashStr& tabName) const;

	// �ش� tab�� ���� ����
	bool SetTabSequence(const MkHashStr& tabName, unsigned int position);

	// �ش� tab ������ �ϳ� ��/�ڷ� �̵�
	bool MoveTabToOneStepForward(const MkHashStr& tabName);
	bool MoveTabToOneStepBackword(const MkHashStr& tabName);

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

	void _RepositionTabs(unsigned int startPos);
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
