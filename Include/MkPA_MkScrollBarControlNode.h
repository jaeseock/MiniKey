#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : scroll bar
//
// ex>
//	MkScrollBarControlNode* hScrollBar = MkScrollBarControlNode::CreateChildNode(bodyFrame, L"HSB");
//	hScrollBar->SetHorizontalScrollBar(MkWindowThemeData::DefaultThemeName, 10, 4, bodyFrame->GetClientSize().x);
//
//	MkScrollBarControlNode* vScrollBar = MkScrollBarControlNode::CreateChildNode(bodyFrame, L"VSB");
//	vScrollBar->SetVerticalScrollBar(MkWindowThemeData::DefaultThemeName, 100, 20, bodyFrame->GetClientSize().y);
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkScrollBarControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_ScrollBarControlNode; }

	// alloc child instance
	static MkScrollBarControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// control interface
	//------------------------------------------------------------------------------------------------//

	// totalItemSize : ��� item ����
	// onePageItemSize : �� page�� ǥ�õ� item ����
	// length : �� page�� �ش��ϴ� ���� pixel size
	// (NOTE) ���� item ��ġ�� 0���� �ʱ�ȭ ��
	void SetHorizontalScrollBar(const MkHashStr& themeName, int totalItemSize, int onePageItemSize, float length);
	void SetVerticalScrollBar(const MkHashStr& themeName, int totalItemSize, int onePageItemSize, float length);

	// ��� item ���� ����/��ȯ
	// (NOTE) ���� item ��ġ�� 0���� �ʱ�ȭ ��
	// (NOTE) ȣ�� �� SetHorizontalScrollBar()/SetVerticalScrollBar()�� �ùٸ��� ȣ��� �����̾�� ��
	void SetTotalItemSize(int totalItemSize);
	inline int GetTotalItemSize(void) const { return m_TotalItemSize; }

	// �� page�� ǥ�õ� item ���� ����/��ȯ
	// (NOTE) ���� item ��ġ�� 0���� �ʱ�ȭ ��
	// (NOTE) ȣ�� �� SetHorizontalScrollBar()/SetVerticalScrollBar()�� �ùٸ��� ȣ��� �����̾�� ��
	void SetOnePageItemSize(int onePageItemSize);
	inline int GetOnePageItemSize(void) const { return m_OnePageItemSize; }

	// ���� item ��ġ ����/��ȯ
	// (NOTE) ȣ�� �� SetHorizontalScrollBar()/SetVerticalScrollBar()�� �ùٸ��� ȣ��� �����̾�� ��
	void SetItemPosition(int position);
	inline int GetItemPosition(void) const { return m_CurrentItemPosition; }

	// wheel scroll
	// �ܰ躰 �̵����� MKDEF_WHEEL_SCROLLING_LEVEL ������ ����
	void WheelScrolling(int delta);

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	virtual const MkFloatRect* GetDraggingMovementLock(void) const { return &m_WindowRect; } // window rect

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//

	MkScrollBarControlNode(const MkHashStr& name);
	virtual ~MkScrollBarControlNode() {}

protected:

	void _SetScrollBar(const MkHashStr& themeName, int totalItemSize, int onePageItemSize, float length, bool horizontal, bool updateTotalBar);

	void _SetItemPosition(int position, bool filter, bool modifyLocalPos);
	int _ItemPositionFilter(int position) const;
	int _ConvertPageBarPositionToItemPosition(float pageBarPos) const;
	float _GetPageBarPosition(const MkSceneNode* node) const;

protected:

	int m_TotalItemSize;
	int m_OnePageItemSize;
	float m_TotalBarLength;
	bool m_Horizontal;
	float m_PageBarLength;
	int m_CurrentItemPosition;

public:

	static const MkHashStr PageBarNodeName;
	static const float BarWidth;

	static const MkHashStr ArgKey_NewItemPosOfScrollBar;
};
