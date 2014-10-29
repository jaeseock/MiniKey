
#include "MkCore_MkDataNode.h"

//#include "GameGlobalDefinition.h"
//#include "GameWizardUnitInfo.h"
//#include "GameAgentUnitInfo.h"
//#include "GameDataNode.h"
#include "GamePlayerSystem.h"


template <class DataType>
class __TSI_DataFileOp
{
public:
	static bool Load(const MkDataNode& node, const MkHashStr& key, DataType& instance)
	{
		return (node.ChildExist(key) && instance.Load(*node.GetChildNode(key)));
	}

	static bool Save(MkDataNode& node, const MkHashStr& key, const DataType& instance)
	{
		MkDataNode* targetNode = node.ChildExist(key) ? node.GetChildNode(key) : node.CreateChildNode(key);
		return instance.Save(*targetNode);
	}
};


const static MkHashStr WIZARD_GROUP_KEY = L"WizardGroup";
const static MkHashStr AGENT_GROUP_KEY = L"AgentGroup";


//------------------------------------------------------------------------------------------------//

void GameNormalPlayer::SetUp(const GameWizardUnitInfo& wizard, const MkArray<GameAgentUnitInfo>& agent)
{
	m_WizardInfo = wizard;
	m_AgentInfo = agent;
}

//------------------------------------------------------------------------------------------------//

bool GameMasterPlayer::Load(const MkDataNode& node)
{
	if (!__TSI_DataFileOp<GameWizardGroupInfo>::Load(node, WIZARD_GROUP_KEY, m_WizardGroup))
		return false;

	if (!__TSI_DataFileOp<GameAgentGroupInfo>::Load(node, AGENT_GROUP_KEY, m_AgentGroup))
		return false;

	return true;
}

bool GameMasterPlayer::Save(MkDataNode& node) const
{
	if (!__TSI_DataFileOp<GameWizardGroupInfo>::Save(node, WIZARD_GROUP_KEY, m_WizardGroup))
		return false;

	if (!__TSI_DataFileOp<GameAgentGroupInfo>::Save(node, AGENT_GROUP_KEY, m_AgentGroup))
		return false;

	return true;
}

void GameMasterPlayer::Clear(void)
{
	m_WizardGroup.Clear();
	m_AgentGroup.Clear();
}

//------------------------------------------------------------------------------------------------//
