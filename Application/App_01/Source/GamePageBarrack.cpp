
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkScrollBarNode.h"
#include "MkS2D_MkWindowEventManager.h"

#include "GameSUI_Thumbnail.h"
//#include "GameDataNode.h"
#include "GameSystemManager.h"
#include "GamePageBarrack.h"

//------------------------------------------------------------------------------------------------//

#define GDEF_ALISTSB_ONE_PAGE_SIZE 20

class GP_BarrackWindow : public GP_IslandBaseWindow
{
public:

	virtual bool HitEventMouseWheelMove(int delta, const MkFloat2& position)
	{
		if (m_AListSB != NULL)
		{
			const MkFloatRect& sbAABR = m_AListSB->GetWorldAABR();
			if ((position.x > sbAABR.position.x) && (position.x < (sbAABR.position.x + 330.f)) &&
				(position.y > sbAABR.position.y) && (position.y < (sbAABR.position.y + sbAABR.size.y)))
			{
				if (!sbAABR.CheckGridIntersection(position))
				{
					m_AListSB->HitEventMouseWheelMove(delta, position); // 가상으로 event를 보내 줌
				}
			}
		}
		return false;
	}

	virtual void UseWindowEvent(WindowEvent& evt)
	{
		switch (evt.type)
		{
		case MkSceneNodeFamilyDefinition::eScrollPositionChanged:
			_UpdateAgentListScroll(m_AListSB->GetCurrentPagePosition());
			break;

		case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
			{
				if (evt.node->GetNodeName().GetString().IsDigit())
				{
					m_TargetAgentBtn = dynamic_cast<GameSUI_SmallAgentThumbnail*>(evt.node);
					_UpdateBattleInfoControl();
					_UpdateTargetAgentInfoTag();
				}
				else if (evt.node == m_ToBattleBtn)
				{
					unsigned int agentID = m_TargetAgentBtn->GetNodeName().GetString().ToUnsignedInteger();
					if (GAME_SYSTEM.GetMasterPlayer().GetAgentGroup().SetTrooper(agentID, true))
					{
						m_TargetAgentBtn->SetToBattle(true);
						_UpdateTrooperInfoTag();
						_UpdateBattleInfoControl();
					}
				}
				else if (evt.node == m_CancelBattleBtn)
				{
					unsigned int agentID = m_TargetAgentBtn->GetNodeName().GetString().ToUnsignedInteger();
					if (GAME_SYSTEM.GetMasterPlayer().GetAgentGroup().SetTrooper(agentID, false))
					{
						m_TargetAgentBtn->SetToBattle(false);
						_UpdateTrooperInfoTag();
						_UpdateBattleInfoControl();
					}
				}
				else if (evt.node->GetNodeName().GetString().CheckPrefix(L"SortBy"))
				{
					for (int i=eGSM_Name; i<eGSM_Max; ++i)
					{
						if (evt.node->GetNodeName() == m_SortBtnName[i])
						{
							GAME_SYSTEM.GetMasterPlayer().GetAgentGroup().SortBy(static_cast<eGroupSortingMethod>(i));
							_UpdateSortingResult();
							_UpdateAgentListScroll(m_AListSB->GetCurrentPagePosition());
							break;
						}
					}
				}
			}
			break;
		}
	}

	GP_BarrackWindow(const MkHashStr& name) : GP_IslandBaseWindow(name, L"Scene\\barrack_page.msd")
	{
		m_AListSB = dynamic_cast<MkScrollBarNode*>(GetChildNode(L"AListSB"));
		m_SortBtnName[eGSM_Name] = L"SortByName";
		m_SortBtnName[eGSM_DescendingLevel] = L"SortByLvD";
		m_SortBtnName[eGSM_AscendingLevel] = L"SortByLvA";

		m_ToBattleBtn = dynamic_cast<MkBaseWindowNode*>(GetChildNode(L"ToBattle"));
		m_CancelBattleBtn = dynamic_cast<MkBaseWindowNode*>(GetChildNode(L"CancelBattle"));
		m_TrooperInfoTag = GetSRect(L"PrivateTag_1");

		const MkArray<const GameAgentUnitInfo*>& members = GAME_SYSTEM.GetMasterPlayer().GetAgentGroup().GetMemberSequence();
		if (!members.Empty())
		{
			m_AListSB->SetPageInfo(members.GetSize(), GDEF_ALISTSB_ONE_PAGE_SIZE, 1, 1);

			m_AListRoot = new MkBaseWindowNode(L"ListRoot");
			AttachChildNode(m_AListRoot);

			MK_INDEXING_LOOP(members, i)
			{
				const GameAgentUnitInfo& aui = *members[i];

				unsigned int agentID = aui.GetAgentID();
				GameSUI_SmallAgentThumbnail* btn = new GameSUI_SmallAgentThumbnail(MkStr(agentID));
				btn->CreateSmallAgentThumbnail(agentID, aui.GetAgentLevel(), GAME_SYSTEM.GetMasterPlayer().GetAgentGroup().IsTrooper(agentID));
				btn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
				m_AListRoot->AttachChildNode(btn);
				m_AgentBtns.Create(agentID, btn);
			}

			_UpdateSortingResult();
			_UpdateAgentListScroll(0);
		}

		_UpdateTrooperInfoTag();

		m_TargetAgentBtn = NULL;
		_UpdateBattleInfoControl();
		_UpdateTargetAgentInfoTag();
	}
	virtual ~GP_BarrackWindow() {}

protected:

	void _UpdateSortingResult(void)
	{
		const MkArray<const GameAgentUnitInfo*>& members = GAME_SYSTEM.GetMasterPlayer().GetAgentGroup().GetMemberSequence();
		const float btnHeight = 34.f;
		const float totalHeight = btnHeight * members.GetSize();

		MK_INDEXING_LOOP(members, i)
		{
			unsigned int agentID = members[i]->GetAgentID();
			GameSUI_SmallAgentThumbnail* btn = m_AgentBtns[agentID];
			btn->SetLocalPosition(MkFloat2(0.f, totalHeight - static_cast<float>(i + 1) * btnHeight));
		}
	}

	void _UpdateAgentListScroll(unsigned int pagePos)
	{
		unsigned int itemCount = m_AListSB->GetTotalPageSize();
		const float btnHeight = 34.f;
		const float totalHeight = btnHeight * static_cast<float>(itemCount);
		const float onePageHeight = btnHeight * static_cast<float>(20);

		float pos = onePageHeight - totalHeight + pagePos * btnHeight;
		m_AListRoot->SetLocalPosition(MkVec3(200.f, m_AListSB->GetLocalPosition().y + pos, -MKDEF_BASE_WINDOW_DEPTH_GRID));

		unsigned int endLine = pagePos + GDEF_ALISTSB_ONE_PAGE_SIZE;
		const MkArray<const GameAgentUnitInfo*>& members = GAME_SYSTEM.GetMasterPlayer().GetAgentGroup().GetMemberSequence();
		for (unsigned int i=0; i<itemCount; ++i)
		{
			unsigned int agentID = members[i]->GetAgentID();
			m_AListRoot->GetChildNode(MkStr(agentID))->SetVisible((i >= pagePos) && (i < endLine));
		}
	}

	void _UpdateTargetAgentInfoTag(void)
	{
		const MkHashStr viewKey = L"AgentView";

		if (ChildExist(viewKey))
		{
			MkSceneNode* viewNode = GetChildNode(viewKey);
			DetachChildNode(viewKey);
			delete viewNode;
		}

		if (m_TargetAgentBtn != NULL)
		{
			GameSUI_SmallAgentView* viewNode = new GameSUI_SmallAgentView(viewKey);
			unsigned int agentID = m_TargetAgentBtn->GetNodeName().GetString().ToUnsignedInteger();
			viewNode->CreateSmallAgentView(agentID);
			viewNode->SetLocalPosition(MkVec3(630.f, 600.f, -MKDEF_BASE_WINDOW_DEPTH_GRID));
			AttachChildNode(viewNode);
		}
	}

	void _UpdateTrooperInfoTag(void)
	{
		if (m_TrooperInfoTag != NULL)
		{
			const GameAgentGroupInfo& gi = GAME_SYSTEM.GetMasterPlayer().GetAgentGroup();
			unsigned int currCount = gi.GetCurrTroopers();
			unsigned int maxCount = GDEF_MAX_TROOP_AGENT_COUNT;
			MkStr msg = L"전투 대기조 최대 " + MkStr(maxCount) + L"명 중 " + MkStr(currCount) + L"명 선택됨";
			MkStr buffer;
			const MkHashStr& fontState = (currCount < maxCount) ? MK_FONT_MGR.OrangeFS() : MK_FONT_MGR.GreenFS();
			MkDecoStr::Convert(L"고딕16", fontState, 0, msg, buffer);
			m_TrooperInfoTag->SetDecoString(buffer);
		}
	}

	void _UpdateBattleInfoControl(void)
	{
		if (m_TargetAgentBtn == NULL)
		{
			if (m_ToBattleBtn != NULL) m_ToBattleBtn->SetVisible(false);
			if (m_CancelBattleBtn != NULL) m_CancelBattleBtn->SetVisible(false);
		}
		else
		{
			if (m_ToBattleBtn != NULL) m_ToBattleBtn->SetVisible(true);
			if (m_CancelBattleBtn != NULL) m_CancelBattleBtn->SetVisible(true);

			const GameAgentGroupInfo& gi = GAME_SYSTEM.GetMasterPlayer().GetAgentGroup();
			unsigned int agentID = m_TargetAgentBtn->GetNodeName().GetString().ToUnsignedInteger();
			bool cbEnable = gi.IsTrooper(agentID);
			bool tbEnable = !cbEnable;
			if (tbEnable)
			{
				tbEnable = (gi.GetCurrTroopers() < GDEF_MAX_TROOP_AGENT_COUNT);
			}

			if (m_ToBattleBtn != NULL) m_ToBattleBtn->SetEnable(tbEnable);
			if (m_CancelBattleBtn != NULL) m_CancelBattleBtn->SetEnable(cbEnable);
		}
	}

protected:

	MkScrollBarNode* m_AListSB;
	MkBaseWindowNode* m_AListRoot;
	MkHashStr m_SortBtnName[eGSM_Max]; // eGSM_Name , eGSM_DescendingLevel, eGSM_AscendingLevel

	MkBaseWindowNode* m_ToBattleBtn;
	MkBaseWindowNode* m_CancelBattleBtn;
	MkSRect* m_TrooperInfoTag;

	MkMap<unsigned int, GameSUI_SmallAgentThumbnail*> m_AgentBtns;
	GameSUI_SmallAgentThumbnail* m_TargetAgentBtn;
};

//------------------------------------------------------------------------------------------------//

bool GamePageBarrack::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_BarrackWindow(GetPageName()), true);

	return true;
}

void GamePageBarrack::Update(const MkTimeState& timeState)
{
	
}

void GamePageBarrack::Clear(MkDataNode* sharingNode)
{
	GamePageIslandBase::Clear(sharingNode);
}

GamePageBarrack::GamePageBarrack(const MkHashStr& name) : GamePageIslandBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
