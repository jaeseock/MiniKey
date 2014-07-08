#pragma once


//------------------------------------------------------------------------------------------------//
// smart button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkScrollBarNode : public MkBaseWindowNode
{
public:

	enum eBarDirection
	{
		eVertical = 0,
		eHorizontal
	};

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_ScrollBarNode; }

	//------------------------------------------------------------------------------------------------//
	// component control
	//------------------------------------------------------------------------------------------------//

	// ����
	// hasDirBtn : prev/next ��ư ���翩��
	// length : ��ư�� ��ũ�� �ٸ� ������ ��ü ����
	bool CreateScrollBar(const MkHashStr& themeName, eBarDirection direction, bool hasDirBtn, float length);

	// ���� �Է�
	// totalPageSize : ��ü�� �ش��ϴ� ���� ����
	// onePageSize : �� �������� �ش��ϴ� ���� ����(�ּ� 1 �̻�)
	// sizePerGrid : ��ġ ���� �ϳ��� �ش��ϴ� ���� ����(onePageSize ����)
	// gridsPerAction : �̵� ��ư/��ũ�ѽ� �̵��� ���� ����
	void SetPageInfo(unsigned int totalPageSize, unsigned int onePageSize, unsigned int sizePerGrid, unsigned int gridsPerAction = 1);

	// ��ü�� �ش��ϴ� ���� ���̸� ����
	void SetPageInfo(unsigned int totalPageSize);

	// ���� ��ġ ���� ����
	void SetGridPosition(unsigned int gridPosition);

	// �� ��ġ ���� �� ��ȯ
	inline unsigned int GetGridCount(void) const { return m_GridCount; }

	// ���� ��ġ ���� ��ȯ
	inline unsigned int GetCurrentGridPosition(void) const { return m_CurrentGridPosition; }

	// ���� ���� ��ġ ��ȯ(0 ~ [totalPageSize - onePageSize])
	inline unsigned int GetCurrentPagePosition(void) const { return m_CurrentPagePosition; }

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

	virtual bool HitEventMouseWheelMove(int delta, const MkFloat2& position);

	virtual void UseWindowEvent(WindowEvent& evt);

	//------------------------------------------------------------------------------------------------//

	MkScrollBarNode(const MkHashStr& name);
	virtual ~MkScrollBarNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	void _UpdateSlideTransform(void);

	unsigned int _GetNewGridPosition(int offset) const;

protected:

	eBarDirection m_BarDirection;
	unsigned int m_TotalPageSize;
	unsigned int m_OnePageSize;
	unsigned int m_SizePerGrid;
	int m_GridsPerAction;

	MkBaseWindowNode* m_SlideNode;
	float m_MaxSlideSize;
	unsigned int m_GridCount;
	unsigned int m_CurrentGridPosition;
	unsigned int m_CurrentPagePosition;
};
