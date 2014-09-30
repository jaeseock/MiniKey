
#include "MkCore_MkDataNode.h"

#include "GameDataNode.h"

//------------------------------------------------------------------------------------------------//

MkDataNode* GameDataNode::WizardSet = NULL;

void GameDataNode::SetUp(void)
{
	Clear();

	WizardSet = new MkDataNode(L"WizardSet");
	WizardSet->Load(L"DataNode\\WizardSet.txt");
}

void GameDataNode::Clear(void)
{
	MK_DELETE(WizardSet);
}

//------------------------------------------------------------------------------------------------//
