
#include "MkCore_MkCheck.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkBaseTexture.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkWindowThemeUnitData.h"


const MkHashStr MkWindowThemeUnitData::ImagePositionName = MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"IMG";

const MkHashStr MkWindowThemeUnitData::EdgeAndTablePositionName[9] =
{
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"LT", // eP_LT
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"MT", // eP_MT
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"RT", // eP_RT
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"LC", // eP_LC
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"MC", // eP_MC
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"RC", // eP_RC
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"LB", // eP_LB
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"MB", // eP_MB
	MkStr(MKDEF_PA_WIN_VISUAL_PATTERN_PREFIX) + L"RB" // eP_RB
};

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeUnitData::SetUp
(const MkStr& imagePath, const MkBaseTexture* texture, const MkArray<MkHashStr>& subsetOrSequenceNameList, bool filledUnit)
{
	unsigned int count = subsetOrSequenceNameList.GetSize();
	switch (count)
	{
	case 1: m_UnitType = eUT_Image; break;
	case 8: m_UnitType = eUT_Edge; break;
	case 9: m_UnitType = eUT_Table; break;
	default:
		{
			m_UnitType = eUT_None;

			MK_CHECK(false, L"MkWindowThemeUnitData의 구성은 1, 8, 9개의 subset/sequence 필요")
				return false;
		}
		break;
	}

	m_PieceDatas.Reserve(count);

	for (unsigned int i=0; i<count; ++i)
	{
		if ((i == eP_MC) && (m_UnitType == eUT_Edge))
		{
			m_PieceDatas.PushBack(); // eUT_Edge의 경우 middle-center에 해당하는 공간을 비워 둠
		}

		const MkHashStr& currSSName = subsetOrSequenceNameList[i];
		const MkImageInfo::Subset* subset = texture->GetImageInfo().GetCurrentSubsetPtr(currSSName);
		MK_CHECK(subset != NULL, L"잘못된 subset/sequence 지정 : " + imagePath + L" (" + currSSName.GetString() + L")")
			return false;

		PieceData& pd = m_PieceDatas.PushBack();

		if (filledUnit)
		{
			pd.subsetOrSequenceName = currSSName;
		}
		pd.size = subset->rectSize;
	}

	if ((m_UnitType == eUT_Edge) || (m_UnitType == eUT_Table))
	{
		// size 검증
		MK_CHECK((m_PieceDatas[eP_LT].size.x == m_PieceDatas[eP_LC].size.x) && (m_PieceDatas[eP_LC].size.x == m_PieceDatas[eP_LB].size.x), L"LT, LC, LB의 width는 동일해야 함")
			return false;
		MK_CHECK((m_PieceDatas[eP_RT].size.x == m_PieceDatas[eP_RC].size.x) && (m_PieceDatas[eP_RC].size.x == m_PieceDatas[eP_RB].size.x), L"RT, RC, RB의 width는 동일해야 함")
			return false;
		MK_CHECK((m_PieceDatas[eP_LT].size.y == m_PieceDatas[eP_MT].size.y) && (m_PieceDatas[eP_MT].size.y == m_PieceDatas[eP_RT].size.y), L"LT, MT, RT의 height는 동일해야 함")
			return false;
		MK_CHECK((m_PieceDatas[eP_LB].size.y == m_PieceDatas[eP_MB].size.y) && (m_PieceDatas[eP_MB].size.y == m_PieceDatas[eP_RB].size.y), L"LB, MB, RB의 height는 동일해야 함")
			return false;

		// panel position pivot 할당. client size 반영시에는,
		// - LC, MC, LB, MB는 pivot을 그대로 사용
		// - (RT, RC, RB).x = pivot.x + clientSize.x
		// - (LT, MT, RT).y = pivot.y + clientSize.y
		for (unsigned int y=0; y<3; ++y)
		{
			for (unsigned int x=0; x<3; ++x)
			{
				ePosition position = static_cast<ePosition>(y * 3 + x);
				MkFloat2& pivot = m_PieceDatas[position].pivot;
				pivot.x = (x == 0) ? 0.f : m_PieceDatas[eP_LB].size.x;
				pivot.y = (y == 2) ? 0.f : m_PieceDatas[eP_LB].size.y;
			}
		}
	}

	return true;
}

const MkHashStr& MkWindowThemeUnitData::GetSubsetOrSequenceName(ePosition position) const
{
	return m_PieceDatas.IsValidIndex(static_cast<unsigned int>(position)) ? m_PieceDatas[position].subsetOrSequenceName : MkHashStr::EMPTY;
}

MkFloat2 MkWindowThemeUnitData::CalculateWindowSize(const MkFloat2& clientSize) const
{
	MkFloat2 windowSize;
	switch (GetUnitType())
	{
	case eUT_Image:
		windowSize = m_PieceDatas[0].size;
		break;

	case eUT_Edge:
	case eUT_Table:
		windowSize.x = m_PieceDatas[eP_LC].size.x + clientSize.x + m_PieceDatas[eP_RC].size.x;
		windowSize.y = m_PieceDatas[eP_MB].size.y + clientSize.y + m_PieceDatas[eP_MT].size.y;
		break;
	}
	return windowSize;
}

//------------------------------------------------------------------------------------------------//

MkWindowThemeUnitData::eUnitType MkWindowThemeUnitData::GetUnitType(const MkSceneNode* sceneNode)
{
	eUnitType unitType = eUT_None;

	// image?
	if (sceneNode->PanelExist(ImagePositionName))
	{
		unitType = eUT_Image;
	}
	// edge or table?
	else if (sceneNode->PanelExist(EdgeAndTablePositionName[eP_LT])) // left-top piece를 가지고 있으면
	{
		unitType = sceneNode->PanelExist(EdgeAndTablePositionName[eP_MC]) ? eUT_Table : eUT_Edge; // middle-center piece 유무에 따라 갈림
	}
	return unitType;
}

void MkWindowThemeUnitData::CreateUnit(MkSceneNode* sceneNode, const MkHashStr& imagePath, double timeOffset) const
{
	_ApplyUnit(sceneNode, true, imagePath, timeOffset);
}

void MkWindowThemeUnitData::SetUnit(MkSceneNode* sceneNode, const MkHashStr& imagePath) const
{
	_ApplyUnit(sceneNode, false, imagePath, 0.);
}

void MkWindowThemeUnitData::DeleteUnit(MkSceneNode* sceneNode)
{
	if (sceneNode->PanelExist(ImagePositionName))
	{
		sceneNode->DeletePanel(ImagePositionName);
	}
	else
	{
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_LT]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_MT]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_RT]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_LC]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_MC]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_RC]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_LB]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_MB]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_RB]);
	}
}

void MkWindowThemeUnitData::SetClientSize(MkSceneNode* sceneNode, MkFloat2& clientSize, MkFloat2& clientPosition, MkFloat2& windowSize) const
{
	eUnitType nodeUnitType = GetUnitType(sceneNode);
	MK_CHECK(nodeUnitType == GetUnitType(), sceneNode->GetNodeName().GetString() + L" node에 적용된 unit type이 theme의 unit type과 다름")
		return;

	if (nodeUnitType == eUT_Image)
	{
		clientPosition.Clear();
		windowSize = clientSize = m_PieceDatas[0].size;
	}
	else
	{
		if (nodeUnitType == eUT_Table)
		{
			_SetPieceSizeAndPosition(sceneNode, eP_MC, clientSize, m_PieceDatas[eP_MC].pivot);
		}

		MkFloat2 offsetX(clientSize.x, 0.f); // RC, RB
		MkFloat2 offsetY(0.f, clientSize.y); // LT, MT

		_SetPiecePosition(sceneNode, eP_LT, m_PieceDatas[eP_LT].pivot + offsetY);
		_SetPieceSizeAndPosition(sceneNode, eP_MT, MkFloat2(clientSize.x, m_PieceDatas[eP_MT].size.y), m_PieceDatas[eP_MT].pivot + offsetY);
		_SetPiecePosition(sceneNode, eP_RT, m_PieceDatas[eP_RT].pivot + clientSize);
		_SetPieceSizeAndPosition(sceneNode, eP_LC, MkFloat2(m_PieceDatas[eP_LC].size.x, clientSize.y), m_PieceDatas[eP_LC].pivot);
		_SetPieceSizeAndPosition(sceneNode, eP_RC, MkFloat2(m_PieceDatas[eP_RC].size.x, clientSize.y), m_PieceDatas[eP_RC].pivot + offsetX);
		_SetPiecePosition(sceneNode, eP_LB, m_PieceDatas[eP_LB].pivot);
		_SetPieceSizeAndPosition(sceneNode, eP_MB, MkFloat2(clientSize.x, m_PieceDatas[eP_MB].size.y), m_PieceDatas[eP_MB].pivot);
		_SetPiecePosition(sceneNode, eP_RB, m_PieceDatas[eP_RB].pivot + offsetX);

		clientPosition = m_PieceDatas[eP_LB].size;
		windowSize = m_PieceDatas[eP_LB].size + clientSize + m_PieceDatas[eP_RT].size;
	}
}

MkWindowThemeUnitData::MkWindowThemeUnitData()
{
	m_UnitType = eUT_None;
}

//------------------------------------------------------------------------------------------------//

void MkWindowThemeUnitData::_ApplyUnit(MkSceneNode* sceneNode, bool createOrGet, const MkHashStr& imagePath, double timeOffset) const
{
	switch (GetUnitType())
	{
	case eUT_Image:
		_SetImageToPiece(sceneNode, createOrGet, ImagePositionName, imagePath, m_PieceDatas[0].subsetOrSequenceName, false, timeOffset);
		break;

	case eUT_Table:
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_MC], imagePath, m_PieceDatas[eP_MC].subsetOrSequenceName, false, timeOffset);
	case eUT_Edge:
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_LT], imagePath, m_PieceDatas[eP_LT].subsetOrSequenceName, true, timeOffset);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_MT], imagePath, m_PieceDatas[eP_MT].subsetOrSequenceName, false, timeOffset);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_RT], imagePath, m_PieceDatas[eP_RT].subsetOrSequenceName, true, timeOffset);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_LC], imagePath, m_PieceDatas[eP_LC].subsetOrSequenceName, false, timeOffset);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_RC], imagePath, m_PieceDatas[eP_RC].subsetOrSequenceName, false, timeOffset);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_LB], imagePath, m_PieceDatas[eP_LB].subsetOrSequenceName, true, timeOffset);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_MB], imagePath, m_PieceDatas[eP_MB].subsetOrSequenceName, false, timeOffset);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_RB], imagePath, m_PieceDatas[eP_RB].subsetOrSequenceName, true, timeOffset);
		break;
	}
}

void MkWindowThemeUnitData::_SetImageToPiece
(MkSceneNode* sceneNode, bool createOrGet, const MkHashStr& pieceName, const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName, bool keepSrcSize, double timeOffset)
{
	MkPanel* panel = sceneNode->PanelExist(pieceName) ?
		(createOrGet ? NULL : sceneNode->GetPanel(pieceName)) : // create
		(createOrGet ? &sceneNode->CreatePanel(pieceName) : NULL); // get

	if (panel != NULL)
	{
		// subsetOrSequenceName이 비었을 경우 null texture 설정(panel 정보는 유지되지만 그려지지는 않음)
		// get일 경우 time offset은 이전 값을 그대로 계승
		panel->SetTexture(subsetOrSequenceName.Empty() ? MkHashStr::EMPTY : imagePath, subsetOrSequenceName, createOrGet ? timeOffset : panel->GetSequenceTimeOffset());
		panel->SetSmallerSourceOp(keepSrcSize ? MkPanel::eReducePanel : MkPanel::eExpandSource);
		panel->SetBiggerSourceOp(keepSrcSize ? MkPanel::eExpandPanel : MkPanel::eReduceSource);
	}
}

void MkWindowThemeUnitData::_SetPiecePosition(MkSceneNode* sceneNode, ePosition piecePosition, const MkFloat2& panelPosition)
{
	const MkHashStr& pieceName = EdgeAndTablePositionName[piecePosition];
	if (sceneNode->PanelExist(pieceName))
	{
		sceneNode->GetPanel(pieceName)->SetLocalPosition(panelPosition);
	}
}

void MkWindowThemeUnitData::_SetPieceSizeAndPosition(MkSceneNode* sceneNode, ePosition piecePosition, const MkFloat2& size, const MkFloat2& panelPosition)
{
	const MkHashStr& pieceName = EdgeAndTablePositionName[piecePosition];
	if (sceneNode->PanelExist(pieceName))
	{
		MkPanel* panel = sceneNode->GetPanel(pieceName);
		panel->SetPanelSize(size);
		panel->SetLocalPosition(panelPosition);
	}
}

//------------------------------------------------------------------------------------------------//
