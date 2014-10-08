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

	//inline const GameAgentUnitInfo* GetAgentUnitInfo(unsigned int gameID) const { return m_Members.Exist(gameID) ? &m_Members[gameID] : NULL;  }

	inline const MkArray<const GameAgentUnitInfo*>& GetMemberSequence(void) const { return m_MemberSequence; }

	inline bool IsTrooper(unsigned int agentID) const { return m_Troopers.Exist(agentID); }
	bool SetTrooper(unsigned int agentID, bool enable);

	void GetTroopers(MkArray<GameAgentUnitInfo>& buffer) const;

	inline unsigned int GetMaxTroopers(void) const { return m_MaxTroopers; }
	inline unsigned int GetCurrTroopers(void) const { return m_Troopers.GetSize(); }
	
	GameAgentGroupInfo();
	~GameAgentGroupInfo() { Clear(); }

protected:

	MkMap<unsigned int, GameAgentUnitInfo> m_Members; // agent id, info
	MkMap<unsigned int, GameAgentUnitInfo*> m_Troopers; // agent id, info ptr

	eGroupSortingMethod m_GroupSortingMethod;
	MkArray<const GameAgentUnitInfo*> m_MemberSequence;

	unsigned int m_MaxTroopers;
	unsigned int m_CurrTroopers;
};