#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"

#include "GameWizardUnitInfo.h"


class GameWizardGroupInfo
{
public:

	bool Load(const MkDataNode& node);
	bool Save(MkDataNode& node) const;

	void SetUp(const MkArray<GameWizardUnitInfo>& members, unsigned int targetWizardID);

	void Clear(void);

	inline void SetTargetWizard(unsigned int targetWizardID) { m_TargetWizardID = targetWizardID; }
	inline unsigned int GetTargetWizard(void) const { return m_TargetWizardID; }

	inline MkMap<unsigned int, GameWizardUnitInfo>& GetMembers(void) { return m_Members; }

	inline const GameWizardUnitInfo& GetWizardInfo(void) const { return m_Members[m_TargetWizardID]; }
	
	GameWizardGroupInfo();
	~GameWizardGroupInfo() { Clear(); }

protected:

	MkMap<unsigned int, GameWizardUnitInfo> m_Members; // wizard id, info

	unsigned int m_TargetWizardID;
};