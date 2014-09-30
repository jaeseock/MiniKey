#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"

#include "GameWizardUnitInfo.h"


class MkDataNode;

class GameWizardGroupInfo
{
public:

	bool Load(const MkDataNode& node);
	bool Save(MkDataNode& node);

	void SetUp(const MkArray<GameWizardUnitInfo>& members, unsigned int targetWizardID);

	void Clear(void);

	inline void SetTargetWizard(unsigned int targetWizardID) { m_TargetWizardID = targetWizardID; }
	inline unsigned int GetTargetWizard(void) const { return m_TargetWizardID; }

	inline const GameWizardUnitInfo* GetTargetWizardInfo(void) const { return m_Members.Exist(m_TargetWizardID) ? &m_Members[m_TargetWizardID] : NULL; }

	inline MkMap<unsigned int, GameWizardUnitInfo>& GetMembers(void) { return m_Members; }
	
	GameWizardGroupInfo();
	~GameWizardGroupInfo() { Clear(); }

protected:

	MkMap<unsigned int, GameWizardUnitInfo> m_Members; // wizard id, info

	unsigned int m_TargetWizardID;
};