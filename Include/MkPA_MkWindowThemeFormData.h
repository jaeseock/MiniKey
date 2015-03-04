#pragma once


//------------------------------------------------------------------------------------------------//
// theme�� component�� �����ϴ� data
// MkWindowThemeUnitData�� ����
// ���¿� ���� 3������ �з���
// - eFT_SingleUnit : �ϳ��� data
// - eFT_DualUnit : �ΰ��� data(front & back)
// - eFT_QuadUnit : �װ��� data(normal, focus, pushing & disable)
//
//------------------------------------------------------------------------------------------------//
// scene node�� �޾� �־��� ���Ǵ�� ���� �� ������ �ִ� interface ����!!!!!!!!!!!!!!!!!!!!!!
//------------------------------------------------------------------------------------------------//
//
// (NOTE) ��ϵ� MkWindowThemeUnitData ��ü�� image/edge/table ��� ���̵� ��� ������,
// �ϳ��� form�� ��ϵ� unit data���� ��� ������ ����, ��Ī�Ǵ� piece�鰣�� ũ�⸦ ������ �־�� ��
// (NOTE) position���� empty unit data�� ���� �� ������ �ϳ��� form �ȿ��� �ּ��� �ϳ��� ������
// unit data�� �����ؾ� ��
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowThemeUnitData.h"


class MkDataNode;

class MkWindowThemeFormData
{
public:

	enum eFormType
	{
		eFT_None = -1,
		eFT_SingleUnit = 0,
		eFT_DualUnit,
		eFT_QuadUnit
	};

	enum eState
	{
		eS_None = -1,
		eS_Default = 0,

		// eFT_SingleUnit
		eS_Single = eS_Default,

		// eFT_DualUnit
		eS_Back = eS_Default,
		eS_Front,

		// eFT_QuadUnit
		eS_Normal = eS_Default,
		eS_Focus,
		eS_Pushing,
		eS_Disable
	};

public:

	// �ʱ�ȭ
	// node ������ ���� form type�� ���� ��
	bool SetUp(const MkHashStr* imagePath, const MkBaseTexture* texture, const MkDataNode& node);

	// form type ��ȯ
	inline eFormType GetFormType(void) const { return m_FormType; }

	//------------------------------------------------------------------------------------------------//
	// scene node ������ interface
	//------------------------------------------------------------------------------------------------//

	// scene node�� default position�� form ����
	bool AttachForm(MkSceneNode* sceneNode, double startTime = 0.) const;

	// scene node�� ����� form ����
	static void RemoveForm(MkSceneNode* sceneNode);

	// position unit�� ����� scene node�� client size ����
	// ����� unit type�� ���� ������ �ٸ�
	// - eUT_Image type�� ũ�� ������ ������� �����Ƿ� �ݿ����� ����
	//   clientSize�� windowSize�� image size, clientPosition���� zero�� �־� ��ȯ
	// - eUT_Edge, eUT_Table type�� ��� edge �κ��� ����� ���� clientSize < windowSize�� ��
	//   clientSize�� �ݿ��ϰ� windowSize�� clientPosition�� ����� ��ȯ
	// (NOTE) �Ϲ����� window ������ 2��и�(top-down)�� �������� minikey�� 1��и�(down-top)�� ����
	void SetClientSizeToForm(MkSceneNode* sceneNode, MkFloat2& clientSize, MkFloat2& clientPosition, MkFloat2& windowSize) const;

	// scene node�� ����� form�� state�� ����
	// ����� ��� unit���� ũ�Ⱑ �����ϹǷ� client size/position, window size���� ��ȭ ����
	// panel�� sequence start time�� ���� ���� �״�� ���
	bool SetFormState(MkSceneNode* sceneNode, eState state) const;

	//------------------------------------------------------------------------------------------------//

	MkWindowThemeFormData();
	~MkWindowThemeFormData() {}

protected:

	bool _AddUnitData(const MkBaseTexture* texture, const MkArray<MkHashStr>& subsetOrSequenceNameList, bool filledUnit);

	// unit type ��ȯ
	// �ʱ�ȭ�� �Ҽ� unit���� ��� ���� type���� �����Ǿ� ������ �����ϱ� ������ ����
	MkWindowThemeUnitData::eUnitType _GetUnitType(void) const;

	// form type�� ���� state ��ȿ�� ����
	bool _CheckState(eState state) const;

protected:

	const MkHashStr* m_ImagePathPtr;

	eFormType m_FormType;

	MkArray<MkWindowThemeUnitData> m_UnitList;
};
