
#include "MkCore_MkDataNode.h"

#include "GameDataNode.h"

//------------------------------------------------------------------------------------------------//

MkDataNode* GameDataNode::WizardSet = NULL;
MkDataNode* GameDataNode::AgentSet = NULL;

void GameDataNode::SetUp(void)
{
	Clear();

	WizardSet = new MkDataNode(L"WizardSet");
	WizardSet->Load(L"DataNode\\WizardSet.txt");

	AgentSet = new MkDataNode(L"AgentSet");
	AgentSet->Load(L"DataNode\\AgentSet.txt");
}

void GameDataNode::Clear(void)
{
	MK_DELETE(WizardSet);
	MK_DELETE(AgentSet);
}

//------------------------------------------------------------------------------------------------//
