
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkTextNode.h"

#include "ApStaticDefinition.h"
#include "ApStageMgr.h"


//------------------------------------------------------------------------------------------------//

bool ApStageMgr::SetField(const MkUInt2& size, MkSceneNode* rootNode)
{
	MK_CHECK(rootNode != NULL, L"root node가 NULL임")
		return false;

	m_StageNode = rootNode->CreateChildNode(L"<Stage>");
	MK_CHECK(m_StageNode != NULL, L"m_StageNode가 NULL임")
		return false;
	m_StageNode->SetLocalDepth(MKDEF_PA_MAX_WORLD_DEPTH * 0.5f);

	// bg
	MkPanel& bgPanel = m_StageNode->CreatePanel(L"BG");
	bgPanel.SetTexture(L"Image\\stage_bg.png");
	bgPanel.SetLocalDepth(1.f);

	// field
	MkUInt2 fieldSize;
	fieldSize.x = Clamp<unsigned int>(size.x, ApStaticDefinition::Instance().GetMinFieldSize().x, ApStaticDefinition::Instance().GetMaxFieldSize().x);
	fieldSize.y = Clamp<unsigned int>(size.x, ApStaticDefinition::Instance().GetMinFieldSize().y, ApStaticDefinition::Instance().GetMaxFieldSize().y);

	if (!m_Field.SetUp(fieldSize, m_StageNode))
		return false;

	// player
	MkSceneNode* playerNode = m_StageNode->CreateChildNode(L"<Player>");
	MK_CHECK(playerNode != NULL, L"playerNode가 NULL임")
		return false;

	return true;
}

bool ApStageMgr::AddPlayer(const MkHashStr& name, ApPlayer::eStandPosition standPos, ApResourceUnit::eTeamColor teamColor, unsigned int initResSlotSize, unsigned int initResCount)
{
	MkSceneNode* playerNode = m_StageNode->GetChildNode(L"<Player>");
	if (playerNode == NULL)
		return false;

	// player 객체 생성
	initResSlotSize = Clamp<unsigned int>(initResSlotSize, ApStaticDefinition::Instance().GetDefaultResourceSlotSize(), ApStaticDefinition::Instance().GetMaxResourceSlotSize());
	unsigned int playerIndex = m_Players.GetSize();
	ApPlayer& player = m_Players.PushBack();
	if (!player.SetUp(playerIndex, name, standPos, teamColor, initResSlotSize, playerNode))
		return false;

	// 시작 자원
	initResCount = GetMin<unsigned int>(initResCount, initResSlotSize);
	for (unsigned int i=0; i<initResCount; ++i)
	{
		_GenerateResource(playerIndex);
	}

	player.UpdateSlotPosition();
	return true;
}

ApPlayer* ApStageMgr::GetPlayer(unsigned int index)
{
	return m_Players.IsValidIndex(index) ? &m_Players[index] : NULL;
}

void ApStageMgr::SetFlowInfo(unsigned int turnPerRound, unsigned int maxRound)
{
	m_TurnPerRound = GetMax<unsigned int>(1, turnPerRound);
	m_MaxRound = GetMax<unsigned int>(1, maxRound);

	m_CurrTurn = 0;
	m_MainState = eMS_Start;
	//m_TurnState = eTS_Start;

	MkTextNode textNode;
	{
		MkPanel& panel = m_StageNode->CreatePanel(L"TurnInfo");
		textNode.SetFontType(L"Large");
		textNode.SetFontStyle(L"AP:Stage:TurnInfo");
		panel.SetTextNode(textNode);
		panel.SetLocalPosition(MkFloat2(50.f, 770.f));

		_UpdateCurrentTurnInfoText(0);
	}
	{
		MkPanel& panel = m_StageNode->CreatePanel(L"Msg");
		textNode.SetFontType(L"Large");
		textNode.SetFontStyle(L"AP:Stage:Msg");
		panel.SetTextNode(textNode);
		panel.SetLocalPosition(MkFloat2(50.f, 744.f));
	}
}

void ApStageMgr::Update(const MkTimeState& timeState)
{
	switch (m_MainState)
	{
	case eMS_Start: _UpdateMS_Start(timeState); break;
	case eMS_Welcome: _UpdateMS_Welcome(timeState); break;
	case eMS_Play: _UpdateMS_Play(timeState); break;
	case eMS_Result: _UpdateMS_Result(timeState); break;
	case eMS_Exit: _UpdateMS_Exit(timeState); break;
	}

	// msg
	MkPanel* panel = m_StageNode->GetPanel(L"Msg");
	if ((panel != NULL) && panel->GetVisible())
	{
		if (m_MsgCounter.OnTick(timeState))
		{
			panel->SetVisible(false);
		}
	}
}

void ApStageMgr::Clear(void)
{
	m_Players.Clear();

	m_CurrResourceID = 0;
}

ApStageMgr::ApStageMgr()
{
	m_CurrResourceID = 0;
}

//------------------------------------------------------------------------------------------------//

bool ApStageMgr::_GenerateResource(unsigned int index)
{
	ApPlayer* player = GetPlayer(index);
	if (player != NULL)
	{
		if (player->HasEmptyResourceSlot())
		{
			m_ResourceDice.SetMinMax(0, m_Field.GetFieldSize().x - 1);
			unsigned int resRow = m_ResourceDice.GenerateRandomNumber();
			m_ResourceDice.SetMinMax(0, m_Field.GetFieldSize().y - 1);
			unsigned int resColumn = m_ResourceDice.GenerateRandomNumber();
			ApResourceType resType(ApResourceType::eK_Normal, resRow, resColumn);

			if (player->AddResource(m_CurrResourceID, resType))
			{
				MK_DEV_PANEL.MsgToLog(L"Generate res to " + player->GetName() + L" (" + MkStr(m_CurrResourceID) + L") " + MkStr(resType.GetRow()) + L"*" + MkStr(resType.GetColumn()));

				++m_CurrResourceID;
				return true;
			}
		}
	}
	return false;
}

void ApStageMgr::_UpdateCurrentTurnInfoText(unsigned int currPlayer)
{
	MkPanel* panel = m_StageNode->GetPanel(L"TurnInfo");
	if (panel != NULL)
	{
		unsigned int currTurn = m_CurrTurn % m_TurnPerRound;
		unsigned int currRound = m_CurrTurn / m_TurnPerRound;

		MkStr msg;
		msg.Reserve(64);
		msg += L"(";
		msg += (currTurn + 1);
		msg += L")턴 (";
		msg += (currRound + 1);
		msg += L"/";
		msg += m_MaxRound;
		msg += L")라운드, [";
		msg += m_Players[currPlayer].GetName();
		msg += L"] 님의 턴";

		panel->GetTextNodePtr()->SetText(msg);
		panel->BuildAndUpdateTextCache();
	}
}

void ApStageMgr::_SetMessageText(const MkStr& msg, const MkTimeState& timeState)
{
	MkPanel* panel = m_StageNode->GetPanel(L"Msg");
	if (panel != NULL)
	{
		panel->SetVisible(true);
		panel->GetTextNodePtr()->SetText(msg);
		panel->BuildAndUpdateTextCache();

		m_MsgCounter.SetUp(timeState, 3.);
	}
}

void ApStageMgr::_UpdateMS_Start(const MkTimeState& timeState)
{
	_SetMessageText(L"환영합니다. 잠시 후 게임이 시작됩니다.", timeState);

	m_FlowCounter.SetUp(timeState, 2.);
	m_MainState = eMS_Welcome;
}

void ApStageMgr::_UpdateMS_Welcome(const MkTimeState& timeState)
{
}

void ApStageMgr::_UpdateMS_Play(const MkTimeState& timeState)
{
}

void ApStageMgr::_UpdateMS_Result(const MkTimeState& timeState)
{
}

void ApStageMgr::_UpdateMS_Exit(const MkTimeState& timeState)
{
}

//------------------------------------------------------------------------------------------------//