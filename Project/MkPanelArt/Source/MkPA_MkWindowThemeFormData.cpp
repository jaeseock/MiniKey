
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkSceneNode.h"
#include "MkPA_MkWindowThemeFormData.h"


const static MkHashStr SINGLE_UNIT_POS_KEY = L"Single";

const static MkHashStr DUAL_UNIT_POS_KEY[2] =
{
	L"Back", // eP_Back
	L"Front" // eP_Front
};

const static MkHashStr QUAD_UNIT_POS_KEY[4] =
{
	L"Normal", // eP_Normal
	L"Focus", // eP_Focus
	L"Pushing", // eP_Pushing
	L"Disable" // eP_Disable
};

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeFormData::SetUp(const MkHashStr* imagePath, const MkBaseTexture* texture, const MkDataNode& node)
{
	if (imagePath == NULL)
		return false;

	m_ImagePathPtr = imagePath;

	MkArray<MkHashStr> buffer[4];
	unsigned int count = 0;

	// eFT_SingleUnit
	if (node.GetDataEx(SINGLE_UNIT_POS_KEY, buffer[0]))
	{
		count = 1;
	}
	else if (node.GetDataEx(DUAL_UNIT_POS_KEY[eP_Back], buffer[eP_Back]) &&
		node.GetDataEx(DUAL_UNIT_POS_KEY[eP_Front], buffer[eP_Front]))
	{
		count = 2;
	}
	else if (node.GetDataEx(QUAD_UNIT_POS_KEY[eP_Normal], buffer[eP_Normal]) &&
		node.GetDataEx(QUAD_UNIT_POS_KEY[eP_Focus], buffer[eP_Focus]) &&
		node.GetDataEx(QUAD_UNIT_POS_KEY[eP_Pushing], buffer[eP_Pushing]) &&
		node.GetDataEx(QUAD_UNIT_POS_KEY[eP_Disable], buffer[eP_Disable]))
	{
		count = 4;
	}

	MK_CHECK((count == 1) || (count == 2) || (count == 4), L"MkWindowThemeFormData의 구성은 1, 2, 4개의 unit 필요")
		return false;

	m_UnitList.Reserve(count);

	for (unsigned int i=0; i<count; ++i)
	{
		if (!_AddUnitData(texture, buffer[i]))
			return false;
	}

	if (count != 1)
	{
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
	}
	return true;
}

MkWindowThemeFormData::eFormType MkWindowThemeFormData::GetFormType(void) const
{
	switch (m_UnitList.GetSize())
	{
	case 1: return eFT_SingleUnit;
	case 2: return eFT_DualUnit;
	case 4: return eFT_QuadUnit;
	}
	return eFT_None;
}

bool MkWindowThemeFormData::AttachFormToSceneNode(MkSceneNode* sceneNode, double startTime) const
{
	if ((sceneNode == NULL) || (GetFormType() == eFT_None))
		return false;

	MkWindowThemeUnitData::eUnitType myUnitType = _GetUnitType(); // eUT_None일 가능성은 없음(초기화 실패한 경우는 호출 자체가 안될뿐더러 위에서 걸러짐)
	MkWindowThemeUnitData::eUnitType nodeUnitType = MkWindowThemeUnitData::GetUnitType(sceneNode);

	// 노드에 적용 될 unit type과 다른 unit type이 존재하면 있으면 삭제
	if ((nodeUnitType != MkWindowThemeUnitData::eUT_None) && (nodeUnitType != myUnitType))
	{
		// 등록된 모든 unit은 같은 type을 가지고 있으므로 아무 곳에서나 명령 가능
		m_UnitList[0].DeleteUnit(sceneNode);
	}

	// 노드에 unit type이 없거나 적용 될 unit type과 다르면 생성
	if ((nodeUnitType == MkWindowThemeUnitData::eUT_None) || (nodeUnitType != myUnitType))
	{
		// 0번 position(eP_Single, eP_Back, eP_Normal)이 default
		m_UnitList[0].CreateUnit(sceneNode, *m_ImagePathPtr, startTime);
	}
	return true;
}

void MkWindowThemeFormData::SetClientSizeToForm(MkSceneNode* sceneNode, MkFloat2& clientSize, MkFloat2& clientPosition, MkFloat2& windowSize) const
{
	if (sceneNode != NULL)
	{
		// 적용된 unit의 크기가 모두 같으므로 아무 unit을 기준으로 적용해도 상관 없음
		m_UnitList[0].SetClientSize(sceneNode, clientSize, clientPosition, windowSize);
	}
}

bool MkWindowThemeFormData::SetFormPosition(MkSceneNode* sceneNode, ePosition position) const
{
	if (sceneNode == NULL)
		return false;

	MK_CHECK(_CheckPosition(position), sceneNode->GetNodeName().GetString() + L" node에 적용된 form과 다른 unit으로 position 변경 시도")
		return false;

	m_UnitList[position].SetUnit(sceneNode, *m_ImagePathPtr);
	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeFormData::_AddUnitData(const MkBaseTexture* texture, const MkArray<MkHashStr>& subsetOrSequenceNameList)
{
	MkWindowThemeUnitData& ud = m_UnitList.PushBack();
	return ud.SetUp(m_ImagePathPtr->GetString(), texture, subsetOrSequenceNameList);
}

MkWindowThemeUnitData::eUnitType MkWindowThemeFormData::_GetUnitType(void) const
{
	return m_UnitList.Empty() ? MkWindowThemeUnitData::eUT_None : m_UnitList[0].GetUnitType();
}

bool MkWindowThemeFormData::_CheckPosition(ePosition position) const
{
	switch (GetFormType())
	{
	case eFT_SingleUnit: return (position == eP_Single);
	case eFT_DualUnit: return (position == eP_Back) || (position == eP_Front);
	case eFT_QuadUnit: return (position == eP_Normal) || (position == eP_Focus) || (position == eP_Pushing) || (position == eP_Disable);
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
