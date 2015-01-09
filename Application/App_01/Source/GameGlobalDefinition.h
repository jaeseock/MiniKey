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
		static const MkHashStr A_BonusAttackPower; // �߰� ���ݷ�
		static const MkHashStr A_AttackPowerRate; // ���� �����
		static const MkHashStr A_OnPairBonusAP; // one-pair ������ �߰� ���ݷ�
		static const MkHashStr A_TrippleBonusAP; // tripple ������ �߰� ���ݷ�
		static const MkHashStr A_FullHouseBonusAP; // full-house ������ �߰� ���ݷ�
		static const MkHashStr A_FourCardBonusAP; // 4-card ������ �߰� ���ݷ�
		static const MkHashStr A_FiveCardBonusAP; // 5-card ������ �߰� ���ݷ�
		static const MkHashStr A_CriticalDamageRate; // ���� ���������� ġ��ջ����� �����Ǵ� ����
		static const MkHashStr A_DrainHPAtFourCard; // 4-card ���ս� ���� �������� �������� �Ϲ�ȸ��
		static const MkHashStr A_DrainHPAtFiveCard; // 5-card ���ս� ���� �������� �������� �Ϲ�ȸ��
		static const MkHashStr A_DecayEnemyHPt; // ���� �������� ��������ŭ ����� �Ϲ� ȸ������ �϶�

		// defense
		static const MkHashStr D_DamageResistance; // % ���׷�(dam = AP - AP * DamageResistance)
		static const MkHashStr D_DefensePower; // - ����(dam = AP - DefensePower)
		static const MkHashStr D_BonusDRAtRest; // ���� �߰� ���׷�
		static const MkHashStr D_Toughness; // �й�� �߰� ����
		static const MkHashStr D_CritRecoveryAtFourCard; // 4-card ���ս� ġ��ջ� ������
		static const MkHashStr D_CritRecoveryAtFiveCard; // 5-card ���ս� ġ��ջ� ������

		// support
		static const MkHashStr S_SupplimentCapacity; // �α���
		static const MkHashStr S_BonusTransformTime; // �߰� ���� ��ȯ/���� �ð�
		static const MkHashStr S_HealPower; // �Ϲ� ȸ����
		static const MkHashStr S_PtToGainHeal; // �Ϲ�ȸ�� ��� ����
		static const MkHashStr S_HPtAtRest; // �޽Ľ� ��� �Ϲ� ȸ������
		static const MkHashStr S_HPtForEveryTurn; // �ϴ� ��� �Ϲ� ȸ������
		static const MkHashStr S_HPtAtLoss; // �й�� ��� �Ϲ� ȸ������
		static const MkHashStr S_ViewEnemyInfo; // ��� �δ� �������� �ܰ�
		static const MkHashStr S_WinnerBonusAPR; // �¸��� �߰� ���� �����

		// resource
		static const MkHashStr R_RedResWeight; // red �ڿ� �߻�����
		static const MkHashStr R_GreenResWeight; // green �ڿ� �߻�����
		static const MkHashStr R_VioletResWeight; // violet �ڿ� �߻�����
		static const MkHashStr R_BlueResWeight; // blue �ڿ� �߻�����
		static const MkHashStr R_VoidResWeight; // ������ �ڿ� �߻�����
		static const MkHashStr R_PtToGainRes; // �ڿ� ��� ����
		static const MkHashStr R_RPtAtRest; // �޽Ľ� ��� �ڿ�����
		static const MkHashStr R_RPtForEveryTurn; // �ϴ� ��� �ڿ�����
		static const MkHashStr R_ResSlotSize; // �ڿ� ���� ũ��
		static const MkHashStr R_OnPairPower; // one-pair ������ power
		static const MkHashStr R_TripplePower; // tripple ������ power
		static const MkHashStr R_FullHousePower; // full-house ������ power
		static const MkHashStr R_FourCardPower; // 4-card ������ power
		static const MkHashStr R_FiveCardPower; // 5-card ������ power
		static const MkHashStr R_DecayEnemyRPtAtFourCard; // 4-card ���ս� ��� �ڿ����� ���ҷ�
		static const MkHashStr R_DecayEnemyRPtAtFiveCard; // 5-card ���ս� ��� �ڿ����� ���ҷ�
	};
};

#define GDEF_DEF_USER_DATA_PATH L"DataNode\\DefaultUserData.txt"
#define GDEF_LAST_SAVE_DATA_PATH L"UserData\\LastSaveFile.txt"

#define GDEF_USER_SAVE_DATA_EXT L"txt"

#define GDEF_MAX_AGENT_COUNT 0xff
#define GDEF_MAX_AGENT_LEVEL 0xff

#define GDEF_MAX_TEAM_PLAYER_COUNT 3
#define GDEF_MAX_TROOP_AGENT_COUNT 8