
#include "MkCore_MkCheck.h"
//#include "MkCore_MkDataNode.h"
//#include "MkCore_MkDevPanel.h"

#include "MkPA_MkRenderer.h"
#include "MkPA_MkDrawSceneNodeStep.h"

#include "App02_ProjectDefinition.h"
#include "App02_GamePageRoot.h"


//------------------------------------------------------------------------------------------------//

bool GamePageRoot::SetUp(MkDataNode& sharingNode)
{
	if (!MkBasePage::SetUp(sharingNode))
		return false;

	MkDrawSceneNodeStep* drawStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(GlobalDef::FinalDrawStepName);
	MK_CHECK(drawStep != NULL, GlobalDef::FinalDrawStepName.GetString() + L" drawStep 생성 실패")
		return false;

	drawStep->SetBackgroundColor(MkColor::Black);

	return true;
}

void GamePageRoot::Update(const MkTimeState& timeState)
{
	MkBasePage::Update(timeState);
}

void GamePageRoot::Clear(MkDataNode* sharingNode)
{
	MK_RENDERER.GetDrawQueue().RemoveStep(GlobalDef::FinalDrawStepName);

	MkBasePage::Clear(sharingNode);
}

GamePageRoot::GamePageRoot(void) : MkBasePage(GamePage::Root::Name)
{
	
}

//------------------------------------------------------------------------------------------------//

