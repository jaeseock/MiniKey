
#include "MkCore_MkDataNode.h"

#include "GameAgentUnitInfo.h"

const static MkHashStr AGENT_LEVEL = L"AgentLevel";


//------------------------------------------------------------------------------------------------//

bool GameAgentUnitInfo::Load(const MkDataNode& node)
{
	m_AgentID = node.GetNodeName().GetString().ToInteger();

	m_AgentLevel = 1;
	if (!node.GetData(AGENT_LEVEL, m_AgentLevel, 0))
		return false;

	return true;
}

bool GameAgentUnitInfo::Save(MkDataNode& node) const
{
	if (!node.CreateUnit(AGENT_LEVEL, m_AgentLevel))
	{
		node.SetData(AGENT_LEVEL, m_AgentLevel, 0);
	}

	return true;
}

GameAgentUnitInfo::GameAgentUnitInfo()
{
	m_AgentID = 0;
	m_AgentLevel = 1;
}

//------------------------------------------------------------------------------------------------//
