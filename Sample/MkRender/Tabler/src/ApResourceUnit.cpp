
#include "MkCore_MkCheck.h"

#include "MkPA_MkSceneNode.h"

#include "ApStaticDefinition.h"
#include "ApResourceUnit.h"


//------------------------------------------------------------------------------------------------//

bool ApResourceUnit::SetUp(const ApResourceType& type, MkSceneNode* sceneNode, const MkPathName& imagePath)
{
	m_Type = type;
	m_SceneNode = sceneNode;
	m_ImagePath = imagePath;

	m_BackgroundType = eBT_None;
	m_RowColumnType = eRCT_None;
	m_TeamColor = eTC_None;
	m_SetType = eST_None;
	return true;
}

bool ApResourceUnit::SetBackgroundType(eBackgroundType bgType)
{
	if (bgType == m_BackgroundType)
		return true;

	MkHashStr ssName;
	switch (bgType)
	{
	case eBT_None: break;
	case eBT_Disable: ssName = L"Slot_Empty"; break;
	case eBT_Neutral: ssName = L"ResBG_Dark"; break;
	case eBT_Owned: ssName = L"ResBG_Normal"; break;
	case eBT_Combo: ssName = L"ResBG_Light"; break;
	}

	bool ok = _UpdateImagePanelToCenter(L"BG", ssName, 0.f);
	if (ok)
	{
		m_BackgroundType = bgType;
	}
	return ok;
}

bool ApResourceUnit::SetRowColumnType(eRowColumnType rcType)
{
	if (rcType == m_RowColumnType)
		return true;

	MkStr ssPrefix;
	switch (rcType)
	{
	case eRCT_None: break;
	case eRCT_Light: ssPrefix = L"ResIndexLight_"; break;
	case eRCT_Dark: ssPrefix = L"ResIndexDark_"; break;
	}

	if (ssPrefix.Empty())
	{
		if (m_SceneNode != NULL)
		{
			m_SceneNode->DeletePanel(L"Row");
			m_SceneNode->DeletePanel(L"Column");
		}
		m_RowColumnType = rcType;
		return true;
	}

	bool ok =
		(_AddImagePanelToLeftTop(L"Row", m_ImagePath, ssPrefix + MkStr(m_Type.GetRow()), -0.1f) &&
		_AddImagePanelToRightBottom(L"Column", m_ImagePath, ssPrefix + MkStr(m_Type.GetColumn()), -0.1f));
	if (ok)
	{
		m_RowColumnType = rcType;
	}
	return ok;
}

bool ApResourceUnit::SetTeamColor(eTeamColor teamColor)
{
	if (teamColor == m_TeamColor)
		return true;

	MkHashStr ssName;
	switch (teamColor)
	{
	case eTC_None: break;
	case eTC_Red: ssName = L"PlayerColor_0"; break;
	case eTC_Green: ssName = L"PlayerColor_1"; break;
	case eTC_Yellow: ssName = L"PlayerColor_2"; break;
	case eTC_Cyan: ssName = L"PlayerColor_3"; break;
	}

	bool ok = _UpdateImagePanelToCenter(L"TeamColor", ssName, -0.2f);
	if (ok)
	{
		m_TeamColor = teamColor;
	}
	return ok;
}

bool ApResourceUnit::SetSetType(eSetType setType)
{
	if (setType == m_SetType)
		return true;

	MkHashStr ssName;
	switch (setType)
	{
	case eST_None:
	case eST_2Set: break;
	case eST_3Set: ssName = L"Set3"; break;
	case eST_4Set: ssName = L"Set4"; break;
	case eST_5Set: ssName = L"Set5"; break;
	}

	bool ok = _UpdateImagePanelToCenter(L"Set", ssName, -0.3f);
	if (ok)
	{
		m_SetType = setType;
	}
	return ok;
}

void ApResourceUnit::ApplyCreationEffect(const MkTimeState& timeState)
{
	m_TimeCounter.SetUp(timeState, 0.3);

	if (m_SceneNode != NULL)
	{
		m_SceneNode->SetLocalDepth(-1.f);
	}
}

bool ApResourceUnit::FieldCommand_Disable(void)
{
	return (SetBackgroundType(eBT_Disable) && SetRowColumnType(eRCT_None) && SetTeamColor(eTC_None) && SetSetType(eST_None));
}

bool ApResourceUnit::FieldCommand_Neutral(void)
{
	return (SetBackgroundType(eBT_Neutral) && SetRowColumnType(eRCT_Dark) && SetTeamColor(eTC_None) && SetSetType(eST_None));
}

bool ApResourceUnit::FieldCommand_Own(eTeamColor teamColor)
{
	return (SetBackgroundType(eBT_Owned) && SetRowColumnType(eRCT_Light) && SetTeamColor(teamColor) && SetSetType(eST_2Set));
}

bool ApResourceUnit::FieldCommand_Upgrade(void)
{
	eSetType setType;
	switch (m_SetType)
	{
	case eST_None:
	case eST_5Set:
		return false;

	case eST_2Set: setType = eST_3Set; break;
	case eST_3Set: setType = eST_4Set; break;
	case eST_4Set: setType = eST_5Set; break;
	}
	return SetSetType(setType);
}

bool ApResourceUnit::FieldCommand_Combo(void)
{
	return SetBackgroundType(eBT_Combo);
}

bool ApResourceUnit::PlayerCommand_Wait(void)
{
	return (SetBackgroundType(eBT_Owned) && SetRowColumnType(eRCT_Light) && SetTeamColor(eTC_None) && SetSetType(eST_None));
}

bool ApResourceUnit::PlayerCommand_Pick(void)
{
	return (SetBackgroundType(eBT_Combo) && SetRowColumnType(eRCT_Light) && SetTeamColor(eTC_None) && SetSetType(eST_None));
}

bool ApResourceUnit::HitTest(const MkFloat2& hitPos) const
{
	if (m_SceneNode != NULL)
	{
		MkFloatRect rect;
		rect.position = m_SceneNode->GetWorldPosition() - ApStaticDefinition::Instance().GetResourceTileSize() * 0.5f;
		rect.size = ApStaticDefinition::Instance().GetResourceTileSize();
		return rect.CheckIntersection(hitPos);
	}
	return false;
}

void ApResourceUnit::Update(const MkTimeState& timeState)
{
	if (m_TimeCounter.GetTickCount() > 0.)
	{
		float localScale = 1.f;
		if (m_TimeCounter.OnTick(timeState))
		{
			m_TimeCounter.SetUp(timeState, 0.);

			if (m_SceneNode != NULL)
			{
				m_SceneNode->SetLocalDepth(0.f);
			}
		}
		else
		{
			float tickRatio = m_TimeCounter.GetTickRatio(timeState);
			
			if (tickRatio < 0.5f) // 0.f -> 0.5f
			{
				localScale += tickRatio * 0.5f; // 1.f -> 1.25f
			}
			else // 0.5f -> 1.f
			{
				localScale += (1.f - tickRatio) * 0.5f; // 1.25f -> 1.f
			}
		}

		if (m_SceneNode != NULL)
		{
			m_SceneNode->SetLocalScale(localScale);
		}
	}
}

ApResourceUnit::ApResourceUnit()
{
	m_SceneNode = NULL;
}

//------------------------------------------------------------------------------------------------//

bool ApResourceUnit::_AddImagePanel(const MkHashStr& name, const MkPathName& imgPath, const MkHashStr& ssName, float localDepth)
{
	if (m_SceneNode == NULL)
		return true;

	MkPanel& panel = m_SceneNode->CreatePanel(name);
	MK_CHECK(panel.SetTexture(imgPath, ssName), imgPath + L" : " + ssName.GetString() + L" 이미지 로딩 실패")
		return false;

	panel.SetLocalDepth(localDepth);
	return true;
}

bool ApResourceUnit::_AddImagePanelToCenter(const MkHashStr& name, const MkPathName& imgPath, const MkHashStr& ssName, float localDepth)
{
	if (m_SceneNode == NULL)
		return true;

	if (!_AddImagePanel(name, imgPath, ssName, localDepth))
		return false;

	MkPanel& panel = *m_SceneNode->GetPanel(name);
	const MkFloat2& panelSize = panel.GetPanelSize();
	panel.SetLocalPosition(panelSize * -0.5f);
	return true;
}

bool ApResourceUnit::_AddImagePanelToLeftTop(const MkHashStr& name, const MkPathName& imgPath, const MkHashStr& ssName, float localDepth)
{
	if (m_SceneNode == NULL)
		return true;

	if (!_AddImagePanel(name, imgPath, ssName, localDepth))
		return false;

	MkPanel& panel = *m_SceneNode->GetPanel(name);
	const MkFloat2& indexSize = panel.GetPanelSize();
	MkFloat2 localPos(0.f, ApStaticDefinition::Instance().GetResourceTileSize().y - indexSize.y); // left top
	localPos += MkFloat2(5.f, -5.f); // offset
	localPos -= ApStaticDefinition::Instance().GetResourceTileSize() * 0.5f; // pivot to center
	panel.SetLocalPosition(localPos);
	return true;
}

bool ApResourceUnit::_AddImagePanelToRightBottom(const MkHashStr& name, const MkPathName& imgPath, const MkHashStr& ssName, float localDepth)
{
	if (m_SceneNode == NULL)
		return true;

	if (!_AddImagePanel(name, imgPath, ssName, localDepth))
		return false;

	MkPanel& panel = *m_SceneNode->GetPanel(name);
	const MkFloat2& indexSize = panel.GetPanelSize();
	MkFloat2 localPos(ApStaticDefinition::Instance().GetResourceTileSize().x - indexSize.x, 0.f); // right bottom
	localPos += MkFloat2(-5.f, 5.f); // offset
	localPos -= ApStaticDefinition::Instance().GetResourceTileSize() * 0.5f; // pivot to center
	panel.SetLocalPosition(localPos);
	return true;
}

bool ApResourceUnit::_UpdateImagePanelToCenter(const MkHashStr& name, const MkHashStr& ssName, float localDepth)
{
	if (ssName.Empty())
	{
		if (m_SceneNode != NULL)
		{
			m_SceneNode->DeletePanel(name);
		}
		return true;
	}
	return _AddImagePanelToCenter(name, m_ImagePath, ssName, localDepth);
}

//------------------------------------------------------------------------------------------------//