#pragma once


//------------------------------------------------------------------------------------------------//
// theme의 component를 정의하는 data
// MkWindowThemeUnitData의 집합
// 형태에 따라 3가지로 분류됨
// - eFT_SingleUnit : 하나의 data
// - eFT_DualUnit : 두개의 data(front & back)
// - eFT_QuadUnit : 네개의 data(normal, focus, pushing & disable)
//
//------------------------------------------------------------------------------------------------//
// scene node를 받아 주어진 조건대로 구성 및 수정해 주는 interface 소유!!!!!!!!!!!!!!!!!!!!!!
//------------------------------------------------------------------------------------------------//
//
// (NOTE) 등록된 MkWindowThemeUnitData 자체는 image/edge/table 어느 것이든 상관 없지만,
// 하나의 form에 등록된 unit data들은 모두 동일한 형태, 매칭되는 piece들간의 크기를 가지고 있어야 함
// (NOTE) position으로 empty unit data를 가질 수 있지만 하나의 form 안에는 최소한 하나의 구성된
// unit data가 존재해야 함
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

	// 초기화
	// node 구성에 따라 form type이 결정 됨
	bool SetUp(const MkHashStr* imagePath, const MkBaseTexture* texture, const MkDataNode& node);

	// form type 반환
	inline eFormType GetFormType(void) const { return m_FormType; }

	// margin 반환
	inline float GetLeftMargin(void) const { return m_Margin[0]; }
	inline float GetRightMargin(void) const { return m_Margin[1]; }
	inline float GetTopMargin(void) const { return m_Margin[2]; }
	inline float GetBottomMargin(void) const { return m_Margin[3]; }

	//------------------------------------------------------------------------------------------------//
	// scene node 구성용 interface
	//------------------------------------------------------------------------------------------------//

	// scene node에 default position의 form 적용
	bool AttachForm(MkSceneNode* sceneNode, double startTime = 0.) const;

	// scene node에 적용된 form 삭제
	static void RemoveForm(MkSceneNode* sceneNode);

	// position unit이 적용된 scene node에 client size 적용
	// 적용된 unit type에 따라 동작이 다름
	// - eUT_Image type은 크기 변경이 허락되지 않으므로 반영되지 않음
	//   clientSize와 windowSize에 image size, clientPosition에는 zero를 넣어 반환
	// - eUT_Edge, eUT_Table type의 경우 edge 부분의 존재로 인해 clientSize < windowSize가 됨
	//   clientSize는 반영하고 windowSize과 clientPosition는 계산해 반환
	// (NOTE) 일반적인 window 구성은 2사분면(top-down)을 따르지만 minikey는 1사분면(down-top)을 따름
	void SetClientSizeToForm(MkSceneNode* sceneNode, MkFloat2& clientSize, MkFloat2& clientPosition, MkFloat2& windowSize) const;

	// scene node에 적용된 form의 state를 변경
	// 적용된 모든 unit들의 크기가 동일하므로 client size/position, window size에는 변화 없음
	// panel의 sequence start time은 이전 값을 그대로 계승
	bool SetFormState(MkSceneNode* sceneNode, eState state) const;

	//------------------------------------------------------------------------------------------------//

	MkWindowThemeFormData();
	~MkWindowThemeFormData() {}

protected:

	bool _AddUnitData(const MkBaseTexture* texture, const MkArray<MkHashStr>& subsetOrSequenceNameList, bool filledUnit);

	void _UpdateMargin(void);

	// unit type 반환
	// 초기화시 소속 unit들은 모두 동일 type으로 구성되어 있음을 보장하기 때문에 가능
	MkWindowThemeUnitData::eUnitType _GetUnitType(void) const;

	// form type에 따른 state 유효성 점검
	bool _CheckState(eState state) const;

protected:

	const MkHashStr* m_ImagePathPtr;

	eFormType m_FormType;

	MkArray<MkWindowThemeUnitData> m_UnitList;

	float m_Margin[4]; // 0:left, 1:right, 2:top, 3:bottom
};
