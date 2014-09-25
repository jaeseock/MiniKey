#pragma once

#include "MkCore_MkHashStr.h"

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

enum eResourceType
{
	eRT_None = -1,
	eRT_Red = 0, // amplification, attack
	eRT_Green, // stability, defense
	eRT_White, // vitality, support
	eRT_Blue, // transformation, resource
	eRT_Void,

	eRT_Max
};

enum eResourceCharacteristic
{
	eRC_None = -1,
	eRC_Amplification = 0, // red, attack
	eRC_Stability, // green, defense
	eRC_Vitality, // white, support
	eRC_Transformation, // blue, resource

	eRC_Max
};

enum eTroopMemberType
{
	eTMT_None = -1,
	eTMT_Attack = 0, // red, amplification
	eTMT_Defense, // green, stability
	eTMT_Support, // white, vitality
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
	eGSM_AgentName = 0,
	eGSM_AgentLevel,
	eGSM_AttackLevel,
	eGSM_DefenseLevel,
	eGSM_SupportLevel,
	eGSM_ResourceLevel,
	eGSM_AllRoundLevel
};

class GamePageName
{
public:
	static const MkHashStr AppRoot;
	static const MkHashStr ClientStart;
	static const MkHashStr GameRoot;
	static const MkHashStr MainTitle;
	static const MkHashStr IslandAgora;
	static const MkHashStr WizardLab;
	static const MkHashStr Barrack;
};

class GameStateName
{
public:

	class Team
	{
	public:
		// resource
		static const MkHashStr R_ExtractResCD; // 자원 추출 쿨다운
		static const MkHashStr R_BoostExtraction; // 자원 추출 쿨다운 가속
		static const MkHashStr R_BonusResPerc; // 추가 자원 추출 확률
		static const MkHashStr R_VoidTypeResAppPerc; // 부정형 자원 등장 확률
		static const MkHashStr R_ComboLevForBonusRes; // 공격 받았을 시 추가 자원 얻는 콤보 레벨
		static const MkHashStr R_BonusResPercByCritical; // 크리티컬 공격을 받았을 시 추가 자원 얻을 확률
		static const MkHashStr R_PurgeSingleResource; // 자원 하나 삭제 가능
		static const MkHashStr R_ResourceSlotSize; // 자원 슬롯 크기

		// attack
		static const MkHashStr A_AttackPowerRate; // 공격 적용률
		static const MkHashStr A_BonusAttackPower; // 추가 공격력
		static const MkHashStr A_CriticalPerc; // 크리티컬 발생 확률
		static const MkHashStr A_CriticalPowerRate; // 크리티컬 발생시 추가 공격 적용률
		static const MkHashStr A_PowerPenetration; // 방어 무시율
		static const MkHashStr A_BustDurability; // 자신이 준 damage가 상대의 내구도 하락으로 전환되는 비율
		static const MkHashStr A_ComboPowerBonus20; // combo 조합의 추가 AP율
		static const MkHashStr A_ComboPowerBonus30; // combo 조합의 추가 AP율
		static const MkHashStr A_ComboPowerBonus32; // combo 조합의 추가 AP율
		static const MkHashStr A_ComboPowerBonus40; // combo 조합의 추가 AP율
		static const MkHashStr A_ComboPowerBonus50; // combo 조합의 추가 AP율

		// defense
		static const MkHashStr D_ImpactResistance; // % 저항력(dam = AP - AP * ImpactResistance)
		static const MkHashStr D_DefensePower; // - 방어력(dam = AP - ImpactProtection)
		static const MkHashStr D_ProtectDurability; // 상대가 준 damage가 자신의 내구도 데미지로 전환되는 비율
		static const MkHashStr D_UnionDefense; // 자신이 공격하지 않았을 경우 공격력의 일부가 IP로 전환되는 비율
		static const MkHashStr D_CriticalResistance; // 크리티컬 damage에 대한 저항
		static const MkHashStr D_ComboPowerResist20; // combo 조합의 AP에 대한 저항
		static const MkHashStr D_ComboPowerResist30; // combo 조합의 AP에 대한 저항
		static const MkHashStr D_ComboPowerResist32; // combo 조합의 AP에 대한 저항
		static const MkHashStr D_ComboPowerResist40; // combo 조합의 AP에 대한 저항
		static const MkHashStr D_ComboPowerResist50; // combo 조합의 AP에 대한 저항

		// support
		static const MkHashStr S_SupplimentCapacity; // 인구수
		static const MkHashStr S_UnitTransformTime; // 유닛 소환/업글 시간
		static const MkHashStr S_BonusSkillAccCount; // 추가 누적 스킬 슬롯 크기
		static const MkHashStr S_BonusHealPower; // 추가 heal 량
		static const MkHashStr S_BonusHealRate; // 추가 heal 비율
		static const MkHashStr S_HitPointRestorationForNextStage; // 다음 스테이지 진행시 HP 복구량
		static const MkHashStr S_DurationRestorationForNextStage; // 다음 스테이지 진행시 dur 복구량
		static const MkHashStr S_ViewEnemyResourcePool; // 상대 자원 보기
		static const MkHashStr S_ViewEnemySkillAccum; // 상대 누적 스킬 보기
		static const MkHashStr S_ViewStageReward; // 스테이지 보상 아이템 보기
		static const MkHashStr S_ViewHiddenPath; // 숨겨진 스테이지 이동 경로 보기
	};
};