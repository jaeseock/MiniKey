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

	// root 생성
	bool CreateTabRoot(const MkHashStr& themeName, eTabAlignment tabAlighment, const MkFloat2& tabButtonSize, const MkFloat2& tabBodySize);

	// tab 추가. 반환값은 해당 tab의 윈도우 영역
	MkBaseWindowNode* AddTab(const MkHashStr& tabName, const ItemTagInfo& tagInfo);

	// 해당 tab의 윈도우 영역 반환
	MkBaseWindowNode* GetWindowNodeOfTab(const MkHashStr& tabName);

	// 해당 tab 선택
	bool SelectTab(const MkHashStr& tabName);

	// 해당 tab 사용 가능 여부 설정
	bool SetTabEnable(const MkHashStr& tabName, bool enable);

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
