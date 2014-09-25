
//#include "MkCore_MkDataNode.h"

#include "GameWizardGroupInfo.h"


//------------------------------------------------------------------------------------------------//

bool GameWizardGroupInfo::SetUp(const MkArray<GameWizardUnitInfo>& members, unsigned int targetWizardID)
{
	MK_INDEXING_LOOP(members, i)
	{
		const GameWizardUnitInfo& wizard = members[i];
		m_Members.Create(wizard.GetWizardID(), wizard);
	}

	m_TargetWizardID = targetWizardID;
	//MkDataNode initNode;

	return true;
}

GameWizardGroupInfo::GameWizardGroupInfo()
{
	m_TargetWizardID = 0;
}

//------------------------------------------------------------------------------------------------//
