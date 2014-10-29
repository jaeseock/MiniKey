#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"

#include "GameGlobalDefinition.h"
#include "GameAgentUnitInfo.h"


class GameAgentGroupInfo
{
public:

	bool Load(const MkDataNode& node);
	bool Save(MkDataNode& node) const;

	void SetUp(const MkArray<GameAgentUnitInfo>& members);

	void SortBy(eGroupSortingMethod gsm);

	void Clear(void);

	inline const MkArray<const GameAgentUnitInfo*>& GetMemberSequence(void) const { return m_MemberSequence; }

	inline bool IsTrooper(unsigned int agentID) const { return m_Troopers.Exist(agentID); }
	bool SetTrooper(unsigned int agentID, bool enable);

	inline unsigned int GetCurrTroopers(void) const { return m_Troopers.GetSize(); }

	inline const MkArray<GameAgentUnitInfo>& GetAgentInfo(void) const { return m_ReadyToBattleAgents; }
	
	GameAgentGroupInfo();
	~GameAgentGroupInfo() { Clear(); }

protected:

	MkMap<unsigned int, GameAgentUnitInfo> m_Members; // agent id, info

	MkMap<unsigned int, unsigned int> m_Troopers; // agent id, ready to battle agent index
	MkArray<GameAgentUnitInfo> m_ReadyToBattleAgents;

	eGroupSortingMethod m_GroupSortingMethod;
	MkArray<const GameAgentUnitInfo*> m_MemberSequence;
};