#pragma once


//------------------------------------------------------------------------------------------------//
// spread button node : MkBaseWindowNode
//
// 전개 규칙
// 0. 최초 로딩시 닫혀 있는 상태이어야 함
// 1. 루트 버튼으로부터 시작되는 tree를 하나의 그룹이라 할 때, 오직 하나의 그룹만 열린 상태를 유지해야 함
// 2. 부모 버튼이 전개중인 상태이어야 자식 버튼도 전개 할 수 있음
// 3. 그룹에 속한 버튼의 노드명(unique key)는 해당 그룹에서 유일해야 함
// 4. 형제 버튼들이 동시에 열려 있을 수 없음
// 5. 루트 버튼은 부모 윈도우 영역 안에 존재하고 하위 버튼은 루트 버튼의 크기를 따라감
//   (그래야 전개 없이 모든 버튼이 접혔을 때 충돌 체크에 문제가 없음)
//
// 영역 관리가 복잡한데, 일반적으로 하위 윈도우는 상위에 포함되지만 spread button의 경우 전개되다보면
// 상위 윈도우의 영역을 넘어가기가 쉬움. 이 경우 인풋 이벤트 등 여러 곳에서 문제가 생기므로 별도로
// 관리하여야 할 필요성이 있음
// 위의 전개 규칙 또한 글로벌한 성향을 가지고 있으므로 전개시 window event manager에 등록하여 별도
// 관리하도록 함
//
// root button이 포함되지 않은 하위 sub button들만으로도 Save()&Load()가 가능하지만 이럴 경우 전개 규칙 중
// 3, 5 를 위반 할 가능성이 있으므로 알아서 잘 맞추어 주어야 함
//
// ex>
//	MkSpreadButtonNode* rbNode = new MkSpreadButtonNode(L"SB");
//	bgNode->AttachChildNode(rbNode);
//	rbNode->CreateSelectionRootTypeButton(L"Default", MkFloat2(100.f, 20.f), MkSpreadButtonNode::eDownward);
//	MkBaseWindowNode::ItemTagInfo tagInfo;
//	tagInfo.iconPath = L"Default\\window_mgr.png";
//	tagInfo.iconSubset = L"SampleIcon";
//	rbNode->SetItemTag(tagInfo);
//	rbNode->SetLocalPosition(MkVec3(10.f, 30.f, -0.1f));
//
//	rbNode->AddItem(L"0", tagInfo);
//
//	MkSpreadButtonNode* g1 = rbNode->AddItem(L"1", tagInfo);
//	g1->AddItem(L"1-0", tagInfo);
//	g1->AddItem(L"1-1", tagInfo);
//	g1->AddItem(L"1-2", tagInfo);
//
//	rbNode->AddItem(L"2", tagInfo);
//
//	MkSpreadButtonNode* g3 = rbNode->AddItem(L"3", tagInfo);
//	g3->AddItem(L"3-0", tagInfo);
//	MkSpreadButtonNode* g31 = g3->AddItem(L"3-1", tagInfo);
//	g31->AddItem(L"3-1-0", tagInfo);
//
//	rbNode->SetTargetItem(L"1-1");
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkSpreadButtonNode : public MkBaseWindowNode
{
public:

	enum eSpreadButtonType // 버튼 종류. 추가를 고려해 대역폭 지정
	{
		// root button type
		eSeletionRoot = 0, // 선택된 버튼을 표시하는 루트 버튼(ex> 일반 list box)
		eStaticRoot, // 표시내용이 변하지 않는 루트(ex> 윈도우 시작 버튼)

		// sub button type
		eChildListButton = 0xff // 일반 하위 버튼
	};

	enum eOpeningDirection // 하위 리스트 전개 방향
	{
		eRightside = 0, // 우측 전개
		eUpward, // 상향 전개
		eDownward // 하향 전개
	};

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

	// 하위 item 추가(생성)
	MkSpreadButtonNode* AddItem(const MkHashStr& uniqueKey, const ItemTagInfo& tagInfo);

	// 모든 하위를 대상으로 item을 찾아 반환
	MkSpreadButtonNode* GetItem(const MkHashStr& uniqueKey);

	// root button에만 해당되는 기능으로 대상 unique key를 지정
	// (NOTE) eSeletionRoot의 경우 해당 item이 존재하고 있어야 함
	bool SetTargetItem(const MkHashStr& uniqueKey);
	bool SetTargetItem(const MkSpreadButtonNode* targetNode);

	// root button에만 해당되는 기능으로 대상 unique key 해제
	bool ClearTargetItem(void);

	// 모든 하위를 대상으로 item을 찾아 제거
	// 만약 root button에 타겟으로 지정되어 있는 버튼이라면 지정 해제
	bool RemoveItem(const MkHashStr& uniqueKey);

	// item tag 반영
	bool SetItemTag(const ItemTagInfo& tagInfo);

	// item tag 반환
	inline const ItemTagInfo& GetItemTagInfo(void) const { return m_ItemTagInfo; }

	// 하위 item이 존재 할 경우 리스트 전개
	void OpenAllItems(void);

	// 열려 있는 리스트가 존재 할 경우 하위 전개까지 모두 닫음
	bool CloseAllItems(void);

	inline eSpreadButtonType GetSpreadButtonType(void) const { return m_ButtonType; }
	inline eOpeningDirection GetOpeningDirection(void) const { return m_OpeningDirection; }

	inline bool IsRootSpreadButton(void) const { return ((m_ButtonType == eSeletionRoot) || (m_ButtonType == eStaticRoot)); }

	inline const MkHashStr& GetTargetItemKey(void) const { return m_TargetUniqueKey; }

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

	virtual bool HitEventMousePress(unsigned int button, const MkFloat2& position);

	//------------------------------------------------------------------------------------------------//

	MkSpreadButtonNode(const MkHashStr& name);
	virtual ~MkSpreadButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	bool __CreateListTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eOpeningDirection openingDirection);

	bool __CheckUniqueKey(const MkHashStr& uniqueKey);

	void __UpdateItemRegion(void);

	inline void __ClearIconPartOfItemTag(void) { m_ItemTagInfo.iconPath.Clear(); m_ItemTagInfo.iconSubset.Clear(); }

protected:

	bool _CreateTypeButton(const MkHashStr& themeName, const MkFloat2& windowSize, eSpreadButtonType buttonType, eOpeningDirection openingDirection);

protected:

	eSpreadButtonType m_ButtonType;
	eOpeningDirection m_OpeningDirection;

	ItemTagInfo m_ItemTagInfo;

	MkArray<MkHashStr> m_ItemSequence;

	MkHashStr m_TargetUniqueKey;

	MkFloatRect m_ItemWorldAABR;

	bool m_ItemsAreOpened;
};
