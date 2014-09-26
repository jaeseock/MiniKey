#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"

#include "GameAgentUnitInfo.h"


class GameAgentGroupInfo
{
public:

	void SetUp(const MkArray<GameAgentUnitInfo>& members, unsigned int maxTroopers);

	void SortBy(eGroupSortingMethod gsm);

	void Clear(void);

	//inline const GameAgentUnitInfo* GetAgentUnitInfo(unsigned int gameID) const { return m_Members.Exist(gameID) ? &m_Members[gameID] : NULL;  }

	inline const MkArray<GameAgentUnitInfo*>& GetMemberSequence(void) const { return m_MemberSequence; }

	bool GetTroopers(MkArray<const GameAgentUnitInfo*>& troopers) const;
	
	GameAgentGroupInfo();
	~GameAgentGroupInfo() { Clear(); }

protected:

	MkMap<unsigned int, GameAgentUnitInfo> m_Members; // agent game id, info

	eGroupSortingMethod m_GroupSortingMethod;
	MkArray<GameAgentUnitInfo*> m_MemberSequence;

	unsigned int m_MaxTroopers;
};