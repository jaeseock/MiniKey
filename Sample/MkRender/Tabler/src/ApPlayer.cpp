
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkPairArray.h"

#include "MkPA_MkSceneNode.h"
#include "MkPA_MkTextNode.h"

#include "ApStaticDefinition.h"
#include "ApPlayer.h"


//------------------------------------------------------------------------------------------------//

bool ApPlayer::SetUp(unsigned int sequence, const MkStr& name, eStandPosition sPos, ApResourceUnit::eTeamColor teamColor, unsigned int initResSlotSize, MkSceneNode* parentSceneNode)
{
	MK_CHECK(parentSceneNode != NULL, L"parentSceneNode�� NULL��")
		return false;

	m_Name = name;
	m_StandPosition = sPos;
	m_TeamColor = teamColor;
	m_SceneNode = parentSceneNode->CreateChildNode(m_Name);
	MK_CHECK(m_SceneNode != NULL, L"m_SceneNode�� NULL��")
		return false;

	// slot node
	MkSceneNode* slotNode = m_SceneNode->CreateChildNode(L"Slot");
	MK_CHECK(slotNode != NULL, L"slotNode�� NULL��")
		return false;
	
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

	// pick node
	if (m_StandPosition == eSP_Down)
	{
		MkSceneNode* pickNode = m_SceneNode->CreateChildNode(L"Pick");
		MK_CHECK(pickNode != NULL, L"pickNode�� NULL��")
			return false;

		pickNode->SetLocalPosition(MkFloat2(0.f, 138.f) + ApStaticDefinition::Instance().GetResourceTileSize() * 0.5f);
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

bool ApPlayer::AddResource(unsigned int key, const ApResourceType& type, const MkTimeState& timeState)
{
	bool ok = ((m_SceneNode != NULL) && HasEmptyResourceSlot());
	if (ok)
	{
		MkSceneNode* slotNode = m_SceneNode->GetChildNode(L"Slot");
		MK_CHECK(slotNode != NULL, L"slotNode�� NULL��")
			return false;

		MkSceneNode* resNode = slotNode->CreateChildNode(MkStr(key));
		MK_CHECK(resNode != NULL, L"resNode�� NULL��")
			return false;

		ApResourceUnit& resUnit = m_ResourcePool.Create(key);
		resUnit.SetUp(type, resNode, L"Image\\stage_total.png");
		resUnit.ApplyCreationEffect(timeState);

		//
		resUnit.PlayerCommand_Wait();
		//

		m_ResourceSlot.PushBack(key);
	}
	return ok;
}

bool ApPlayer::SelectResource(unsigned int slotIndex)
{
	MK_DEV_PANEL.MsgToLog(MkStr(slotIndex));

	if (!m_ResourceSlot.IsValidIndex(slotIndex))
		return false;

	MkSceneNode* pickNode = m_SceneNode->GetChildNode(L"Pick");
	
	unsigned int key = m_ResourceSlot[slotIndex];
	ApResourceUnit& targetUnit = m_ResourcePool[key];

	// select
	if (targetUnit.GetBackgroundType() == ApResourceUnit::eBT_Owned)
	{
		if (m_SelectionSlot.GetSize() == 6) // max
			return false;

		m_SelectionSlot.PushBack(key);
		
		if (!targetUnit.PlayerCommand_Pick())
			return false;

		ApResourceUnit& newUnit = m_SelectionPool.Create(key);
		MkSceneNode* resNode = NULL;
		if (pickNode != NULL)
		{
			resNode = pickNode->CreateChildNode(MkStr(key));
			MK_CHECK(resNode != NULL, L"resNode�� NULL��")
				return false;
		}
		newUnit.SetUp(targetUnit.GetType(), resNode, L"Image\\stage_total.png");
		newUnit.PlayerCommand_Wait();
	}
	// cancel
	else
	{
		if (!targetUnit.PlayerCommand_Wait())
			return false;

		if (pickNode != NULL)
		{
			pickNode->RemoveChildNode(MkStr(key));
		}

		m_SelectionPool.Erase(key);

		unsigned int selPos = m_SelectionSlot.FindFirstInclusion(MkArraySection(0), key);
		if (selPos != MKDEF_ARRAY_ERROR)
		{
			m_SelectionSlot.Erase(MkArraySection(selPos, 1));
		}
	}

	_UpdatePickNode(pickNode);
	_FindSet();
	return true;
}

bool ApPlayer::SelectResource(const MkFloat2& hitPos)
{
	MK_INDEXING_LOOP(m_ResourceSlot, i)
	{
		if (m_ResourcePool[m_ResourceSlot[i]].HitTest(hitPos))
		{
			return SelectResource(i);
		}
	}
	return false;
}

void ApPlayer::ClearSelection(void)
{
	MkSceneNode* pickNode = m_SceneNode->GetChildNode(L"Pick");
	if (pickNode != NULL)
	{
		pickNode->Clear();
	}
	m_SelectionPool.Clear();
	m_SelectionSlot.Clear();
}

void ApPlayer::ConsumeSelection(void)
{
	MkSceneNode* slotNode = m_SceneNode->GetChildNode(L"Slot");
	MK_INDEXING_LOOP(m_SelectionSlot, i)
	{
		unsigned int key = m_SelectionSlot[i];
		if (slotNode != NULL)
		{
			slotNode->RemoveChildNode(MkStr(key));
		}
		m_ResourcePool.Erase(key);

		unsigned int slotPos = m_ResourceSlot.FindFirstInclusion(MkArraySection(0), key);
		if (slotPos != MKDEF_ARRAY_ERROR)
		{
			m_ResourceSlot.Erase(MkArraySection(slotPos, 1));
		}
	}

	if (!m_SelectionSlot.Empty())
	{
		UpdateResourceSlotPosition();
	}

	ClearSelection();
}

void ApPlayer::UpdateResourceSlotPosition(void)
{
	// sort
	MkPairArray<unsigned int, unsigned int> sorter(m_ResourceSlot.GetSize()); // type, key
	MK_INDEXING_LOOP(m_ResourceSlot, i)
	{
		unsigned int key = m_ResourceSlot[i];
		sorter.PushBack(m_ResourcePool[key].GetType().GetValue(), key);
	}
	sorter.SortInAscendingOrder();
	m_ResourceSlot = sorter.GetFields();

	// reposition
	for (unsigned int i=0; i<ApStaticDefinition::Instance().GetMaxResourceSlotSize(); ++i)
	{
		_UpdateSlotPosition(i);
	}
}

void ApPlayer::Update(const MkTimeState& timeState)
{
	MkMapLooper<unsigned int, ApResourceUnit> looper(m_ResourcePool);
	MK_STL_LOOP(looper)
	{
		looper.GetCurrentField().Update(timeState);
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

void ApPlayer::_UpdatePickNode(MkSceneNode* pickNode)
{
	if (pickNode != NULL)
	{
		MK_INDEXING_LOOP(m_SelectionSlot, i)
		{
			unsigned int key = m_SelectionSlot[i];
			MkSceneNode* resNode = pickNode->GetChildNode(MkStr(key));
			if (resNode != NULL)
			{
				resNode->SetLocalPosition(MkFloat2(ApStaticDefinition::Instance().GetResourceTileSize().x * static_cast<float>(i), 0.f));
			}
		}
	}
}

void ApPlayer::_FindSet(void)
{
}

//------------------------------------------------------------------------------------------------//