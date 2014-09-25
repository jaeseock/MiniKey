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
		static const MkHashStr R_ExtractResCD; // �ڿ� ���� ��ٿ�
		static const MkHashStr R_BoostExtraction; // �ڿ� ���� ��ٿ� ����
		static const MkHashStr R_BonusResPerc; // �߰� �ڿ� ���� Ȯ��
		static const MkHashStr R_VoidTypeResAppPerc; // ������ �ڿ� ���� Ȯ��
		static const MkHashStr R_ComboLevForBonusRes; // ���� �޾��� �� �߰� �ڿ� ��� �޺� ����
		static const MkHashStr R_BonusResPercByCritical; // ũ��Ƽ�� ������ �޾��� �� �߰� �ڿ� ���� Ȯ��
		static const MkHashStr R_PurgeSingleResource; // �ڿ� �ϳ� ���� ����
		static const MkHashStr R_ResourceSlotSize; // �ڿ� ���� ũ��

		// attack
		static const MkHashStr A_AttackPowerRate; // ���� �����
		static const MkHashStr A_BonusAttackPower; // �߰� ���ݷ�
		static const MkHashStr A_CriticalPerc; // ũ��Ƽ�� �߻� Ȯ��
		static const MkHashStr A_CriticalPowerRate; // ũ��Ƽ�� �߻��� �߰� ���� �����
		static const MkHashStr A_PowerPenetration; // ��� ������
		static const MkHashStr A_BustDurability; // �ڽ��� �� damage�� ����� ������ �϶����� ��ȯ�Ǵ� ����
		static const MkHashStr A_ComboPowerBonus20; // combo ������ �߰� AP��
		static const MkHashStr A_ComboPowerBonus30; // combo ������ �߰� AP��
		static const MkHashStr A_ComboPowerBonus32; // combo ������ �߰� AP��
		static const MkHashStr A_ComboPowerBonus40; // combo ������ �߰� AP��
		static const MkHashStr A_ComboPowerBonus50; // combo ������ �߰� AP��

		// defense
		static const MkHashStr D_ImpactResistance; // % ���׷�(dam = AP - AP * ImpactResistance)
		static const MkHashStr D_DefensePower; // - ����(dam = AP - ImpactProtection)
		static const MkHashStr D_ProtectDurability; // ��밡 �� damage�� �ڽ��� ������ �������� ��ȯ�Ǵ� ����
		static const MkHashStr D_UnionDefense; // �ڽ��� �������� �ʾ��� ��� ���ݷ��� �Ϻΰ� IP�� ��ȯ�Ǵ� ����
		static const MkHashStr D_CriticalResistance; // ũ��Ƽ�� damage�� ���� ����
		static const MkHashStr D_ComboPowerResist20; // combo ������ AP�� ���� ����
		static const MkHashStr D_ComboPowerResist30; // combo ������ AP�� ���� ����
		static const MkHashStr D_ComboPowerResist32; // combo ������ AP�� ���� ����
		static const MkHashStr D_ComboPowerResist40; // combo ������ AP�� ���� ����
		static const MkHashStr D_ComboPowerResist50; // combo ������ AP�� ���� ����

		// support
		static const MkHashStr S_SupplimentCapacity; // �α���
		static const MkHashStr S_UnitTransformTime; // ���� ��ȯ/���� �ð�
		static const MkHashStr S_BonusSkillAccCount; // �߰� ���� ��ų ���� ũ��
		static const MkHashStr S_BonusHealPower; // �߰� heal ��
		static const MkHashStr S_BonusHealRate; // �߰� heal ����
		static const MkHashStr S_HitPointRestorationForNextStage; // ���� �������� ����� HP ������
		static const MkHashStr S_DurationRestorationForNextStage; // ���� �������� ����� dur ������
		static const MkHashStr S_ViewEnemyResourcePool; // ��� �ڿ� ����
		static const MkHashStr S_ViewEnemySkillAccum; // ��� ���� ��ų ����
		static const MkHashStr S_ViewStageReward; // �������� ���� ������ ����
		static const MkHashStr S_ViewHiddenPath; // ������ �������� �̵� ��� ����
	};
};