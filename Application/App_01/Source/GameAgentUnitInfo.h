#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalDefinition.h"
#include "MkCore_MkStr.h"

//#include "GameGlobalDefinition.h"


class MkDataNode;

class GameAgentUnitInfo
{
public:

	bool Load(const MkDataNode& node);
	bool Save(MkDataNode& node) const;

	inline void SetAgentID(unsigned int id) { m_AgentID = id; }
	inline unsigned int GetAgentID(void) const { return m_AgentID; }

	inline void SetAgentLevel(int level) { m_AgentLevel = level; }
	inline int GetAgentLevel(void) const { return m_AgentLevel; }

	GameAgentUnitInfo();
	~GameAgentUnitInfo() {}

protected:

	unsigned int m_AgentID;

	int m_AgentLevel;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(GameAgentUnitInfo)