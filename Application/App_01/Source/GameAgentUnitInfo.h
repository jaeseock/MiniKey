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

	inline unsigned int GetAgentID(void) const { return m_AgentID; }

	inline int GetAgentLevel(void) const { return m_AgentLevel; }

	inline void SetDopingLevel(int level) { m_DopingLevel = level; }
	inline int GetDopingLevel(void) const { return m_DopingLevel; }

	GameAgentUnitInfo();
	~GameAgentUnitInfo() {}

protected:

	unsigned int m_AgentID;

	int m_AgentLevel;
	int m_DopingLevel;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(GameAgentUnitInfo)