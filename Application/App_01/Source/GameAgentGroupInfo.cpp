
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

	unsigned int currTroopers = troopers.GetSize();
	if (currTroopers > m_MaxTroopers)
	{
		troopers.PopBack(currTroopers - m_MaxTroopers);
	}

	MK_INDEXING_LOOP(troopers, i)
	{
		unsigned int currID = troopers[i];
		if (m_Members.Exist(currID))
		{
			m_Troopers.Create(currID, &m_Members[currID]);
		}
	}

	return !members.Empty();
}

bool GameAgentGroupInfo::Save(MkDataNode& node) const
{
	MkArray<unsigned int> troopers(m_Troopers.GetSize());
	MkConstMapLooper<unsigned int, GameAgentUnitInfo*> trooperLooper(m_Troopers);
	MK_STL_LOOP(trooperLooper)
	{
		troopers.PushBack(trooperLooper.GetCurrentKey());
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
				if (m_Troopers.GetSize() < m_MaxTroopers)
				{
					m_Troopers.Create(agentID, &m_Members[agentID]);
					return true;
				}
			}
		}
		else
		{
			if (m_Troopers.Exist(agentID))
			{
				m_Troopers.Erase(agentID);
				return true;
			}
		}
	}
	return false;
}

void GameAgentGroupInfo::GetTroopers(MkArray<GameAgentUnitInfo>& buffer) const
{
	buffer.Reserve(m_MaxTroopers);

	// eGSM_DescendingLevel로 정렬된 순서대로 끊어서 troopers에 추가
	MkMultiMap<int, GameAgentUnitInfo*> seq;
	MkConstMapLooper<unsigned int, GameAgentUnitInfo*> looper(m_Troopers);
	MK_STL_LOOP(looper)
	{
		seq.Create(GDEF_MAX_AGENT_LEVEL - looper.GetCurrentField()->GetAgentLevel(), looper.GetCurrentField());
	}

	MkMultiMapLooper<int, GameAgentUnitInfo*> seqLooper(seq);
	MK_STL_LOOP(seqLooper)
	{
		if (buffer.GetSize() < m_MaxTroopers)
		{
			buffer.PushBack(*seqLooper.GetCurrentField());
		}
		else
			break;
	}
}

GameAgentGroupInfo::GameAgentGroupInfo()
{
	m_GroupSortingMethod = eGSM_DescendingLevel;
	m_MaxTroopers = 8;
}

//------------------------------------------------------------------------------------------------//
