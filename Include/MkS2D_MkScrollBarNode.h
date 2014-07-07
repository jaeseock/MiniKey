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

	bool CreateScrollBar(const MkHashStr& themeName, eBarDirection direction, bool hasDirBtn, float length);

	void SetPageInfo(unsigned int totalPageSize, unsigned int onePageSize, unsigned int sizePerGrid);
	void SetPageInfo(unsigned int totalPageSize);

	inline unsigned int GetGridCount(void) const { return m_GridCount; }
	inline unsigned int GetCurrentGridPosition(void) const { return m_CurrentGridPosition; }
	inline unsigned int GetCurrentPagePosition(void) const { return m_CurrentPagePosition; }

	void SetWheelMovementCatchingArea(const MkFloat2& areaSize);

	//------------------------------------------------------------------------------------------------//
	// 구성
	//------------------------------------------------------------------------------------------------//

	// MkDataNode로 구성. MkSceneNode()의 구성 규칙과 동일
	virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	//------------------------------------------------------------------------------------------------//

	MkScrollBarNode(const MkHashStr& name);
	virtual ~MkScrollBarNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	void _UpdateSlideTransform(void);

protected:

	eBarDirection m_BarDirection;
	unsigned int m_TotalPageSize;
	unsigned int m_OnePageSize;
	unsigned int m_SizePerGrid;

	MkBaseWindowNode* m_SlideNode;
	float m_MaxSlideSize;
	unsigned int m_GridCount;
	unsigned int m_CurrentGridPosition;
	unsigned int m_CurrentPagePosition;

	MkFloat2 m_WheelMovementCatchingArea;
};
