#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalDefinition.h"

//#include "GameGlobalDefinition.h"


class MkDataNode;

class GameWizardUnitInfo
{
public:

	bool Load(const MkDataNode& node);
	bool Save(MkDataNode& node);

	inline unsigned int GetWizardID(void) const { return m_WizardID; }
	inline ID64 GetUniqieID(void) const { return m_UniqueID; }

	inline int GetWizardLevel(void) const { return m_WizardLevel; }
	
	GameWizardUnitInfo();
	~GameWizardUnitInfo() {}

protected:

	unsigned int m_WizardID;
	ID64 m_UniqueID;

	int m_WizardLevel;
};