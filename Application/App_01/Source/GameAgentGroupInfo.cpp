
//#include "MkCore_MkDataNode.h"
#include "MkCore_MkMultiMap.h"

#include "GameAgentGroupInfo.h"


//------------------------------------------------------------------------------------------------//

void GameAgentGroupInfo::SetUp(const MkArray<GameAgentUnitInfo>& members, unsigned int maxTroopers)
{
	Clear();

	MK_INDEXING_LOOP(members, i)
	{
		const GameAgentUnitInfo& agent = members[i];
		m_Members.Create(agent.GetGameID(), agent);
	}

	m_MaxTroopers = maxTroopers;
	
	SortBy(m_GroupSortingMethod);
	//MkDataNode initNode;
}

void GameAgentGroupInfo::SortBy(eGroupSortingMethod gsm)
{
	m_GroupSortingMethod = gsm;
	m_MemberSequence.Clear();

	if (m_Members.GetSize() == 1)
	{
		m_MemberSequence.PushBack(&m_Members[m_Members.GetFirstKey()]);
	}
	else if (m_Members.GetSize() > 1)
	{
		m_MemberSequence.Reserve(m_Members.GetSize());

		MkMapLooper<unsigned int, GameAgentUnitInfo> looper(m_Members);

		if (m_GroupSortingMethod == eGSM_AgentName)
		{
			MkMultiMap<MkStr, GameAgentUnitInfo*> seq;
			MK_STL_LOOP(looper)
			{
				seq.Create(looper.GetCurrentField().GetName(), &looper.GetCurrentField());
			}

			MkMultiMapLooper<MkStr, GameAgentUnitInfo*> seqLooper(seq);
			MK_STL_LOOP(seqLooper)
			{
				m_MemberSequence.PushBack(seqLooper.GetCurrentField());
			}
		}
		else
		{
			MkMultiMap<int, GameAgentUnitInfo*> seq;
			const int inverter = 0xffff;

			MK_STL_LOOP(looper)
			{
				const GameAgentUnitInfo& agent = looper.GetCurrentField();
				switch (m_GroupSortingMethod)
				{
				case eGSM_AgentLevel:
					seq.Create(inverter - agent.GetAgentLevel(), &looper.GetCurrentField());
					break;
				case eGSM_AttackLevel:
					seq.Create(inverter - agent.GetMemberTypeLevel(eTMT_Attack), &looper.GetCurrentField());
					break;
				case eGSM_DefenseLevel:
					seq.Create(inverter - agent.GetMemberTypeLevel(eTMT_Defense), &looper.GetCurrentField());
					break;
				case eGSM_SupportLevel:
					seq.Create(inverter - agent.GetMemberTypeLevel(eTMT_Support), &looper.GetCurrentField());
					break;
				case eGSM_ResourceLevel:
					seq.Create(inverter - agent.GetMemberTypeLevel(eTMT_Resource), &looper.GetCurrentField());
					break;
				case eGSM_AllRoundLevel:
					seq.Create(inverter - (agent.GetMemberTypeLevel(eTMT_Attack) +
						agent.GetMemberTypeLevel(eTMT_Defense) +
						agent.GetMemberTypeLevel(eTMT_Support) + 
						agent.GetMemberTypeLevel(eTMT_Resource)), &looper.GetCurrentField());
					break;
				}
			}

			MkMultiMapLooper<int, GameAgentUnitInfo*> seqLooper(seq);
			MK_STL_LOOP(seqLooper)
			{
				m_MemberSequence.PushBack(seqLooper.GetCurrentField());
			}
		}
	}
}

void GameAgentGroupInfo::Clear(void)
{
	m_Members.Clear();
	m_MemberSequence.Clear();
}

bool GameAgentGroupInfo::GetTroopers(MkArray<const GameAgentUnitInfo*>& troopers) const
{
	troopers.Reserve(m_MaxTroopers);

	MkConstMapLooper<unsigned int, GameAgentUnitInfo> looper(m_Members);
	MK_STL_LOOP(looper)
	{
		const GameAgentUnitInfo& agent = looper.GetCurrentField();
		if (agent.IsTrooper())
		{
			troopers.PushBack(&agent);
		}
	}
	return (troopers.GetSize() == m_MaxTroopers);
}

GameAgentGroupInfo::GameAgentGroupInfo()
{
	m_GroupSortingMethod = eGSM_AgentLevel;
	m_MaxTroopers = 8;
}

//------------------------------------------------------------------------------------------------//
