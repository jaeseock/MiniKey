
//#include "MkCore_MkDataNode.h"

#include "GameGlobalDefinition.h"
#include "GameWizardUnitInfo.h"
#include "GameAgentUnitInfo.h"
#include "GameTroopState.h"


//------------------------------------------------------------------------------------------------//

bool GameTroopState::SetUp(const GameWizardUnitInfo* wizard, const MkArray<const GameAgentUnitInfo*>& troopers)
{
	m_Wizard = wizard;
	m_Troopers = troopers;
	//MkDataNode initNode;

	return true;
}

void GameTroopState::UpdateTeamState(void)
{
	m_HitPointGage.UpdateGage();

	MkHashMapLooper<MkHashStr, IntStateType> looper(m_StateTable);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField().UpdateState();
	}
}

GameTroopState::GameTroopState()
{
	m_Wizard = NULL;

	// base
	m_HitPointGage.Initialize(1000);

	// resource
	m_StateTable.Create(GameStateName::Team::R_ExtractResCD).Initialize(5, 1, 0xffff);
	m_StateTable.Create(GameStateName::Team::R_BoostExtraction).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::R_BonusResPerc).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::R_VoidTypeResAppPerc).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::R_ComboLevForBonusRes).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::R_BonusResPercByCritical).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::R_PurgeSingleResource).Initialize(0, 0, 1);
	m_StateTable.Create(GameStateName::Team::R_ResourceSlotSize).Initialize(5, 0, 8);

	// attack
	m_StateTable.Create(GameStateName::Team::A_AttackPowerRate).Initialize(100, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::A_BonusAttackPower).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::A_CriticalPerc).Initialize(5, 0, 100);
	m_StateTable.Create(GameStateName::Team::A_CriticalPowerRate).Initialize(30, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::A_PowerPenetration).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::A_BustDurability).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::A_ComboPowerBonus20).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::A_ComboPowerBonus30).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::A_ComboPowerBonus32).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::A_ComboPowerBonus40).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::A_ComboPowerBonus50).Initialize(0, 0, 0xffff);

	// defense
	m_StateTable.Create(GameStateName::Team::D_ImpactResistance).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::D_DefensePower).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::D_ProtectDurability).Initialize(25, 0, 100);
	m_StateTable.Create(GameStateName::Team::D_UnionDefense).Initialize(10, 0, 100);
	m_StateTable.Create(GameStateName::Team::D_CriticalResistance).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::D_ComboPowerResist20).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::D_ComboPowerResist30).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::D_ComboPowerResist32).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::D_ComboPowerResist40).Initialize(0, 0, 100);
	m_StateTable.Create(GameStateName::Team::D_ComboPowerResist50).Initialize(0, 0, 100);

	// support
	m_StateTable.Create(GameStateName::Team::S_SupplimentCapacity).Initialize(6, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::S_UnitTransformTime).Initialize(3, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::S_BonusSkillAccCount).Initialize(1, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::S_BonusHealPower).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::S_BonusHealRate).Initialize(0, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::S_HitPointRestorationForNextStage).Initialize(50, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::S_DurationRestorationForNextStage).Initialize(50, 0, 0xffff);
	m_StateTable.Create(GameStateName::Team::S_ViewEnemyResourcePool).Initialize(0, 0, 1);
	m_StateTable.Create(GameStateName::Team::S_ViewEnemySkillAccum).Initialize(0, 0, 1);
	m_StateTable.Create(GameStateName::Team::S_ViewStageReward).Initialize(0, 0, 2);
	m_StateTable.Create(GameStateName::Team::S_ViewHiddenPath).Initialize(0, 0, 2);
}

//------------------------------------------------------------------------------------------------//
