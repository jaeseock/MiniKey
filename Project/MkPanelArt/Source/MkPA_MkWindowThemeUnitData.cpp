
#include "MkCore_MkCheck.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkBaseTexture.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkWindowThemeUnitData.h"

const MkStr MkWindowThemeUnitData::WindowThemeTagPrefix(MKDEF_PA_WINDOW_THEME_TAG_PREFIX);

const MkHashStr MkWindowThemeUnitData::ImagePositionName = WindowThemeTagPrefix + L"IMG";

const MkHashStr MkWindowThemeUnitData::EdgeAndTablePositionName[9] =
{
	WindowThemeTagPrefix + L"LT", // eP_LT
	WindowThemeTagPrefix + L"MT", // eP_MT
	WindowThemeTagPrefix + L"RT", // eP_RT
	WindowThemeTagPrefix + L"LC", // eP_LC
	WindowThemeTagPrefix + L"MC", // eP_MC
	WindowThemeTagPrefix + L"RC", // eP_RC
	WindowThemeTagPrefix + L"LB", // eP_LB
	WindowThemeTagPrefix + L"MB", // eP_MB
	WindowThemeTagPrefix + L"RB" // eP_RB
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

			MK_CHECK(false, L"MkWindowThemeUnitData�� ������ 1, 8, 9���� subset/sequence �ʿ�")
				return false;
		}
		break;
	}

	m_PieceDatas.Reserve(count);

	for (unsigned int i=0; i<count; ++i)
	{
		if ((i == eP_MC) && (m_UnitType == eUT_Edge))
		{
			m_PieceDatas.PushBack(); // eUT_Edge�� ��� middle-center�� �ش��ϴ� ������ ��� ��
		}

		const MkHashStr& currSSName = subsetOrSequenceNameList[i];
		const MkImageInfo::Subset* subset = texture->GetImageInfo().GetCurrentSubsetPtr(currSSName);
		MK_CHECK(subset != NULL, L"�߸��� subset/sequence ���� : " + imagePath + L" (" + currSSName.GetString() + L")")
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
		// size ����
		MK_CHECK((m_PieceDatas[eP_LT].size.x == m_PieceDatas[eP_LC].size.x) && (m_PieceDatas[eP_LC].size.x == m_PieceDatas[eP_LB].size.x), L"LT, LC, LB�� width�� �����ؾ� ��")
			return false;
		MK_CHECK((m_PieceDatas[eP_RT].size.x == m_PieceDatas[eP_RC].size.x) && (m_PieceDatas[eP_RC].size.x == m_PieceDatas[eP_RB].size.x), L"RT, RC, RB�� width�� �����ؾ� ��")
			return false;
		MK_CHECK((m_PieceDatas[eP_LT].size.y == m_PieceDatas[eP_MT].size.y) && (m_PieceDatas[eP_MT].size.y == m_PieceDatas[eP_RT].size.y), L"LT, MT, RT�� height�� �����ؾ� ��")
			return false;
		MK_CHECK((m_PieceDatas[eP_LB].size.y == m_PieceDatas[eP_MB].size.y) && (m_PieceDatas[eP_MB].size.y == m_PieceDatas[eP_RB].size.y), L"LB, MB, RB�� height�� �����ؾ� ��")
			return false;

		// panel position pivot �Ҵ�. client size �ݿ��ÿ���,
		// - LC, MC, LB, MB�� pivot�� �״�� ���
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
	else if (sceneNode->PanelExist(EdgeAndTablePositionName[eP_LT])) // left-top piece�� ������ ������
	{
		unitType = sceneNode->PanelExist(EdgeAndTablePositionName[eP_MC]) ? eUT_Table : eUT_Edge; // middle-center piece ������ ���� ����
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
	MK_CHECK(nodeUnitType == GetUnitType(), sceneNode->GetNodeName().GetString() + L" node�� ����� unit type�� theme�� unit type�� �ٸ�")
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

void MkWindowThemeUnitData::_ApplyUnit(MkSceneNode* sceneNode, bool createOrGet, const MkHashStr& imagePath, double startTime) const
{
	switch (GetUnitType())
	{
	case eUT_Image:
		_SetImageToPiece(sceneNode, createOrGet, ImagePositionName, imagePath, m_PieceDatas[0].subsetOrSequenceName, false, startTime);
		break;

	case eUT_Table:
		_SetImageToPiece(sceneNode, createOrGet, EdgeAndTablePositionName[eP_MC], imagePath, m_PieceDatas[eP_MC].subsetOrSequenceName, false, startTime);
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
		// subsetOrSequenceName�� ����� ��� null texture ����(panel ������ ���������� �׷������� ����)
		// get�� ��� start time�� ���� ���� �״�� ���
		panel->SetTexture(subsetOrSequenceName.Empty() ? MkHashStr::EMPTY : imagePath, subsetOrSequenceName, createOrGet ? startTime : panel->GetSequenceStartTime(), 0.);
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
