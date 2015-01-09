#pragma once

#include "MkCore_MkHashStr.h"

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

enum eResourceType
{
	eRT_None = -1,
	eRT_Red = 0, // amplification, attack
	eRT_Green, // stability, defense
	eRT_Violet, // vitality, support
	eRT_Blue, // transformation, resource
	eRT_Void,

	eRT_Max
};

enum eResourceCharacteristic
{
	eRC_None = -1,
	eRC_Amplification = 0, // red, attack
	eRC_Stability, // green, defense
	eRC_Vitality, // violet, support
	eRC_Transformation, // blue, resource

	eRC_Max
};

enum eTroopMemberType
{
	eTMT_None = -1,
	eTMT_Attack = 0, // red, amplification
	eTMT_Defense, // green, stability
	eTMT_Support, // violet, vitality
	eTMT_Resource, // blue, transformation

	eTMT_Max
};

enum eResComboType
{
	eRCT_None = -1,
	eRCT_20 = 0,
	eRCT_30,
	eRCT_32,
	eRCT_40,
	eRCT_50,

	eRCT_Max
};

enum eGroupSortingMethod
{
	eGSM_Name = 0,
	eGSM_DescendingLevel,
	eGSM_AscendingLevel,

	eGSM_Max
};

enum eShowTroopInfoLevel
{
	eSTIL_None = 0,
	eSTIL_Little,
	eSTIL_Low,
	eSTIL_Medium,
	eSTIL_High,
	eSTIL_All
};

class GamePageName
{
public:
	static const MkHashStr AppRoot;
	static const MkHashStr ClientStart;
	static const MkHashStr GameRoot;
	static const MkHashStr IslandRoot;
	static const MkHashStr MainTitle;
	static const MkHashStr IslandAgora;
	static const MkHashStr WizardLab;
	static const MkHashStr Barrack;
	static const MkHashStr TrainRoom;
	static const MkHashStr SupplyDepot;
	static const MkHashStr BattleRoot;
	static const MkHashStr BattlePhase;
};

class GameStateName
{
public:

	class Troop
	{
	public:

		// attack
		static const MkHashStr A_BonusAttackPower; // 추가 공격력
		static const MkHashStr A_AttackPowerRate; // 공격 적용률
		static const MkHashStr A_OnPairBonusAP; // one-pair 조합의 추가 공격력
		static const MkHashStr A_TrippleBonusAP; // tripple 조합의 추가 공격력
		static const MkHashStr A_FullHouseBonusAP; // full-house 조합의 추가 공격력
		static const MkHashStr A_FourCardBonusAP; // 4-card 조합의 추가 공격력
		static const MkHashStr A_FiveCardBonusAP; // 5-card 조합의 추가 공격력
		static const MkHashStr A_CriticalDamageRate; // 최종 데미지에서 치명손상으로 인정되는 비율
		static const MkHashStr A_DrainHPAtFourCard; // 4-card 조합시 최종 데미지의 일정량을 일반회복
		static const MkHashStr A_DrainHPAtFiveCard; // 5-card 조합시 최종 데미지의 일정량을 일반회복
		static const MkHashStr A_DecayEnemyHPt; // 최종 데미지의 일정량만큼 상대의 일반 회복점수 하락

		// defense
		static const MkHashStr D_DamageResistance; // % 저항력(dam = AP - AP * DamageResistance)
		static const MkHashStr D_DefensePower; // - 방어력(dam = AP - DefensePower)
		static const MkHashStr D_BonusDRAtRest; // 대기시 추가 저항력
		static const MkHashStr D_Toughness; // 패배시 추가 방어력
		static const MkHashStr D_CritRecoveryAtFourCard; // 4-card 조합시 치명손상 복구량
		static const MkHashStr D_CritRecoveryAtFiveCard; // 5-card 조합시 치명손상 복구량

		// support
		static const MkHashStr S_SupplimentCapacity; // 인구수
		static const MkHashStr S_BonusTransformTime; // 추가 유닛 소환/업글 시간
		static const MkHashStr S_HealPower; // 일반 회복량
		static const MkHashStr S_PtToGainHeal; // 일반회복 얻는 점수
		static const MkHashStr S_HPtAtRest; // 휴식시 얻는 일반 회복점수
		static const MkHashStr S_HPtForEveryTurn; // 턴당 얻는 일반 회복점수
		static const MkHashStr S_HPtAtLoss; // 패배시 얻는 일반 회복점수
		static const MkHashStr S_ViewEnemyInfo; // 상대 부대 정보공개 단계
		static const MkHashStr S_WinnerBonusAPR; // 승리시 추가 공격 적용률

		// resource
		static const MkHashStr R_RedResWeight; // red 자원 발생비중
		static const MkHashStr R_GreenResWeight; // green 자원 발생비중
		static const MkHashStr R_VioletResWeight; // violet 자원 발생비중
		static const MkHashStr R_BlueResWeight; // blue 자원 발생비중
		static const MkHashStr R_VoidResWeight; // 부정형 자원 발생비중
		static const MkHashStr R_PtToGainRes; // 자원 얻는 점수
		static const MkHashStr R_RPtAtRest; // 휴식시 얻는 자원점수
		static const MkHashStr R_RPtForEveryTurn; // 턴당 얻는 자원점수
		static const MkHashStr R_ResSlotSize; // 자원 슬롯 크기
		static const MkHashStr R_OnPairPower; // one-pair 조합의 power
		static const MkHashStr R_TripplePower; // tripple 조합의 power
		static const MkHashStr R_FullHousePower; // full-house 조합의 power
		static const MkHashStr R_FourCardPower; // 4-card 조합의 power
		static const MkHashStr R_FiveCardPower; // 5-card 조합의 power
		static const MkHashStr R_DecayEnemyRPtAtFourCard; // 4-card 조합시 상대 자원점수 감소량
		static const MkHashStr R_DecayEnemyRPtAtFiveCard; // 5-card 조합시 상대 자원점수 감소량
	};
};

#define GDEF_DEF_USER_DATA_PATH L"DataNode\\DefaultUserData.txt"
#define GDEF_LAST_SAVE_DATA_PATH L"UserData\\LastSaveFile.txt"

#define GDEF_USER_SAVE_DATA_EXT L"txt"

#define GDEF_MAX_AGENT_COUNT 0xff
#define GDEF_MAX_AGENT_LEVEL 0xff

#define GDEF_MAX_TEAM_PLAYER_COUNT 3
#define GDEF_MAX_TROOP_AGENT_COUNT 8