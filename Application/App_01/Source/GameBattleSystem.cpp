
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

	// ���� ally player�� master player
	if (!_AddPlayer(true, masterPlayer))
		return false;

	m_CurrStage = 0;
	m_FinalStage = 4;

	return true;
}

bool GameBattleSystem::StartStage(void)
{
	// ���� ��� �ϳ� ���
	if (!_FillRandomNormalPlayer(false))
		return false;

	// �߰� �ο� ���
	unsigned int playerCount = 0;
	/*
	if ((m_CurrStage != 0) && (m_CurrStage != m_FinalStage))
	{
		MkBiasedDice<unsigned int>& dice = GAME_SYSTEM.GetBiasedDice();
		dice.Clear();
		unsigned int pt = 0xffff;
		for (unsigned int i=0; (i<GDEF_MAX_TEAM_PLAYER_COUNT) && (pt != 0); ++i)
		{
			dice.RegisterID(i, pt); // ally, enemy �ϳ��� �߰� �� Ȯ���� 2/3�� �پ��
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

	// ��� ������ wizard list�� �ҷ���
	MkArray<MkHashStr> wizardKeys;
	if (GameDataNode::WizardSet->GetChildNodeList(wizardKeys) == 0)
		return false;

	// ��ü wizard ���
	MkUniformDice& dice = GAME_SYSTEM.GetUniformDice();
	dice.SetMinMax(0, wizardKeys.GetSize() - 1);

	// �ϳ��� wizard�� ���� 0~2 level �÷� ���
	GameWizardUnitInfo wizardInfo;
	wizardInfo.SetWizardID(wizardKeys[dice.GenerateRandomNumber()].GetString().ToUnsignedInteger());
	dice.SetMinMax(0, 2);
	wizardInfo.SetWizardLevel(basePlayer.GetWizardInfo().GetWizardLevel() + static_cast<int>(dice.GenerateRandomNumber()));
	
	// ��� ������ agent list�� �ҷ���
	MkArray<MkHashStr> agentKeys;
	if (GameDataNode::AgentSet->GetChildNodeList(agentKeys) == 0)
		return false;

	MkArray<GameAgentUnitInfo> agentInfo(GDEF_MAX_TROOP_AGENT_COUNT);
	for (unsigned int i=0; i<GDEF_MAX_TROOP_AGENT_COUNT; ++i)
	{
		// ��ü agent ���
		dice.SetMinMax(0, agentKeys.GetSize() - 1);

		// �ϳ��� agent�� ���� 0~1 level �÷� ���
		GameAgentUnitInfo& currInfo = agentInfo.PushBack();
		currInfo.SetAgentID(agentKeys[dice.GenerateRandomNumber()].GetString().ToUnsignedInteger());
		dice.SetMinMax(0, 1);
		currInfo.SetAgentLevel(basePlayer.GetWizardInfo().GetWizardLevel() + static_cast<int>(dice.GenerateRandomNumber()));
	}

	// ���� ���
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
