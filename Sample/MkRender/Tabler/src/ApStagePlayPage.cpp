
#include "MkCore_MkDataNode.h"
#include "MkCore_MkPageManager.h"
#include "MkCore_MkTimeState.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkInputManager.h"

#include "MkPA_MkRenderer.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkDrawSceneNodeStep.h"

#include "ApStagePlayPage.h"


//------------------------------------------------------------------------------------------------//

bool ApStagePlayPage::SetUp(MkDataNode& sharingNode)
{
	//unsigned int colors = 3;
	//unsigned int simbols = 3;

	MkDataNode* stageSetUpNode = sharingNode.GetChildNode(L"StageSetup");
	if (stageSetUpNode != NULL)
	{
		MkPathName tmpSetupFilePath;
		tmpSetupFilePath.ConvertToRootBasisAbsolutePath(L"StageSetup.txt");
		if (tmpSetupFilePath.CheckAvailable())
		{
			MkDataNode tmpNode;
			if (tmpNode.Load(tmpSetupFilePath))
			{
				*stageSetUpNode = tmpNode;
			}
		}
	}
	
	//node->GetData(L"Colors", colors, 0);
	//node->GetData(L"Simbols", simbols, 0);

	// root
	m_SceneRootNode = new MkSceneNode(L"<Root>");

	// field
	m_StageMgr.SetField(MkUInt2(5, 5), m_SceneRootNode);

	// player - me
	m_StageMgr.AddPlayer(L"나에용", ApPlayer::eSP_Down, ApResourceUnit::eTC_Red, 12);

	// player - enemy
	m_StageMgr.AddPlayer(L"상대 1", ApPlayer::eSP_Left, ApResourceUnit::eTC_Green, 12);

	// player - enemy
	m_StageMgr.AddPlayer(L"상대 2", ApPlayer::eSP_Top, ApResourceUnit::eTC_Yellow, 12);

	// player - enemy
	m_StageMgr.AddPlayer(L"상대 3", ApPlayer::eSP_Right, ApResourceUnit::eTC_Cyan, 12);

	// flow
	m_StageMgr.SetFlowInfo(5, 10);

	// draw step
	MkDrawSceneNodeStep* drawStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"Scene");
	drawStep->SetSceneNode(m_SceneRootNode);

	// finish
	m_SceneRootNode->Update();

	return true;
}

void ApStagePlayPage::Update(const MkTimeState& timeState)
{
	// 1st msg(header)
	{
		MkStr msg;
		msg.Reserve(512);

		msg += L"커서 : ";
		msg += MK_INPUT_MGR.GetAbsoluteMousePosition(true);

		MK_DEV_PANEL.MsgToFreeboard(0, msg);
	}

	m_StageMgr.Update(timeState);

	if (m_SceneRootNode != NULL)
	{
		m_SceneRootNode->Update(timeState.fullTime);
	}
}

void ApStagePlayPage::Clear(MkDataNode* sharingNode)
{
	MK_DELETE(m_SceneRootNode);
	MK_RENDERER.GetDrawQueue().Clear();
}

ApStagePlayPage::ApStagePlayPage(const MkHashStr& name) : MkBasePage(name)
{
	m_SceneRootNode = NULL;
}
