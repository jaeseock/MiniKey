#pragma once


//------------------------------------------------------------------------------------------------//
// list button node : MkBaseWindowNode
// 
// 전개 규칙
// - 최초 로딩시 닫혀 있는 상태이어야 함
// - 루트 버튼으로부터 시작되는 tree를 하나의 그룹이라 할 때, 오직 하나의 그룹만 열린 상태를 유지해야 함
// - 부모 버튼이 전개중인 상태이어야 자식 버튼도 전개 할 수 있음
// - 형제 버튼들이 동시에 열려 있을 수 없음
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkSpreadButtonNode : public MkBaseWindowNode
{
public:

	enum eSpreadButtonType // 버튼 종류
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

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_SpreadButtonNode; }

	MkSpreadButtonNode* GetRootListButton(void) const;
	MkSpreadButtonNode* GetRootListButton(int& depthFromRoot) const;

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
	MkSpreadButtonNode* AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo, bool initialItem = false);

	// 모든 하위(해당 노드 미포함)를 대상으로 item을 찾아 반환
	MkSpreadButtonNode* GetItem(const MkHashStr& uniqueKey);

	// 모든 하위(해당 노드 미포함)를 대상으로 item을 찾아 제거
	// 만약 root button에 타겟으로 지정되어 있는 버튼이라면 지정 해제
	bool RemoveItem(const MkHashStr& uniqueKey);

	// item tag 반영
	bool SetItemTag(const ItemTagInfo& tagInfo);

	// item tag 반환
	inline const ItemTagInfo& GetItemTagInfo(void) const { return m_ItemTagInfo; }

	// 하위 item이 존재 할 경우 리스트 염
	void OpenAllItems(void);

	// 열려 있는 리스트가 존재 할 경우 하위 전개까지 모두 닫음
	bool CloseAllItems(void);

	inline eSpreadButtonType GetSpreadButtonType(void) const { return m_ButtonType; }
	inline eOpeningDirection GetOpeningDirection(void) const { return m_OpeningDirection; }

	inline bool IsRootSpreadButton(void) const { return ((m_ButtonType == eSeletionRoot) || (m_ButtonType == eStaticRoot)); }

	inline const MkHashStr& GetTargetUniqueKey(void) const { return m_TargetUniqueKey; }

	inline const MkFloatRect& GetItemWorldAABR(void) const { return m_ItemWorldAABR; }

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

	MkSpreadButtonNode(const MkHashStr& name);
	virtual ~MkSpreadButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	bool __CheckUniqueKey(const MkHashStr& uniqueKey);

	inline void __SetTargetUniqueKey(const MkHashStr& uniqueKey) { m_TargetUniqueKey = uniqueKey; }

	void __UpdateItemRegion(void);

protected:

	bool _CreateTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eSpreadButtonType buttonType, eOpeningDirection openingDirection);

	void _FlushItemRegion(void);

protected:

	eSpreadButtonType m_ButtonType;
	eOpeningDirection m_OpeningDirection;

	ItemTagInfo m_ItemTagInfo;

	MkArray<MkHashStr> m_ItemSequence;

	MkHashStr m_TargetUniqueKey;

	MkFloatRect m_ItemWorldAABR;

	bool m_ItemsAreOpened;
};
