
#include "MkS2D_MkWindowEventManager.h"

#include "GamePageWizardLab.h"

//------------------------------------------------------------------------------------------------//

class GP_WizardLabWindow : public GP_IslandBaseWindow
{
public:

	virtual void UseWindowEvent(WindowEvent& evt)
	{
		/*
		if (evt.type == MkSceneNodeFamilyDefinition::eCursorLeftRelease)
		{
			if (evt.node->GetNodeName() == m_NewBtnName)
			{
				//MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
			else if (evt.node->GetNodeName() == m_LoadBtnName)
			{
				//MK_WIN_EVENT_MGR.DeactivateWindow(GetNodeName());
			}
			else if (evt.node->GetNodeName() == m_ExitBtnName)
			{
				DestroyWindow(MK_INPUT_MGR.__GetTargetWindowHandle());
			}
		}
		*/
	}

	GP_WizardLabWindow(const MkHashStr& name) : GP_IslandBaseWindow(name, L"Scene\\wizard_laboratory_page.msd") {}
	virtual ~GP_WizardLabWindow() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageWizardLab::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_WizardLabWindow(GetPageName()), true);

	return true;
}

void GamePageWizardLab::Update(const MkTimeState& timeState)
{
	
}

void GamePageWizardLab::Clear(void)
{
	GamePageIslandBase::Clear();
}

GamePageWizardLab::GamePageWizardLab(const MkHashStr& name) : GamePageIslandBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
