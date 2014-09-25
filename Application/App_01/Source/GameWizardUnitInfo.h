#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalDefinition.h"

//#include "GameGlobalDefinition.h"


class GameWizardUnitInfo
{
public:

	bool SetUp(void); // tmp

	inline unsigned int GetWizardID(void) const { return m_WizardID; }
	inline ID64 GetUniqieID(void) const { return m_UniqueID; }
	
	GameWizardUnitInfo();
	~GameWizardUnitInfo() {}

protected:

	unsigned int m_WizardID;
	ID64 m_UniqueID;
};