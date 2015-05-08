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

	// totalItemSize : 모든 item 갯수
	// onePageItemSize : 한 page에 표시될 item 갯수
	// length : 한 page에 해당하는 실제 pixel size
	// (NOTE) 현재 item 위치는 0으로 초기화 됨
	void SetHorizontalScrollBar(const MkHashStr& themeName, int totalItemSize, int onePageItemSize, float length);
	void SetVerticalScrollBar(const MkHashStr& themeName, int totalItemSize, int onePageItemSize, float length);

	// 모든 item 갯수 설정/반환
	// (NOTE) 현재 item 위치는 0으로 초기화 됨
	// (NOTE) 호출 전 SetHorizontalScrollBar()/SetVerticalScrollBar()가 올바르게 호출된 상태이어야 함
	void SetTotalItemSize(int totalItemSize);
	inline int GetTotalItemSize(void) const { return m_TotalItemSize; }

	// 한 page에 표시될 item 갯수 설정/반환
	// (NOTE) 현재 item 위치는 0으로 초기화 됨
	// (NOTE) 호출 전 SetHorizontalScrollBar()/SetVerticalScrollBar()가 올바르게 호출된 상태이어야 함
	void SetOnePageItemSize(int onePageItemSize);
	inline int GetOnePageItemSize(void) const { return m_OnePageItemSize; }

	// 현재 item 위치 설정/반환
	// (NOTE) 호출 전 SetHorizontalScrollBar()/SetVerticalScrollBar()가 올바르게 호출된 상태이어야 함
	void SetItemPosition(int position);
	inline int GetItemPosition(void) const { return m_CurrentItemPosition; }

	// wheel scroll
	// 단계별 이동량은 MKDEF_WHEEL_SCROLLING_LEVEL 비율을 따름
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
