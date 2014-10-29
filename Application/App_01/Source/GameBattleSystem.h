#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "GamePlayerSystem.h"
#include "GameTroopState.h"

class GameBattleSystem
{
public:

	bool StartBattle(const GamePlayerBase* masterPlayer);

	bool StartStage(void);

	void UpdateAllTroopState(void);

	bool EndOfStage(void);

	void Clear(void);
	
	GameBattleSystem();
	~GameBattleSystem() { Clear(); }

protected:

	bool _AddPlayer(bool ally, const GamePlayerBase* player);

	bool _FillRandomNormalPlayer(GameNormalPlayer& buffer, const GameNormalPlayer& basePlayer);
	bool _FillRandomNormalPlayer(bool ally);

	void _ClearTroopState(bool allPlayer);

protected:

	int m_CurrStage;
	int m_FinalStage;

	MkArray<GameNormalPlayer> m_AllyPlayer;
	MkArray<GameNormalPlayer> m_EnemyPlayer;

	GameTroopState m_AllyTroop[GDEF_MAX_TEAM_PLAYER_COUNT];
	GameTroopState m_EnemyTroop[GDEF_MAX_TEAM_PLAYER_COUNT];
};
