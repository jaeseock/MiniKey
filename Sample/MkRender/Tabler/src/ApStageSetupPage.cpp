
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkPageManager.h"

#include "ApStageSetupPage.h"


//------------------------------------------------------------------------------------------------//

bool ApStageSetupPage::SetUp(MkDataNode& sharingNode)
{
	//unsigned int colors = 3;
	//unsigned int simbols = 3;

	m_SetupNode.Clear();
	//m_SetupNode.CreateUnit(L"Colors", colors);
	//m_SetupNode.CreateUnit(L"Simbols", simbols);

	//MK_DEV_PANEL.MsgToLog(L"Colors/Simbols : " + MkStr(colors) + L" * " + MkStr(simbols));

	return true;
}

void ApStageSetupPage::Update(const MkTimeState& timeState)
{
	MK_PAGE_MGR.ChangePageDirectly(L"ApStagePlay");
}

void ApStageSetupPage::Clear(MkDataNode* sharingNode)
{
	if (sharingNode != NULL)
	{
		const MkHashStr& nodeName = m_SetupNode.GetNodeName();
		MkDataNode* node = sharingNode->ChildExist(nodeName) ? sharingNode->GetChildNode(nodeName) : sharingNode->CreateChildNode(nodeName);
		*node = m_SetupNode;
	}
}

ApStageSetupPage::ApStageSetupPage(const MkHashStr& name) : MkBasePage(name)
{
	m_SetupNode.ChangeNodeName(L"StageSetup");
}
