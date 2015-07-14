
#include "MkCore_MkCheck.h"
//#include "MkCore_MkDataNode.h"
//#include "MkCore_MkDevPanel.h"

#include "MkCore_MkTimeState.h"

#include "MkPA_MkRenderer.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkDrawSceneNodeStep.h"

#include "App02_ProjectDefinition.h"
#include "App02_GamePageBase.h"


//------------------------------------------------------------------------------------------------//

bool GamePageBase::SetUp(MkDataNode& sharingNode)
{
	if (!MkBasePage::SetUp(sharingNode))
		return false;

	m_RootSceneNode = new MkSceneNode(L"Root");
	MK_CHECK(m_RootSceneNode != NULL, GetPageName().GetString() + L" game page의 root scene node 생성 실패")
		return false;

	if (m_FinalDrawStep == NULL)
	{
		m_FinalDrawStep = dynamic_cast<MkDrawSceneNodeStep*>(MK_RENDERER.GetDrawQueue().GetStep(GlobalDef::FinalDrawStepName));
		MK_CHECK(m_FinalDrawStep != NULL, GlobalDef::FinalDrawStepName.GetString() + L" drawStep 없음")
			return false;
	}

	m_FinalDrawStep->SetSceneNode(m_RootSceneNode);
	return true;
}

void GamePageBase::Update(const MkTimeState& timeState)
{
	if (m_RootSceneNode != NULL)
	{
		m_RootSceneNode->Update(timeState.fullTime);
	}

	MkBasePage::Update(timeState);
}

void GamePageBase::Clear(MkDataNode* sharingNode)
{
	MK_DELETE(m_RootSceneNode);

	if (m_FinalDrawStep != NULL)
	{
		m_FinalDrawStep->SetSceneNode(NULL);
		m_FinalDrawStep = NULL;
	}

	MkBasePage::Clear(sharingNode);
}

GamePageBase::GamePageBase(const MkHashStr& name) : MkBasePage(name)
{
	m_RootSceneNode = NULL;
	m_FinalDrawStep = NULL;
}

//------------------------------------------------------------------------------------------------//

