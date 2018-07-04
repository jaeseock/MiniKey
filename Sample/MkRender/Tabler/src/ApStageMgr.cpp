
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkInputManager.h"

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

bool ApStageMgr::AddPlayer(const MkHashStr& name, ApPlayer::eStandPosition standPos, ApResourceUnit::eTeamColor teamColor, unsigned int initResSlotSize)
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
	m_LastMainState = eMS_None;
	m_CurrMainState = eMS_Start;
	m_CurrPlayerIndex = 0;
	
	MkTextNode textNode;
	{
		MkPanel& panel = m_StageNode->CreatePanel(L"TurnInfo");
		textNode.SetFontType(L"Large");
		textNode.SetFontStyle(L"AP:Stage:TurnInfo");
		panel.SetTextNode(textNode);
		panel.SetLocalPosition(MkFloat2(50.f, 770.f));
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
	switch (m_CurrMainState)
	{
	case eMS_Start: _UpdateMS_Start(timeState); break;
	case eMS_Play: _UpdateMS_Play(timeState); break;
	case eMS_Result: _UpdateMS_Result(timeState); break;
	case eMS_Exit: _UpdateMS_Exit(timeState); break;
	}

	_UpdateMS_All(timeState);	
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

bool ApStageMgr::_GenerateRandomResource(unsigned int playerIndex, unsigned int count, bool showMsg, const MkTimeState& timeState)
{
	ApPlayer* player = GetPlayer(playerIndex);
	if (player != NULL)
	{
		unsigned int lastResID = m_CurrResourceID;
		for (unsigned int i=0; i<count; ++i)
		{
			if (player->HasEmptyResourceSlot())
			{
				m_ResourceDice.SetMinMax(0, m_Field.GetFieldSize().x - 1);
				unsigned int resRow = m_ResourceDice.GenerateRandomNumber();
				m_ResourceDice.SetMinMax(0, m_Field.GetFieldSize().y - 1);
				unsigned int resColumn = m_ResourceDice.GenerateRandomNumber();
				ApResourceType resType(ApResourceType::eK_Normal, resRow, resColumn);

				if (player->AddResource(m_CurrResourceID, resType, timeState))
				{
					if (showMsg)
					{
						MkStr msg;
						msg.Reserve(64);
						msg += L"[";
						msg += player->GetName();
						msg += L"] 플레이어에게 (";
						msg += resType.GetRow();
						msg += L"-";
						msg += resType.GetColumn();
						msg += L") 자원이 생겼습니다.";
						_SetMessageText(msg, timeState);
					}
					
					MK_DEV_PANEL.MsgToLog(L"[" + player->GetName() + L"] (" + MkStr(m_CurrResourceID) + L") " + MkStr(resType.GetRow()) + L"-" + MkStr(resType.GetColumn()));

					++m_CurrResourceID;
				}
			}
			else
			{
				if (showMsg)
				{
					MkStr msg;
					msg.Reserve(64);
					msg += L"[";
					msg += player->GetName();
					msg += L"] 플레이어의 슬롯이 가득찬 상태입니다.";
					_SetMessageText(msg, timeState);
				}
			}
		}

		if (lastResID < m_CurrResourceID)
		{
			player->UpdateResourceSlotPosition();
			return true;
		}
	}
	return false;
}

void ApStageMgr::_UpdateCurrentTurnInfoText(bool showFlowTime, const MkTimeState& timeState)
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
		msg += m_Players[m_CurrPlayerIndex].GetName();
		msg += L"] 님의 차례.";

		if (showFlowTime && (m_FlowCounter.GetTickCount() > 0.))
		{
			msg += L" > ";
			float remainTime = static_cast<float>(m_FlowCounter.GetTickCount() - m_FlowCounter.GetTickTime(timeState));
			msg += remainTime;
		}

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
	if (m_LastMainState == eMS_None)
	{
		// 시작 자원
		MK_INDEXING_LOOP(m_Players, i)
		{
			unsigned int initResCount = m_Players[i].GetMaxResourceSlotSize() / 2;
			_GenerateRandomResource(i, initResCount, false, timeState);
		}

		// start
		_SetMessageText(L"환영합니다. 잠시 후 게임이 시작됩니다.", timeState);

		m_FlowCounter.SetUp(timeState, 3.);
	}
	else
	{
		if (m_FlowCounter.OnTick(timeState))
		{
			m_CurrMainState = eMS_Play;
			m_LastPlayerState = ePS_None;
			m_CurrPlayerState = ePS_GenerateResource;

			m_FlowCounter.SetUp(timeState, 0.);
		}
	}

	_UpdateCurrentTurnInfoText(false, timeState);
}

void ApStageMgr::_UpdateMS_Play(const MkTimeState& timeState)
{
	switch (m_CurrPlayerState)
	{
	case ePS_GenerateResource: _UpdatePS_GenerateResource(timeState); break;
	case ePS_Action: _UpdatePS_Action(timeState); break;
	case ePS_ApplyEvent: _UpdatePS_ApplyEvent(timeState); break;
	case ePS_Next: _UpdatePS_Next(timeState); break;
	}

	m_LastPlayerState = m_CurrPlayerState;
}

void ApStageMgr::_UpdateMS_Result(const MkTimeState& timeState)
{
}

void ApStageMgr::_UpdateMS_Exit(const MkTimeState& timeState)
{
}

void ApStageMgr::_UpdateMS_All(const MkTimeState& timeState)
{
	m_Field.Update(timeState);

	MK_INDEXING_LOOP(m_Players, i)
	{
		m_Players[i].Update(timeState);
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

	m_LastMainState = m_CurrMainState;
}

void ApStageMgr::_UpdatePS_GenerateResource(const MkTimeState& timeState)
{
	if (m_LastPlayerState == ePS_None)
	{
		_GenerateRandomResource(m_CurrPlayerIndex, 1, true, timeState);

		ApPlayer* currPlayer = GetPlayer(m_CurrPlayerIndex);
		if (currPlayer->GetStandPosition() == ApPlayer::eSP_Down)
		{
			m_FlowCounter.SetUp(timeState, 10.); // 나는 10초
		}
		else
		{
			m_FlowCounter.SetUp(timeState, 2.); // ai는 2초
		}
		
		m_CurrPlayerState = ePS_Action;
	}
}

void ApStageMgr::_UpdatePS_Action(const MkTimeState& timeState)
{
	ApPlayer* currPlayer = GetPlayer(m_CurrPlayerIndex);

	if (m_FlowCounter.OnTick(timeState))
	{
		currPlayer->ClearSelection();

		m_FlowCounter.SetUp(timeState, 0.);
	}
	else
	{
		// 나야나
		if (currPlayer->GetStandPosition() == ApPlayer::eSP_Down)
		{
			// keyboard
			if (MK_INPUT_MGR.GetKeyPressed(L'0'))
			{
				currPlayer->SelectResource(9);
			}
			for (wchar_t key = L'1'; key <= L'9'; ++key)
			{
				if (MK_INPUT_MGR.GetKeyPressed(static_cast<unsigned int>(key)))
				{
					currPlayer->SelectResource(static_cast<unsigned int>(key - L'1'));
				}
			}
			if (MK_INPUT_MGR.GetKeyPressed(VK_OEM_MINUS))
			{
				currPlayer->SelectResource(10);
			}
			if (MK_INPUT_MGR.GetKeyPressed(VK_OEM_PLUS))
			{
				currPlayer->SelectResource(11);
			}

			// mouse
			if (MK_INPUT_MGR.GetMouseLeftButtonPressed() || MK_INPUT_MGR.GetMouseLeftButtonDoubleClicked())
			{
				MkInt2 hitPos = MK_INPUT_MGR.GetAbsoluteMousePosition(true);
				currPlayer->SelectResource(MkFloat2(static_cast<float>(hitPos.x), static_cast<float>(hitPos.y)));
			}

			// cheat : bonus resource
			if (MK_INPUT_MGR.GetKeyPressed(VK_SPACE))
			{
				_GenerateRandomResource(m_CurrPlayerIndex, 1, true, timeState);
			}
		}
		// 다른 플레이어
		else
		{
		}
	}

	_UpdateCurrentTurnInfoText(true, timeState);
}

void ApStageMgr::_UpdatePS_ApplyEvent(const MkTimeState& timeState)
{
}

void ApStageMgr::_UpdatePS_Next(const MkTimeState& timeState)
{
}

//------------------------------------------------------------------------------------------------//