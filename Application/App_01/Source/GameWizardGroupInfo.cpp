
//#include "MkCore_MkDataNode.h"

#include "GameWizardGroupInfo.h"


//------------------------------------------------------------------------------------------------//

void GameWizardGroupInfo::SetUp(const MkArray<GameWizardUnitInfo>& members, unsigned int targetWizardID)
{
	Clear();

	MK_INDEXING_LOOP(members, i)
	{
		const GameWizardUnitInfo& wizard = members[i];
		m_Members.Create(wizard.GetWizardID(), wizard);
	}

	m_TargetWizardID = targetWizardID;
	//MkDataNode initNode;
}

void GameWizardGroupInfo::Clear(void)
{
	m_Members.Clear();
}

GameWizardGroupInfo::GameWizardGroupInfo()
{
	m_TargetWizardID = 0;
}

//------------------------------------------------------------------------------------------------//
