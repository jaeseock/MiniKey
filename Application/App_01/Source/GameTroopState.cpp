
#include "MkCore_MkDataNode.h"

#include "GameGlobalDefinition.h"
#include "GamePlayerSystem.h"
//#include "GameWizardUnitInfo.h"
//#include "GameAgentUnitInfo.h"
#include "GameDataNode.h"
#include "GameTroopState.h"


//------------------------------------------------------------------------------------------------//

bool GameTroopState::SetUp(const GameNormalPlayer* player)
{
	m_Player = player;
	if (m_Player == NULL)
		return false;

	// wizard
	unsigned int wizardID = m_Player->GetWizardInfo().GetWizardID();
	MkHashStr wizardKey = MkStr(wizardID);
	const MkDataNode* wizardData = GameDataNode::WizardSet->GetChildNode(wizardKey);
	if (wizardData == NULL)
		return false;

	const MkDataNode* baseState = wizardData->GetChildNode(L"BaseState");

	int wizardLvIndex = m_Player->GetWizardInfo().GetWizardLevel() - 1;
	
	int baseHP = 0;
	wizardData->GetData(L"BaseHP", baseHP, 0);

	int levelHP = 0;
	wizardData->GetData(L"LevelHP", levelHP, 0);

	// base
	m_HitPointGage.Initialize(baseHP + wizardLvIndex * levelHP);

	// attack
	_SetState(baseState, GameStateName::Troop::A_BonusAttackPower, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::A_AttackPowerRate, 100, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::A_OnPairBonusAP, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::A_TrippleBonusAP, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::A_FullHouseBonusAP, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::A_FourCardBonusAP, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::A_FiveCardBonusAP, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::A_CriticalDamageRate, 20, 1, 100);
	_SetState(baseState, GameStateName::Troop::A_DrainHPAtFourCard, 0, 0, 100);
	_SetState(baseState, GameStateName::Troop::A_DrainHPAtFiveCard, 0, 0, 100);
	_SetState(baseState, GameStateName::Troop::A_DecayEnemyHPt, 0, 0, 100);

	// defense
	_SetState(baseState, GameStateName::Troop::D_DamageResistance, 5, 0, 100);
	_SetState(baseState, GameStateName::Troop::D_DefensePower, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::D_BonusDRAtRest, 5, 0, 100);
	_SetState(baseState, GameStateName::Troop::D_Toughness, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::D_CritRecoveryAtFourCard, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::D_CritRecoveryAtFiveCard, 0, 0, 0xffff);

	// support
	_SetState(baseState, GameStateName::Troop::S_SupplimentCapacity, 4, 0, 20);
	_SetState(baseState, GameStateName::Troop::S_BonusTransformTime, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::S_HealPower, 100, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::S_PtToGainHeal, 100, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::S_HPtAtRest, 15, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::S_HPtForEveryTurn, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::S_HPtAtLoss, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::S_ViewEnemyInfo, eSTIL_None, eSTIL_None, eSTIL_All);
	_SetState(baseState, GameStateName::Troop::S_WinnerBonusAPR, 100, 0, 0xffff);

	// resource
	_SetState(baseState, GameStateName::Troop::R_RedResWeight, 100, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_GreenResWeight, 100, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_VioletResWeight, 100, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_BlueResWeight, 100, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_VoidResWeight, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_PtToGainRes, 100, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_RPtAtRest, 10, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_RPtForEveryTurn, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_ResSlotSize, 5, 0, 8);
	_SetState(baseState, GameStateName::Troop::R_OnPairPower, 10, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_TripplePower, 20, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_FullHousePower, 40, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_FourCardPower, 45, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_FiveCardPower, 60, 1, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_DecayEnemyRPtAtFourCard, 0, 0, 0xffff);
	_SetState(baseState, GameStateName::Troop::R_DecayEnemyRPtAtFiveCard, 0, 0, 0xffff);

	return true;
}

void GameTroopState::Clear(void)
{
	m_Player = NULL;
	m_HitPointGage.Clear();
	m_StateTable.Clear();
}

void GameTroopState::UpdateTroopState(void)
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
	m_Player = NULL;
}

//------------------------------------------------------------------------------------------------//

void GameTroopState::_SetState(const MkDataNode* baseState, const MkHashStr& key, int defValue, int minCap, int maxCap)
{
	int attribute = 0;
	if ((baseState != NULL) && baseState->IsValidKey(key))
	{
		baseState->GetData(key, attribute, 0);
	}

	m_StateTable.Create(key).Initialize(defValue + attribute, minCap, maxCap);
}

//------------------------------------------------------------------------------------------------//
