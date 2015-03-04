
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkSceneNode.h"
#include "MkPA_MkWindowThemeFormData.h"


const static MkHashStr SINGLE_UNIT_POS_KEY = MK_VALUE_TO_STRING(eS_Single);

const static MkHashStr DUAL_UNIT_POS_KEY[2] =
{
	MK_VALUE_TO_STRING(eS_Back),
	MK_VALUE_TO_STRING(eS_Front)
};

const static MkHashStr QUAD_UNIT_POS_KEY[4] =
{
	MK_VALUE_TO_STRING(eS_Normal),
	MK_VALUE_TO_STRING(eS_Focus),
	MK_VALUE_TO_STRING(eS_Pushing),
	MK_VALUE_TO_STRING(eS_Disable)
};

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeFormData::SetUp(const MkHashStr* imagePath, const MkBaseTexture* texture, const MkDataNode& node)
{
	if (imagePath == NULL)
		return false;

	m_ImagePathPtr = imagePath;

	MkArray<MkHashStr> buffer[4];
	
	// eFT_SingleUnit
	if (node.GetDataEx(SINGLE_UNIT_POS_KEY, buffer[eS_Single]))
	{
		MK_CHECK(!buffer[eS_Single][0].Empty(), L"eFT_SingleUnit은 empty unit data가 허용되지 않음")
			return false;

		m_FormType = eFT_SingleUnit;
		return _AddUnitData(texture, buffer[eS_Single], true);
	}
	
	MkArray<bool> haveUD(4);
	unsigned int count = 0;

	// eFT_DualUnit
	if (node.GetDataEx(DUAL_UNIT_POS_KEY[eS_Back], buffer[eS_Back]) &&
		node.GetDataEx(DUAL_UNIT_POS_KEY[eS_Front], buffer[eS_Front]))
	{
		count = 2;
		haveUD.PushBack(!buffer[eS_Back][0].Empty());
		haveUD.PushBack(!buffer[eS_Front][0].Empty());

		MK_CHECK(haveUD[eS_Back] || haveUD[eS_Front], L"eFT_DualUnit은 최소 한 개 state에 해당하는 unit data를 가지고 있어야 함")
			return false;

		m_FormType = eFT_DualUnit;
	}
	// eFT_QuadUnit
	else if (node.GetDataEx(QUAD_UNIT_POS_KEY[eS_Normal], buffer[eS_Normal]) &&
		node.GetDataEx(QUAD_UNIT_POS_KEY[eS_Focus], buffer[eS_Focus]) &&
		node.GetDataEx(QUAD_UNIT_POS_KEY[eS_Pushing], buffer[eS_Pushing]) &&
		node.GetDataEx(QUAD_UNIT_POS_KEY[eS_Disable], buffer[eS_Disable]))
	{
		count = 4;
		haveUD.PushBack(!buffer[eS_Normal][0].Empty());
		haveUD.PushBack(!buffer[eS_Focus][0].Empty());
		haveUD.PushBack(!buffer[eS_Pushing][0].Empty());
		haveUD.PushBack(!buffer[eS_Disable][0].Empty());

		MK_CHECK(haveUD[eS_Normal] || haveUD[eS_Focus] || haveUD[eS_Pushing] || haveUD[eS_Disable], L"eFT_QuadUnit은 최소 한 개 state에 해당하는 unit data를 가지고 있어야 함")
			return false;

		m_FormType = eFT_QuadUnit;
	}
	// error
	else
	{
		m_FormType = eFT_None;

		MK_CHECK(false, L"MkWindowThemeFormData의 구성이 정상적이지 않음")
			return false;
	}

	unsigned int firstFilledPos = haveUD.FindFirstInclusion(MkArraySection(0), true); // MKDEF_ARRAY_ERROR가 반환될 가능성은 없음

	m_UnitList.Reserve(count); // count는 2, 아니면 4

	for (unsigned int i=0; i<count; ++i)
	{
		if (!_AddUnitData(texture, buffer[(haveUD[i]) ? i : firstFilledPos], haveUD[i]))
			return false;
	}

	// 동일 unit type인지 점검
	MkWindowThemeUnitData::eUnitType ut = m_UnitList[0].GetUnitType();
	for (unsigned int i=1; i<count; ++i)
	{
		MK_CHECK(m_UnitList[i].GetUnitType() == ut, L"MkWindowThemeFormData에 등록된 unit들의 type이 동일하지 않음")
			return false;
	}

	// 동일 size인지 점검
	const MkArray<MkWindowThemeUnitData::PieceData>& defData = m_UnitList[0].GetPieceData();
	for (unsigned int i=1; i<count; ++i)
	{
		const MkArray<MkWindowThemeUnitData::PieceData>& currData = m_UnitList[i].GetPieceData();
		MK_INDEXING_LOOP(defData, j)
		{
			MK_CHECK(defData[j].size == currData[j].size, L"MkWindowThemeFormData에 등록된 unit들간 매칭되는 piece size가 동일하지 않음")
				return false;
		}
	}
	return true;
}

bool MkWindowThemeFormData::AttachForm(MkSceneNode* sceneNode, double startTime) const
{
	if ((sceneNode == NULL) || (GetFormType() == eFT_None))
		return false;

	MkWindowThemeUnitData::eUnitType myUnitType = _GetUnitType(); // eUT_None일 가능성은 없음(초기화 실패한 경우는 호출 자체가 안될뿐더러 위에서 걸러짐)
	MkWindowThemeUnitData::eUnitType nodeUnitType = MkWindowThemeUnitData::GetUnitType(sceneNode);

	// 노드에 적용 될 unit type과 다른 unit type이 존재하면 삭제
	if ((nodeUnitType != MkWindowThemeUnitData::eUT_None) && (nodeUnitType != myUnitType))
	{
		MkWindowThemeUnitData::DeleteUnit(sceneNode);
	}

	// 노드에 unit type이 없거나 적용 될 unit type과 다르면 생성
	if ((nodeUnitType == MkWindowThemeUnitData::eUT_None) || (nodeUnitType != myUnitType))
	{
		// 0번 position(eS_Single, eS_Back, eS_Normal)이 default
		m_UnitList[0].CreateUnit(sceneNode, *m_ImagePathPtr, startTime);
	}
	// 같은 unit type이면 theme 변경일 가능성이 있으므로 set
	else
	{
		m_UnitList[0].SetUnit(sceneNode, *m_ImagePathPtr);
	}
	return true;
}

void MkWindowThemeFormData::RemoveForm(MkSceneNode* sceneNode)
{
	if ((sceneNode != NULL) && (MkWindowThemeUnitData::GetUnitType(sceneNode) != MkWindowThemeUnitData::eUT_None))
	{
		MkWindowThemeUnitData::DeleteUnit(sceneNode);
	}
}

void MkWindowThemeFormData::SetClientSizeToForm(MkSceneNode* sceneNode, MkFloat2& clientSize, MkFloat2& clientPosition, MkFloat2& windowSize) const
{
	if (sceneNode != NULL)
	{
		// 적용된 unit의 크기가 모두 같으므로 아무 unit을 기준으로 적용해도 상관 없음
		m_UnitList[0].SetClientSize(sceneNode, clientSize, clientPosition, windowSize);
	}
}

bool MkWindowThemeFormData::SetFormState(MkSceneNode* sceneNode, eState state) const
{
	if ((sceneNode == NULL) || (state == eS_None) || (!_CheckState(state)))
		return false;

	m_UnitList[state].SetUnit(sceneNode, *m_ImagePathPtr);
	return true;
}

MkWindowThemeFormData::MkWindowThemeFormData()
{
	m_FormType = eFT_None;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeFormData::_AddUnitData(const MkBaseTexture* texture, const MkArray<MkHashStr>& subsetOrSequenceNameList, bool filledUnit)
{
	MkWindowThemeUnitData& ud = m_UnitList.PushBack();
	return ud.SetUp(m_ImagePathPtr->GetString(), texture, subsetOrSequenceNameList, filledUnit);
}

MkWindowThemeUnitData::eUnitType MkWindowThemeFormData::_GetUnitType(void) const
{
	return m_UnitList.Empty() ? MkWindowThemeUnitData::eUT_None : m_UnitList[0].GetUnitType();
}

bool MkWindowThemeFormData::_CheckState(eState state) const
{
	switch (GetFormType())
	{
	case eFT_SingleUnit: return (state == eS_Single);
	case eFT_DualUnit: return (state == eS_Back) || (state == eS_Front);
	case eFT_QuadUnit: return (state == eS_Normal) || (state == eS_Focus) || (state == eS_Pushing) || (state == eS_Disable);
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
