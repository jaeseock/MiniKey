
//#include "MkCore_MkCheck.h"
//#include "MkCore_MkDataNode.h"
//#include "MkCore_MkDevPanel.h"

//#include "MkCore_MkTimeState.h"

//#include "MkPA_MkRenderer.h"
#include "MkPA_MkProgressBarNode.h"

//#include "MkPA_MkDrawSceneNodeStep.h"

#include "App02_ProjectDefinition.h"
#include "App02_GamePageAppStart.h"


//------------------------------------------------------------------------------------------------//

bool GamePageAppStart::SetUp(MkDataNode& sharingNode)
{
	if (!GamePageBase::SetUp(sharingNode))
		return false;

	m_RootSceneNode->SetLocalDepth(10.f);

	// progress
	m_TotalResCount = 100;
	_SetLoadedResCount(0);

	// welcome msg
	MkPanel& msgPanel = m_RootSceneNode->CreatePanel(L"msg");

	MkArray<MkHashStr> msgTN;
	msgTN.PushBack(L"AppStartMsg");
	msgPanel.SetTextNode(msgTN);

	MkFloat2 msgPos = (m_ProgressBarNode == NULL) ? MkFloat2::Zero : m_ProgressBarNode->GetLocalPosition();
	msgPos.y += 50.f;
	msgPanel.SetLocalPosition(msgPos);

	msgPanel.SetLocalDepth(-1.f);
	
	return true;
}

void GamePageAppStart::Update(const MkTimeState& timeState)
{
	switch (m_PageState)
	{
	case ePS_ShowInitMsg:
		m_PageState = ePS_LoadGlobalResource;
		break;

	case ePS_LoadGlobalResource:
		{
			// tmp
			int count = m_LoadedResCount + 1;
			//

			_SetLoadedResCount(count);
		}
		break;

	case ePS_MoveToNextPage:
		SetMoveMessage(GamePage::AppStart::Condition::Next);
		break;
	}

	GamePageBase::Update(timeState);
}

void GamePageAppStart::Clear(MkDataNode* sharingNode)
{
	GamePageBase::Clear(sharingNode);
}

GamePageAppStart::GamePageAppStart(void) : GamePageBase(GamePage::AppStart::Name)
{
	m_PageState = ePS_ShowInitMsg;

	m_LoadedResCount = 0;
	m_TotalResCount = 0;

	m_ProgressBarNode = NULL;

	// page flow
	SetPageFlowTable(GamePage::AppStart::Condition::Next, GamePage::AppIntro::Name);
}

void GamePageAppStart::_SetLoadedResCount(int count)
{
	// apply
	m_LoadedResCount = count;

	// update tag
	if (m_RootSceneNode != NULL)
	{
		static const MkHashStr ProgressBarName(L"Prog");

		if (m_ProgressBarNode == NULL)
		{
			m_ProgressBarNode = MkProgressBarNode::CreateChildNode(m_RootSceneNode, ProgressBarName);
			if (m_ProgressBarNode != NULL)
			{
				m_ProgressBarNode->SetProgressBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Medium, 900.f, m_LoadedResCount, m_TotalResCount, MkProgressBarNode::eSPM_Percentage);
				float barWidth = m_ProgressBarNode->CalculateWindowSize().x;
				m_ProgressBarNode->SetLocalPosition(MkFloat2((GlobalDef::AppResolution.x - barWidth) * 0.5f, 50.f));
				m_ProgressBarNode->SetLocalDepth(-1.f);
			}
		}

		if (m_ProgressBarNode != NULL)
		{
			m_ProgressBarNode->SetCurrentValue(m_LoadedResCount);
		}
	}

	// check to next
	if (m_LoadedResCount >= m_TotalResCount)
	{
		m_PageState = ePS_MoveToNextPage;
	}
}

//------------------------------------------------------------------------------------------------//

