
//#include "MkCore_MkGlobalFunction.h"
//#include "MkCore_MkCheck.h"
//#include "MkCore_MkTimeManager.h"

#include "MkCore_MkDataNode.h"

#include "GameSystemManager.h"


//------------------------------------------------------------------------------------------------//

void NormalPlayer::SetUp(const GameWizardUnitInfo& wizard, const MkArray<GameAgentUnitInfo>& troop)
{
	m_WizardInfo = wizard;
	m_TroopInfo = troop;
}

//------------------------------------------------------------------------------------------------//

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

bool MasterPlayer::Load(const MkDataNode& node)
{
	if (!__TSI_DataFileOp<GameWizardGroupInfo>::Load(node, WIZARD_GROUP_KEY, m_WizardGroup))
		return false;

	if (!__TSI_DataFileOp<GameAgentGroupInfo>::Load(node, AGENT_GROUP_KEY, m_AgentGroup))
		return false;

	return true;
}

bool MasterPlayer::Save(MkDataNode& node) const
{
	if (!__TSI_DataFileOp<GameWizardGroupInfo>::Save(node, WIZARD_GROUP_KEY, m_WizardGroup))
		return false;

	if (!__TSI_DataFileOp<GameAgentGroupInfo>::Save(node, AGENT_GROUP_KEY, m_AgentGroup))
		return false;

	return true;
}

void MasterPlayer::Clear(void)
{
	m_WizardGroup.Clear();
	m_AgentGroup.Clear();
}

//------------------------------------------------------------------------------------------------//

bool GameSystemManager::LoadMasterUserData(const MkPathName& filePath)
{
	MkDataNode userData;
	return (userData.Load(filePath) && GetMasterPlayer().Load(userData));
}

bool GameSystemManager::SaveMasterUserData(const MkPathName& filePath) const
{
	MkDataNode userData;
	if (GetMasterPlayer().Save(userData))
	{
		if (userData.SaveToText(filePath))
			return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
