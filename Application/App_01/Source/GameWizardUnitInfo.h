#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

//#include "MkCore_MkGlobalDefinition.h"

//#include "GameGlobalDefinition.h"


class MkDataNode;

class GameWizardUnitInfo
{
public:

	bool Load(const MkDataNode& node);
	bool Save(MkDataNode& node) const;

	inline unsigned int GetWizardID(void) const { return m_WizardID; }
	
	inline int GetWizardLevel(void) const { return m_WizardLevel; }
	
	GameWizardUnitInfo();
	~GameWizardUnitInfo() {}

protected:

	unsigned int m_WizardID;
	int m_WizardLevel;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(GameWizardUnitInfo)