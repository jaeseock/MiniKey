
#include "MkCore_MkCheck.h"

#include "MkPA_MkBaseTexture.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkWindowThemeUnitData.h"


const MkHashStr MkWindowThemeUnitData::ImagePositionName = L"__#P:IMG";

const MkHashStr MkWindowThemeUnitData::EdgeAndTablePositionName[9] =
{
	L"__#P:LT", // eP_LT
	L"__#P:MT", // eP_MT
	L"__#P:RT", // eP_RT
	L"__#P:LC", // eP_LC
	L"__#P:MC", // eP_MC
	L"__#P:RC", // eP_RC
	L"__#P:LB", // eP_LB
	L"__#P:MB", // eP_MB
	L"__#P:RB" // eP_RB
};

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeUnitData::SetUp(const MkStr& imagePath, const MkBaseTexture* texture, const MkArray<MkHashStr>& subsetOrSequenceNameList)
{
	unsigned int count = subsetOrSequenceNameList.GetSize();

	MK_CHECK((count == 1) || (count == 8) || (count == 9), L"MkWindowThemeUnitData의 구성은 1, 8, 9개의 subset/sequence 필요")
		return false;

	m_PieceDatas.Reserve(count);

	for (unsigned int i=0; i<count; ++i)
	{
		if ((count == 8) && (i == eP_MC))
		{
			m_PieceDatas.PushBack(); // eUT_Edge의 경우 middle-center에 해당하는 공간을 비워 둠
		}

		if (!_AddPieceData(imagePath, texture, subsetOrSequenceNameList[i]))
			return false;
	}

	if ((count == 8) || (count == 9))
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

MkWindowThemeUnitData::eUnitType MkWindowThemeUnitData::GetUnitType(void) const
{
	switch (m_PieceDatas.GetSize())
	{
	case 1: return eUT_Image;
	case 9: return m_PieceDatas[eP_MC].subsetOrSequenceName.Empty() ? eUT_Edge : eUT_Table;
	}
	return eUT_None;
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

void MkWindowThemeUnitData::CreateUnit(MkSceneNode* sceneNode, const MkHashStr& imagePath, double startTime) const
{
	_ApplyUnit(sceneNode, true, imagePath, startTime);
}

void MkWindowThemeUnitData::SetUnit(MkSceneNode* sceneNode, const MkHashStr& imagePath) const
{
	_ApplyUnit(sceneNode, false, imagePath, 0.);
}

void MkWindowThemeUnitData::DeleteUnit(MkSceneNode* sceneNode) const
{
	switch (GetUnitType())
	{
	case eUT_Image:
		sceneNode->DeletePanel(ImagePositionName);
		break;

	case eUT_Table:
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_MC]);
	case eUT_Edge:
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_LT]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_MT]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_RT]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_LC]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_RC]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_LB]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_MB]);
		sceneNode->DeletePanel(EdgeAndTablePositionName[eP_RB]);
		break;
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

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeUnitData::_AddPieceData(const MkStr& imagePath, const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName)
{
	const MkImageInfo::Subset* subset = texture->GetImageInfo().GetCurrentSubsetPtr(subsetOrSequenceName);
	MK_CHECK(subset != NULL, L"잘못된 subset/sequence 지정 : " + imagePath + L" (" + subsetOrSequenceName.GetString() + L")")
		return false;

	PieceData& pd = m_PieceDatas.PushBack();
	pd.subsetOrSequenceName = subsetOrSequenceName;
	pd.size = subset->rectSize;
	return true;
}

void MkWindowThemeUnitData::_ApplyUnit(MkSceneNode* sceneNode, bool createOrGet, const MkHashStr& imagePath, double startTime) const
{
	switch (GetUnitType())
	{
	case eUT_Image:
		_SetImageToPiece(sceneNode, createOrGet, ImagePositionName, imagePath, m_PieceDatas[0].subsetOrSequenceName, true, startTime);
		break;

	case eUT_Table:
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_MC], imagePath, m_PieceDatas[eP_RB].subsetOrSequenceName, false, startTime);
	case eUT_Edge:
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_LT], imagePath, m_PieceDatas[eP_LT].subsetOrSequenceName, true, startTime);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_MT], imagePath, m_PieceDatas[eP_MT].subsetOrSequenceName, false, startTime);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_RT], imagePath, m_PieceDatas[eP_RT].subsetOrSequenceName, true, startTime);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_LC], imagePath, m_PieceDatas[eP_LC].subsetOrSequenceName, false, startTime);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_RC], imagePath, m_PieceDatas[eP_RC].subsetOrSequenceName, false, startTime);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_LB], imagePath, m_PieceDatas[eP_LB].subsetOrSequenceName, true, startTime);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_MB], imagePath, m_PieceDatas[eP_MB].subsetOrSequenceName, false, startTime);
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_RB], imagePath, m_PieceDatas[eP_RB].subsetOrSequenceName, true, startTime);
		break;
	}
}

void MkWindowThemeUnitData::_SetImageToPiece
(MkSceneNode* sceneNode, bool createOrGet, const MkHashStr& pieceName, const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName, bool keepSrcSize, double startTime)
{
	MkPanel* panel = sceneNode->PanelExist(pieceName) ?
		(createOrGet ? NULL : sceneNode->GetPanel(pieceName)) : // create
		(createOrGet ? &sceneNode->CreatePanel(pieceName) : NULL); // get

	if (panel != NULL)
	{
		// get일 경우 start time은 이전 값을 그대로 계승
		panel->SetTexture(imagePath, subsetOrSequenceName, createOrGet ? startTime : panel->GetSequenceStartTime(), 0.);
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
