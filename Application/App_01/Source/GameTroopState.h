#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "GameHitPointGage.h"
#include "GameStateType.h"


class GameWizardUnitInfo;
class GameAgentUnitInfo;

class GameTroopState
{
public:

	bool SetUp(const GameWizardUnitInfo* wizard, const MkArray<const GameAgentUnitInfo*>& troopers);

	void UpdateTeamState(void);

	
	GameTroopState();
	~GameTroopState() {}

protected:

	const GameWizardUnitInfo* m_Wizard;
	MkArray<const GameAgentUnitInfo*> m_Troopers;

	GameHitPointGage m_HitPointGage;

	MkHashMap<MkHashStr, IntStateType> m_StateTable;
};