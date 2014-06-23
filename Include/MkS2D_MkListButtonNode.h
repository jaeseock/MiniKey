#pragma once


//------------------------------------------------------------------------------------------------//
// list button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkListButtonNode : public MkBaseWindowNode
{
public:

	enum eListButtonType // 버튼 종류
	{
		eSeletionRoot = 0, // 선택된 버튼을 표시하는 루트 버튼(ex> 일반 list button)
		eConstantRoot, // 표시내용이 변하지 않는 루트(ex> 윈도우 시작 버튼)
		eChildButton // 일반 버튼
	};

	enum eOpeningMethod // 리스트 열리는 방식
	{
		ePressAndOnCursorInTimeOnly = 0, // 누르거나 일정 시간 머무르면
		ePressOnly, // 누를 때
		eOnCursorInTimeOnly, // 일정 시간 머무를 때
		eRelease // 클릭되었을 때
	};

	enum eOpeningDirection // 리스트 전개 방향
	{
		eRightside = 0, // 우측 전개
		eDownward, // 하향 전개
		eUpward // 상향 전개
	};

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_ListButtonNode; }

	//------------------------------------------------------------------------------------------------//
	// 구성
	//------------------------------------------------------------------------------------------------//

	// MkDataNode로 구성. MkSceneNode()의 구성 규칙과 동일
	virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	virtual void Save(MkDataNode& node);

	//bool CreateListButton(eListButtonType btnType, eOpeningMethod opMethod, eOpeningDirection opDir,
	//	const MkHashStr& themeName, const MkPathName& iconPath, const MkHashStr& iconSubset)

	//bool CreateList(const MkArray<MkHashStr>& menuList, unsigned int initIndex);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot);
	virtual bool InputEventMouseRelease(unsigned int button, const MkFloat2& position, bool managedRoot);

	MkListButtonNode(const MkHashStr& name) : MkBaseWindowNode(name) {}
	MkListButtonNode(const MkHashStr& name, const MkHashStr& themeName, const MkFloat2& bodySize, const MkHashStr& componentName) : MkBaseWindowNode(name, themeName, bodySize, componentName) {}
	virtual ~MkListButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	MkHashStr m_TargetNodeName;
};
