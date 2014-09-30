
//#include "MkCore_MkGlobalFunction.h"
//#include "MkCore_MkCheck.h"
//#include "MkCore_MkTimeManager.h"

#include "MkCore_MkDataNode.h"

#include "GameSystemManager.h"


//------------------------------------------------------------------------------------------------//

void NormalPlayer::SetUp(const GameWizardUnitInfo& wizard, const MkArray<GameAgentUnitInfo>& troop)
{
	m_WizardInfo = wizard;
	m_TroopInfo = troop;

	m_Troopers.Clear();
	if (!m_TroopInfo.Empty())
	{
		m_Troopers.Reserve(m_TroopInfo.GetSize());

		MK_INDEXING_LOOP(m_TroopInfo, i)
		{
			m_Troopers.PushBack(&m_TroopInfo[i]);
		}
	}
}

//------------------------------------------------------------------------------------------------//

const static MkHashStr WIZARD_GROUP_KEY = L"WizardGroup";

bool MasterPlayer::Load(const MkDataNode& node)
{
	if ((!node.ChildExist(WIZARD_GROUP_KEY)) || (!m_WizardGroup.Load(*node.GetChildNode(WIZARD_GROUP_KEY))))
		return false;

	return true;
}

bool MasterPlayer::Save(MkDataNode& node)
{
	MkDataNode* wizardGroupNode = node.ChildExist(WIZARD_GROUP_KEY) ? node.GetChildNode(WIZARD_GROUP_KEY) : node.CreateChildNode(WIZARD_GROUP_KEY);
	if (!m_WizardGroup.Save(*wizardGroupNode))
		return false;

	return true;
}

void MasterPlayer::Clear(void)
{
	m_WizardGroup.Clear();
	m_AgentGroup.Clear();
}

//------------------------------------------------------------------------------------------------//
