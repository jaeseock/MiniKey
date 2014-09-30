
//#include "MkCore_MkDataNode.h"

#include "GameAgentUnitInfo.h"


//------------------------------------------------------------------------------------------------//

bool GameAgentUnitInfo::SetUp(void)
{
	//MkDataNode initNode;

	return true;
}

GameAgentUnitInfo::GameAgentUnitInfo()
{
	m_GameID = 0;
	m_UniqueID = 0;
	
	m_Name = L"Undefined";
	
	m_AgentLevel = -1;
	m_MemberTypeLevel[eTMT_Attack] = 10;
	m_MemberTypeLevel[eTMT_Defense] = 10;
	m_MemberTypeLevel[eTMT_Support] = 10;
	m_MemberTypeLevel[eTMT_Resource] = 10;
	m_Trooper = false;

	m_ViewSetID = 0;
}

//------------------------------------------------------------------------------------------------//
