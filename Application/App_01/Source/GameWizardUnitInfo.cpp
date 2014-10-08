
#include "MkCore_MkDataNode.h"

#include "GameWizardUnitInfo.h"


const static MkHashStr WIZARD_LEVEL = L"WizardLevel";


//------------------------------------------------------------------------------------------------//

bool GameWizardUnitInfo::Load(const MkDataNode& node)
{
	m_WizardID = node.GetNodeName().GetString().ToInteger();

	m_WizardLevel = 1;
	if (!node.GetData(WIZARD_LEVEL, m_WizardLevel, 0))
		return false;

	return true;
}

bool GameWizardUnitInfo::Save(MkDataNode& node) const
{
	if (!node.CreateUnit(WIZARD_LEVEL, m_WizardLevel))
	{
		node.SetData(WIZARD_LEVEL, m_WizardLevel, 0);
	}

	return true;
}

GameWizardUnitInfo::GameWizardUnitInfo()
{
	m_WizardID = 0;
	m_WizardLevel = 1;
}

//------------------------------------------------------------------------------------------------//
