
#include "MkCore_MkDataNode.h"

#include "GameWizardGroupInfo.h"


const static MkHashStr TARGET_WIZARD = L"TargetWizard";

//------------------------------------------------------------------------------------------------//

bool GameWizardGroupInfo::Load(const MkDataNode& node)
{
	unsigned int targetWizardID = 0;
	if (!node.GetData(TARGET_WIZARD, targetWizardID, 0))
		return false;

	MkArray<MkHashStr> keys;
	node.GetChildNodeList(keys);

	MkArray<GameWizardUnitInfo> members(10);
	MK_INDEXING_LOOP(keys, i)
	{
		GameWizardUnitInfo wizardUnitInfo;
		if (wizardUnitInfo.Load(*node.GetChildNode(keys[i])))
		{
			members.PushBack(wizardUnitInfo);
		}
	}

	SetUp(members, targetWizardID);

	return !members.Empty();
}

bool GameWizardGroupInfo::Save(MkDataNode& node)
{
	if (!node.CreateUnit(TARGET_WIZARD, m_TargetWizardID))
	{
		node.SetData(TARGET_WIZARD, m_TargetWizardID, 0);
	}

	MkMapLooper<unsigned int, GameWizardUnitInfo> looper(m_Members);
	MK_STL_LOOP(looper)
	{
		MkHashStr key = MkStr(looper.GetCurrentField().GetWizardID());
		MkDataNode* childNode = node.ChildExist(key) ? node.GetChildNode(key) : node.CreateChildNode(key);
		if (!looper.GetCurrentField().Save(*childNode))
			return false;
	}

	return true;
}

void GameWizardGroupInfo::SetUp(const MkArray<GameWizardUnitInfo>& members, unsigned int targetWizardID)
{
	Clear();

	MK_INDEXING_LOOP(members, i)
	{
		const GameWizardUnitInfo& wizard = members[i];
		m_Members.Create(wizard.GetWizardID(), wizard);
	}

	m_TargetWizardID = targetWizardID;
}

void GameWizardGroupInfo::Clear(void)
{
	m_Members.Clear();
}

GameWizardGroupInfo::GameWizardGroupInfo()
{
	m_TargetWizardID = 0;
}

//------------------------------------------------------------------------------------------------//
