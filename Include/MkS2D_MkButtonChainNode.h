#pragma once


//------------------------------------------------------------------------------------------------//
// list button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkButtonChainNode : public MkBaseWindowNode
{
public:

	enum eListButtonType // 버튼 종류
	{
		eSeletionRoot = 0, // 선택된 버튼을 표시하는 루트 버튼(ex> 일반 list box)
		eStaticRoot, // 표시내용이 변하지 않는 루트(ex> 윈도우 시작 버튼)
		eChildListButton // 일반 하위 버튼
	};

	enum eOpeningDirection // 하위 리스트 전개 방향
	{
		eRightside = 0, // 우측 전개
		eUpward, // 상향 전개
		eDownward // 하향 전개
	};

	typedef struct _ItemTagInfo
	{
		MkPathName iconPath;
		MkHashStr iconSubset;
		MkStr captionStr; // captionList보다 우선순위 높음
		MkArray<MkHashStr> captionList;
	}
	ItemTagInfo;

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_ButtonChainNode; }

	MkButtonChainNode* GetRootListButton(void) const;

	//------------------------------------------------------------------------------------------------//
	// button control
	//------------------------------------------------------------------------------------------------//

	// eSeletionRoot 버튼 생성
	bool CreateSelectionRootTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection);

	// eStaticRoot 버튼 생성
	bool CreateStaticRootTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection, const ItemTagInfo& tagInfo);

	// eChildListButton 버튼 생성
	bool CreateListTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection);

	// 하위 item 추가. initialItem이 true면 소속된 root button에게 타겟으로 지정 됨
	MkButtonChainNode* AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem = false);

	// item tag 반영
	bool SetItemTag(const ItemTagInfo& tagInfo);

	// item tag 반환
	inline const ItemTagInfo& GetItemTagInfo(void) const { return m_ItemTagInfo; }

	void AlignChildItems(void);

	void OpenAllItems(void);
	void CloseAllItems(void);

	inline eListButtonType GetListButtonType(void) const { return m_ButtonType; }
	inline eOpeningDirection GetOpeningDirection(void) const { return m_OpeningDirection; }

	inline bool IsRootListButton(void) const { return ((m_ButtonType == eSeletionRoot) || (m_ButtonType == eStaticRoot)); }

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

	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot);

	//------------------------------------------------------------------------------------------------//

	MkButtonChainNode(const MkHashStr& name);
	virtual ~MkButtonChainNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	bool __CheckUniqueKey(const MkHashStr& uniqueKey);

	inline void __SetTargetUniqueKey(const MkHashStr& uniqueKey) { m_TargetUniqueKey = uniqueKey; }

protected:

	bool _CreateTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eListButtonType buttonType, eOpeningDirection openingDirection);

protected:

	eListButtonType m_ButtonType;
	eOpeningDirection m_OpeningDirection;

	ItemTagInfo m_ItemTagInfo;

	MkHashStr m_TargetUniqueKey;

	MkArray<MkHashStr> m_ItemSequence;

	bool m_ItemsAreOpened;
};
