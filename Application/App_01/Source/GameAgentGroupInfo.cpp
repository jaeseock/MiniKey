
#include "MkCore_MkMultiMap.h"
#include "MkCore_MkDataNode.h"

#include "GameDataNode.h"
#include "GameAgentGroupInfo.h"

const static MkHashStr TROOPERS = L"Troopers";


//------------------------------------------------------------------------------------------------//

bool GameAgentGroupInfo::Load(const MkDataNode& node)
{
	MkArray<MkHashStr> keys;
	node.GetChildNodeList(keys);

	MkArray<GameAgentUnitInfo> members(GDEF_MAX_AGENT_COUNT);
	MK_INDEXING_LOOP(keys, i)
	{
		GameAgentUnitInfo agentUnitInfo;
		if (agentUnitInfo.Load(*node.GetChildNode(keys[i])))
		{
			members.PushBack(agentUnitInfo);
		}
	}

	SetUp(members);

	MkArray<unsigned int> troopers;
	if (!node.GetData(TROOPERS, troopers))
		return false;

	MK_INDEXING_LOOP(troopers, i)
	{
		unsigned int currID = troopers[i];
		if (m_Members.Exist(currID))
		{
			m_Troopers.Create(currID, m_ReadyToBattleAgents.GetSize());
			m_ReadyToBattleAgents.PushBack(m_Members[currID]);
		}
	}

	return !members.Empty();
}

bool GameAgentGroupInfo::Save(MkDataNode& node) const
{
	MkArray<unsigned int> troopers(m_ReadyToBattleAgents.GetSize());
	MK_INDEXING_LOOP(m_ReadyToBattleAgents, i)
	{
		troopers.PushBack(m_ReadyToBattleAgents[i].GetAgentID());
	}
	if (!node.CreateUnit(TROOPERS, troopers))
	{
		node.SetData(TROOPERS, troopers);
	}

	MkConstMapLooper<unsigned int, GameAgentUnitInfo> memberLooper(m_Members);
	MK_STL_LOOP(memberLooper)
	{
		MkHashStr key = MkStr(memberLooper.GetCurrentField().GetAgentID());
		MkDataNode* childNode = node.ChildExist(key) ? node.GetChildNode(key) : node.CreateChildNode(key);
		if (!memberLooper.GetCurrentField().Save(*childNode))
			return false;
	}

	return true;
}

void GameAgentGroupInfo::SetUp(const MkArray<GameAgentUnitInfo>& members)
{
	Clear();

	MK_INDEXING_LOOP(members, i)
	{
		const GameAgentUnitInfo& agent = members[i];
		m_Members.Create(agent.GetAgentID(), agent);
	}
	
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

		if (m_GroupSortingMethod == eGSM_Name)
		{
			MkMap<MkStr, GameAgentUnitInfo*> seq;
			MK_STL_LOOP(looper)
			{
				const MkDataNode* agentData = GameDataNode::AgentSet->GetChildNode(MkStr(looper.GetCurrentField().GetAgentID()));
				if (agentData != NULL)
				{
					//MkStr imagePath;
					//wizardData->GetData(L"ImagePath", imagePath, 0);

					MkStr agentName;
					if (agentData->GetData(L"Name", agentName, 0))
					{
						seq.Create(agentName, &looper.GetCurrentField());
					}
				}
			}

			MkMapLooper<MkStr, GameAgentUnitInfo*> seqLooper(seq);
			MK_STL_LOOP(seqLooper)
			{
				m_MemberSequence.PushBack(seqLooper.GetCurrentField());
			}
		}
		else
		{
			MkMultiMap<int, GameAgentUnitInfo*> seq;
			
			MK_STL_LOOP(looper)
			{
				const GameAgentUnitInfo& agent = looper.GetCurrentField();
				int currKey = -1;
				switch (m_GroupSortingMethod)
				{
				case eGSM_AscendingLevel:
					currKey = agent.GetAgentLevel();
					break;
				case eGSM_DescendingLevel:
					currKey = GDEF_MAX_AGENT_LEVEL - agent.GetAgentLevel();
					break;
				}

				if (currKey >= 0)
				{
					seq.Create(currKey, &looper.GetCurrentField());
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
	m_Troopers.Clear();
	m_ReadyToBattleAgents.Clear();
	m_MemberSequence.Clear();
}

bool GameAgentGroupInfo::SetTrooper(unsigned int agentID, bool enable)
{
	if (m_Members.Exist(agentID))
	{
		if (enable)
		{
			if (!m_Troopers.Exist(agentID))
			{
				if (m_Troopers.GetSize() < GDEF_MAX_TROOP_AGENT_COUNT)
				{
					m_Troopers.Create(agentID, m_ReadyToBattleAgents.GetSize());
					m_ReadyToBattleAgents.PushBack(m_Members[agentID]);
					return true;
				}
			}
		}
		else
		{
			if (m_Troopers.Exist(agentID))
			{
				unsigned int index = m_Troopers[agentID];
				m_ReadyToBattleAgents.Erase(MkArraySection(index, 1));
				m_Troopers.Erase(agentID);
				return true;
			}
		}
	}
	return false;
}

GameAgentGroupInfo::GameAgentGroupInfo()
{
	m_GroupSortingMethod = eGSM_DescendingLevel;
}

//------------------------------------------------------------------------------------------------//
