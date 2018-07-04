#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkStr.h"

#include "ApResourceUnit.h"


class ApPlayer
{
public:

	enum eStandPosition
	{
		eSP_Down = 0,
		eSP_Top,
		eSP_Left,
		eSP_Right
	};

	bool SetUp(unsigned int sequence, const MkStr& name, eStandPosition sPos, ApResourceUnit::eTeamColor teamColor, unsigned int initResSlotSize, MkSceneNode* parentSceneNode);

	inline const MkStr& GetName(void) const { return m_Name; }
	inline unsigned int GetMaxResourceSlotSize(void) const { return m_MaxResourceSlotSize; }

	inline eStandPosition GetStandPosition(void) const { return m_StandPosition; }

	bool HasEmptyResourceSlot(void) const;
	bool AddResource(unsigned int key, const ApResourceType& type, const MkTimeState& timeState);

	bool SelectResource(unsigned int slotIndex);
	bool SelectResource(const MkFloat2& hitPos);

	void ClearSelection(void);
	void ConsumeSelection(void);

	void UpdateResourceSlotPosition(void);

	void Update(const MkTimeState& timeState);

	ApPlayer();

protected:

	void _UpdateSlotPosition(unsigned int index);
	MkFloat2 _GetResourceSlotPosition(unsigned int index) const;
	MkFloat2 _GetEmptySlotPosition(unsigned int index) const;

	void _UpdatePickNode(MkSceneNode* pickNode);
	void _FindSet(void);

protected:

	MkStr m_Name;
	eStandPosition m_StandPosition;
	ApResourceUnit::eTeamColor m_TeamColor;
	MkSceneNode* m_SceneNode;

	MkMap<unsigned int, ApResourceUnit> m_ResourcePool;
	MkArray<unsigned int> m_ResourceSlot;
	unsigned int m_MaxResourceSlotSize;

	MkMap<unsigned int, ApResourceUnit> m_SelectionPool;
	MkArray<unsigned int> m_SelectionSlot;
};
