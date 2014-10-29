
#include "MkCore_MkDataNode.h"

//#include "GameWizardUnitInfo.h"
#include "GameDataNode.h"
#include "GameSystemManager.h"
#include "GameBattleSystem.h"


//------------------------------------------------------------------------------------------------//

bool GameBattleSystem::StartBattle(const GamePlayerBase* masterPlayer)
{
	if (masterPlayer == NULL)
		return false;

	// 최초 ally player는 master player
	if (!_AddPlayer(true, masterPlayer))
		return false;

	m_CurrStage = 0;
	m_FinalStage = 4;

	return true;
}

bool GameBattleSystem::StartStage(void)
{
	// 랜덤 상대 하나 등록
	if (!_FillRandomNormalPlayer(false))
		return false;

	// 추가 인원 등록
	unsigned int playerCount = 0;
	/*
	if ((m_CurrStage != 0) && (m_CurrStage != m_FinalStage))
	{
		MkBiasedDice<unsigned int>& dice = GAME_SYSTEM.GetBiasedDice();
		dice.Clear();
		unsigned int pt = 0xffff;
		for (unsigned int i=0; (i<GDEF_MAX_TEAM_PLAYER_COUNT) && (pt != 0); ++i)
		{
			dice.RegisterID(i, pt); // ally, enemy 하나씩 추가 될 확률이 2/3씩 줄어듬
			pt /= 3;
		}
		dice.UpdatePoints();
		dice.GenerateRandomID(playerCount);
	}
	*/

	if (playerCount > 0)
	{
		for (unsigned int i=0; i<playerCount; ++i)
		{
			if (!_FillRandomNormalPlayer(true))
				return false;

			if (!_FillRandomNormalPlayer(false))
				return false;
		}
	}
	
	return false;
}

void GameBattleSystem::UpdateAllTroopState(void)
{
	for (unsigned int i=0; i<m_AllyPlayer.GetSize(); ++i)
	{
		m_AllyTroop[i].UpdateTroopState();
	}

	for (unsigned int i=0; i<m_EnemyPlayer.GetSize(); ++i)
	{
		m_EnemyTroop[i].UpdateTroopState();
	}
}

bool GameBattleSystem::EndOfStage(void)
{
	if (m_CurrStage == m_FinalStage)
	{
		_ClearTroopState(true);
	}
	else
	{
		++m_CurrStage;

		_ClearTroopState(false);

		return StartStage();
	}

	return true;
}

void GameBattleSystem::Clear(void)
{
	_ClearTroopState(true);
}

GameBattleSystem::GameBattleSystem()
{
	m_AllyPlayer.Reserve(GDEF_MAX_TEAM_PLAYER_COUNT);
	m_EnemyPlayer.Reserve(GDEF_MAX_TEAM_PLAYER_COUNT);
}

//------------------------------------------------------------------------------------------------//

bool GameBattleSystem::_AddPlayer(bool ally, const GamePlayerBase* player)
{
	MkArray<GameNormalPlayer>& playerList = (ally) ? m_AllyPlayer : m_EnemyPlayer;
	unsigned int index = playerList.GetSize();

	GameNormalPlayer& normalPlayer = playerList.PushBack();
	normalPlayer.SetUp(player->GetWizardInfo(), player->GetAgentInfo());

	GameTroopState& troopState = (ally) ? m_AllyTroop[index] : m_EnemyTroop[index];
	return troopState.SetUp(&normalPlayer);
}

bool GameBattleSystem::_FillRandomNormalPlayer(GameNormalPlayer& buffer, const GameNormalPlayer& basePlayer)
{
	if ((GameDataNode::WizardSet == NULL) || (GameDataNode::AgentSet == NULL))
		return false;

	// 사용 가능한 wizard list를 불러옴
	MkArray<MkHashStr> wizardKeys;
	if (GameDataNode::WizardSet->GetChildNodeList(wizardKeys) == 0)
		return false;

	// 전체 wizard 대상
	MkUniformDice& dice = GAME_SYSTEM.GetUniformDice();
	dice.SetMinMax(0, wizardKeys.GetSize() - 1);

	// 하나의 wizard를 택해 0~2 level 올려 사용
	GameWizardUnitInfo wizardInfo;
	wizardInfo.SetWizardID(wizardKeys[dice.GenerateRandomNumber()].GetString().ToUnsignedInteger());
	dice.SetMinMax(0, 2);
	wizardInfo.SetWizardLevel(basePlayer.GetWizardInfo().GetWizardLevel() + static_cast<int>(dice.GenerateRandomNumber()));
	
	// 사용 가능한 agent list를 불러옴
	MkArray<MkHashStr> agentKeys;
	if (GameDataNode::AgentSet->GetChildNodeList(agentKeys) == 0)
		return false;

	MkArray<GameAgentUnitInfo> agentInfo(GDEF_MAX_TROOP_AGENT_COUNT);
	for (unsigned int i=0; i<GDEF_MAX_TROOP_AGENT_COUNT; ++i)
	{
		// 전체 agent 대상
		dice.SetMinMax(0, agentKeys.GetSize() - 1);

		// 하나의 agent를 택해 0~1 level 올려 사용
		GameAgentUnitInfo& currInfo = agentInfo.PushBack();
		currInfo.SetAgentID(agentKeys[dice.GenerateRandomNumber()].GetString().ToUnsignedInteger());
		dice.SetMinMax(0, 1);
		currInfo.SetAgentLevel(basePlayer.GetWizardInfo().GetWizardLevel() + static_cast<int>(dice.GenerateRandomNumber()));
	}

	// 최종 등록
	buffer.SetUp(wizardInfo, agentInfo);
	return true;
}

bool GameBattleSystem::_FillRandomNormalPlayer(bool ally)
{
	GameNormalPlayer npBuffer;
	return _FillRandomNormalPlayer(npBuffer, m_AllyPlayer[0]) ? _AddPlayer(ally, &npBuffer) : false;
}

void GameBattleSystem::_ClearTroopState(bool allPlayer)
{
	m_AllyPlayer.Erase(MkArraySection((allPlayer) ? 0 : 1));
	m_EnemyPlayer.Flush();

	for (unsigned int i=(allPlayer) ? 0 : 1; i<GDEF_MAX_TEAM_PLAYER_COUNT; ++i)
	{
		m_AllyTroop[i].Clear();
	}

	for (unsigned int i=0; i<GDEF_MAX_TEAM_PLAYER_COUNT; ++i)
	{
		m_EnemyTroop[i].Clear();
	}
}

//------------------------------------------------------------------------------------------------//
