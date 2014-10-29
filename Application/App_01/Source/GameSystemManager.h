#pragma once


//------------------------------------------------------------------------------------------------//
// text ют╥б©К edit box
//------------------------------------------------------------------------------------------------//

//#include "MkCore_MkHashStr.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkBiasedDice.h"
#include "MkCore_MkUniformDice.h"

#include "GameBattleSystem.h"


#define GAME_SYSTEM GameSystemManager::Instance()


//------------------------------------------------------------------------------------------------//

class MkPathName;

class GameSystemManager : public MkSingletonPattern<GameSystemManager>
{
public:

	inline GameMasterPlayer& GetMasterPlayer(void) { return m_MasterPlayer; }
	inline const GameMasterPlayer& GetMasterPlayer(void) const { return m_MasterPlayer; }

	bool LoadMasterUserData(const MkPathName& filePath);
	bool SaveMasterUserData(const MkPathName& filePath) const;

	// battle
	inline GameBattleSystem& GetBattleSystem(void) { return m_BattleSystem; }

	bool StartBattle(void);

	// dice
	void SetDiceSeed(unsigned int seed = 0xffffffff);
	inline MkBiasedDice<unsigned int>& GetBiasedDice(void) { return m_BiasedDice; }
	inline MkUniformDice& GetUniformDice(void) { return m_UniformDice; }

	GameSystemManager() {}
	virtual ~GameSystemManager() {}

protected:

	GameMasterPlayer m_MasterPlayer;

	GameBattleSystem m_BattleSystem;

	MkBiasedDice<unsigned int> m_BiasedDice;
	MkUniformDice m_UniformDice;
};

//------------------------------------------------------------------------------------------------//