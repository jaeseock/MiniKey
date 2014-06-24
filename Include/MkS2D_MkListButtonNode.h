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
		eChildListButton // 일반 버튼
	};

	enum eOpeningDirection // 하위 리스트 전개 방향
	{
		eRightside = 0, // 우측 전개
		eDownward, // 하향 전개
		eUpward // 상향 전개
	};

	typedef struct _ItemTagInfo
	{
		MkPathName iconPath;
		MkHashStr iconSubset;
		MkStr caption;
	}
	ItemTagInfo;

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_ListButtonNode; }

	MkListButtonNode* GetRootListButton(void) const;

	//------------------------------------------------------------------------------------------------//
	// 구성
	//------------------------------------------------------------------------------------------------//

	// MkDataNode로 구성. MkSceneNode()의 구성 규칙과 동일
	virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	virtual void Save(MkDataNode& node);

	bool CreateListTypeButton
		(const MkHashStr& themeName, const MkFloat2& windowSize, eListButtonType buttonType, eOpeningDirection openingDirection);

	MkListButtonNode* AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem = false);

	bool SetItemTag(const ItemTagInfo& tagInfo);

	inline const ItemTagInfo& GetItemTagInfo(void) const { return m_ItemTagInfo; }

	void AlignChildItems(void);

	inline eListButtonType GetListButtonType(void) const { return m_ButtonType; }
	inline eOpeningDirection GetOpeningDirection(void) const { return m_OpeningDirection; }

	inline bool IsRootListButton(void) const { return ((m_ButtonType == eSeletionRoot) || (m_ButtonType == eConstantRoot)); }

	void OpenAllItems(void);
	void CloseAllItems(void);

	//------------------------------------------------------------------------------------------------//

	virtual void SetPresetThemeName(const MkHashStr& themeName);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot);

	MkListButtonNode(const MkHashStr& name);
	virtual ~MkListButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	inline void __SetTargetUniqueKey(const MkHashStr& uniqueKey) { m_TargetUniqueKey = uniqueKey; }

protected:

	eListButtonType m_ButtonType;
	eOpeningDirection m_OpeningDirection;

	ItemTagInfo m_ItemTagInfo;

	MkHashStr m_TargetUniqueKey;

	MkArray<MkHashStr> m_ChildButtonSequence;

	bool m_ItemsAreOpened;
};
