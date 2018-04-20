
#include "MkCore_MkCheck.h"

#include "MkPA_MkSceneNode.h"
#include "MkPA_MkTextNode.h"

#include "ApStaticDefinition.h"
#include "ApPlayer.h"


//------------------------------------------------------------------------------------------------//

bool ApPlayer::SetUp(unsigned int sequence, const MkStr& name, eStandPosition sPos, ApResourceUnit::eTeamColor teamColor, unsigned int initResSlotSize, MkSceneNode* parentSceneNode)
{
	MK_CHECK(parentSceneNode != NULL, L"parentSceneNode가 NULL임")
		return false;

	m_Name = name;
	m_StandPosition = sPos;
	m_TeamColor = teamColor;
	m_SceneNode = parentSceneNode->CreateChildNode(m_Name);
	MK_CHECK(m_SceneNode != NULL, L"m_SceneNode가 NULL임")
		return false;

	MkSceneNode* slotNode = m_SceneNode->CreateChildNode(L"Slot");
	MK_CHECK(slotNode != NULL, L"slotNode가 NULL임")
		return false;

	// position
	MkFloat2 localPos;
	switch (m_StandPosition)
	{
	case eSP_Down: localPos = MkFloat2(48.f, 10.f); break;
	case eSP_Top: localPos = MkFloat2(48.f, 690.f); break;
	case eSP_Left: localPos = MkFloat2(26.f, 614.f); break;
	case eSP_Right: localPos = MkFloat2(424.f, 614.f); break;
	}
	m_SceneNode->SetLocalPosition(localPos);

	if (m_StandPosition != eSP_Down)
	{
		slotNode->SetLocalScale(0.5f);
	}

	// empty slot
	for (unsigned int i=0; i<ApStaticDefinition::Instance().GetMaxResourceSlotSize(); ++i)
	{
		MkPanel& panel = slotNode->CreatePanel(L"Empty_" + MkStr(i));
		panel.SetTexture(L"Image\\stage_total.png", L"Slot_Empty");
	}

	// name panel
	MkPanel& namePanel = m_SceneNode->CreatePanel(L"Name");
	MkTextNode textNode;
	textNode.SetFontType(L"Large");
	switch (teamColor)
	{
	case ApResourceUnit::eTC_None: textNode.SetFontStyle(L"AP:Stage:NameObs"); break;
	case ApResourceUnit::eTC_Red: textNode.SetFontStyle(L"AP:Stage:NameRed"); break;
	case ApResourceUnit::eTC_Green: textNode.SetFontStyle(L"AP:Stage:NameGreen"); break;
	case ApResourceUnit::eTC_Yellow: textNode.SetFontStyle(L"AP:Stage:NameYellow"); break;
	case ApResourceUnit::eTC_Cyan: textNode.SetFontStyle(L"AP:Stage:NameCyan"); break;
	}
	textNode.SetText(L"(" + MkStr(sequence + 1) + L") " + m_Name);
	namePanel.SetTextNode(textNode);
	
	switch (m_StandPosition)
	{
	case eSP_Down: localPos = MkFloat2(0.f, 212.f); break;
	case eSP_Top: localPos = MkFloat2(0.f, -22.f); break;
	case eSP_Left: localPos = MkFloat2(40.f, 14.f); break;
	case eSP_Right:
		{
			namePanel.BuildAndUpdateTextCache();
			const MkFloat2& texSize = namePanel.GetTextureSize();
			localPos = MkFloat2(-texSize.x - 12.f, 14.f);
		}
		break;
	}
	namePanel.SetLocalPosition(localPos);

	// hp panel
	MkPanel& hpPanel = m_SceneNode->CreatePanel(L"HP");
	textNode;
	textNode.SetFontType(L"Large");
	textNode.SetFontStyle(L"AP:Stage:HP");
	textNode.SetText(L"12345/12345");
	hpPanel.SetTextNode(textNode);

	switch (m_StandPosition)
	{
	case eSP_Down: localPos.x += 150.f; break;
	case eSP_Top: localPos.x += 150.f; break;
	case eSP_Left: localPos.y -= 32.f; break;
	case eSP_Right:
		{
			hpPanel.BuildAndUpdateTextCache();
			const MkFloat2& texSize = hpPanel.GetTextureSize();
			localPos.x = -texSize.x - 12.f;
			localPos.y -= 32.f;
		}
		break;
	}
	hpPanel.SetLocalPosition(localPos);

	// joker panel
	MkPanel& jokerPanel = m_SceneNode->CreatePanel(L"Joker");
	textNode.SetFontType(L"Large");
	textNode.SetFontStyle(L"AP:Stage:Joker");
	textNode.SetText(MkStr(static_cast<int>(sPos)) + L"/5");
	jokerPanel.SetTextNode(textNode);

	switch (m_StandPosition)
	{
	case eSP_Down: localPos.x += 105.f; break;
	case eSP_Top: localPos.x += 105.f; break;
	case eSP_Left: localPos.x += 105.f; break;
	case eSP_Right: localPos.x -= 40.f; break;
	}
	jokerPanel.SetLocalPosition(localPos);

	m_ResourceSlot.Reserve(ApStaticDefinition::Instance().GetMaxResourceSlotSize());
	m_MaxResourceSlotSize = initResSlotSize;

	return true;
}

bool ApPlayer::HasEmptyResourceSlot(void) const
{
	return (m_ResourceSlot.GetSize() < m_MaxResourceSlotSize);
}

bool ApPlayer::AddResource(unsigned int key, const ApResourceType& type)
{
	bool ok = ((m_SceneNode != NULL) && HasEmptyResourceSlot());
	if (ok)
	{
		MkSceneNode* slotNode = m_SceneNode->GetChildNode(L"Slot");
		MK_CHECK(slotNode != NULL, L"slotNode가 NULL임")
			return false;

		MkSceneNode* resNode = slotNode->CreateChildNode(MkStr(key));
		MK_CHECK(resNode != NULL, L"resNode가 NULL임")
			return false;

		ApResourceUnit& resUnit = m_ResourcePool.Create(key);
		resUnit.SetUp(type, resNode, L"Image\\stage_total.png");

		//
		resUnit.PlayerCommand_Wait();
		//

		m_ResourceSlot.PushBack(key);
	}
	return ok;
}

void ApPlayer::UpdateSlotPosition(void)
{
	for (unsigned int i=0; i<ApStaticDefinition::Instance().GetMaxResourceSlotSize(); ++i)
	{
		_UpdateSlotPosition(i);
	}
}

ApPlayer::ApPlayer()
{
	
}

//------------------------------------------------------------------------------------------------//

void ApPlayer::_UpdateSlotPosition(unsigned int index)
{
	MkSceneNode* slotNode = m_SceneNode->GetChildNode(L"Slot");
	MkPanel* panel = slotNode->GetPanel(L"Empty_" + MkStr(index));

	if (m_ResourceSlot.IsValidIndex(index))
	{
		unsigned int key = m_ResourceSlot[index];
		MkSceneNode* resNode = slotNode->GetChildNode(MkStr(key));
		resNode->SetLocalPosition(_GetResourceSlotPosition(index));
		panel->SetVisible(false);
	}
	else if (index < m_MaxResourceSlotSize)
	{
		panel->SetLocalPosition(_GetEmptySlotPosition(index));
		panel->SetVisible(true);
	}
	else
	{
		panel->SetVisible(false);
	}
}

MkFloat2 ApPlayer::_GetResourceSlotPosition(unsigned int index) const
{
	return (_GetEmptySlotPosition(index) + ApStaticDefinition::Instance().GetResourceTileSize() * 0.5f);
}

MkFloat2 ApPlayer::_GetEmptySlotPosition(unsigned int index) const
{
	MkFloat2 localPos;
	switch (m_StandPosition)
	{
	case eSP_Down:
		localPos.x = ApStaticDefinition::Instance().GetResourceTileSize().x * static_cast<float>(index % 6);
		localPos.y = (index < 6) ? ApStaticDefinition::Instance().GetResourceTileSize().y : 0.f;
		break;

	case eSP_Top:
		localPos.x = ApStaticDefinition::Instance().GetResourceTileSize().x * static_cast<float>(index);
		break;

	case eSP_Left:
	case eSP_Right:
		localPos.y = -ApStaticDefinition::Instance().GetResourceTileSize().y * static_cast<float>(index);
		break;
	}
	return localPos;
}

//------------------------------------------------------------------------------------------------//