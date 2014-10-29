
//#include "MkCore_MkGlobalFunction.h"
//#include "MkCore_MkCheck.h"
//#include "MkCore_MkTimeManager.h"

#include "MkCore_MkDataNode.h"

#include "GameSystemManager.h"


//------------------------------------------------------------------------------------------------//

bool GameSystemManager::LoadMasterUserData(const MkPathName& filePath)
{
	MkDataNode userData;
	return (userData.Load(filePath) && GetMasterPlayer().Load(userData));
}

bool GameSystemManager::SaveMasterUserData(const MkPathName& filePath) const
{
	MkDataNode userData;
	if (GetMasterPlayer().Save(userData))
	{
		if (userData.SaveToText(filePath))
			return true;
	}
	return false;
}

bool GameSystemManager::StartBattle(void)
{
	return m_BattleSystem.StartBattle(&GetMasterPlayer());
}

void GameSystemManager::SetDiceSeed(unsigned int seed)
{
	if (seed > 20000)
	{
		seed = static_cast<unsigned int>(GetTickCount()) % 20001; // 0 ~ 20000
	}

	m_BiasedDice.SetSeed(seed);
	m_UniformDice.SetSeed(seed);
}

//------------------------------------------------------------------------------------------------//
