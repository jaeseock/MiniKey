#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalDefinition.h"
#include "MkCore_MkStr.h"

#include "GameGlobalDefinition.h"


class MkDataNode;

class GameAgentUnitInfo
{
public:

	//bool Load(const MkDataNode& node);
	//bool Save(MkDataNode& node);

	bool SetUp(void); // tmp

	inline unsigned int GetGameID(void) const { return m_GameID; }
	inline ID64 GetUniqieID(void) const { return m_UniqueID; }
	inline const MkStr& GetName(void) const { return m_Name; }
	inline int GetAgentLevel(void) const { return m_AgentLevel; }
	inline int GetMemberTypeLevel(eTroopMemberType tmt) const { return m_MemberTypeLevel[tmt]; }
	inline unsigned int GetViewSetID(void) const { return m_ViewSetID; }
	inline bool IsTrooper(void) const { return m_Trooper; }
	
	GameAgentUnitInfo();
	~GameAgentUnitInfo() {}

protected:

	unsigned int m_GameID;
	ID64 m_UniqueID;
	
	MkStr m_Name;

	int m_AgentLevel;
	int m_MemberTypeLevel[eTMT_Max];
	bool m_Trooper;

	unsigned int m_ViewSetID;
};