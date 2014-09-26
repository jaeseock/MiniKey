
//#include "MkCore_MkGlobalFunction.h"
//#include "MkCore_MkCheck.h"
//#include "MkCore_MkTimeManager.h"

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

void MasterPlayer::Clear(void)
{
	m_WizardGroup.Clear();
	m_AgentGroup.Clear();
}

//------------------------------------------------------------------------------------------------//
