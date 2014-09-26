#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"

#include "GameWizardUnitInfo.h"


class GameWizardGroupInfo
{
public:

	void SetUp(const MkArray<GameWizardUnitInfo>& members, unsigned int targetWizardID);

	void Clear(void);

	inline void SelectTargetWizard(unsigned int targetWizardID) { m_TargetWizardID = targetWizardID; }

	inline const GameWizardUnitInfo* GetTargetWizardInfo(void) const { return m_Members.Exist(m_TargetWizardID) ? &m_Members[m_TargetWizardID] : NULL; }
	
	GameWizardGroupInfo();
	~GameWizardGroupInfo() { Clear(); }

protected:

	MkMap<unsigned int, GameWizardUnitInfo> m_Members; // wizard game id, info

	unsigned int m_TargetWizardID;
};