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

	// root 생성
	bool CreateTabRoot(const MkHashStr& themeName, eTabAlignment tabAlighment, const MkFloat2& tabButtonSize, const MkFloat2& tabBodySize);

	// tab button size 수정
	bool SetTabButtonSize(const MkFloat2& tabButtonSize);

	// tab button size 반환
	const MkFloat2& GetTabButtonSize(void) const { return m_TabButtonSize; }

	// tab body size 수정
	bool SetTabBodySize(const MkFloat2& tabBodySize);

	// tab body size 반환
	const MkFloat2& GetTabBodySize(void) const { return m_TabBodySize; }

	// tab 추가. 반환값은 해당 tab의 윈도우 영역
	// customWindow에 전용 객체를 대신 넣을 수 있으며 이 경우 반환값과 동일(NULL일경우 MkBaseWindowNode*를 생성해 반환)
	MkBaseWindowNode* AddTab(const MkHashStr& tabName, const ItemTagInfo& tagInfo, MkBaseWindowNode* customWindow = NULL);

	// 해당 tab의 윈도우 영역 반환
	MkBaseWindowNode* GetWindowNodeOfTab(const MkHashStr& tabName);

	// 해당 tab 선택
	bool SelectTab(const MkHashStr& tabName);

	// 해당 tab 사용 가능 여부 설정
	bool SetTabEnable(const MkHashStr& tabName, bool enable);

	// 현재 선택중인 tab 반환
	const MkHashStr& GetCurrentFrontTab(void) const { return m_CurrentFrontTab; }

	// 해당 tab의 순서 반환. 존재하지 않는 tab이라면 MKDEF_ARRAY_ERROR 반환
	unsigned int GetTabSequence(const MkHashStr& tabName) const;

	// 해당 tab의 순서 변경
	bool SetTabSequence(const MkHashStr& tabName, unsigned int position);

	// 해당 tab 순서를 하나 앞/뒤로 이동
	bool MoveTabToOneStepForward(const MkHashStr& tabName);
	bool MoveTabToOneStepBackword(const MkHashStr& tabName);

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
