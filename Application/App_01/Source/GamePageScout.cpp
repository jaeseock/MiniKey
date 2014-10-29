
#include "MkS2D_MkWindowEventManager.h"

#include "GamePageScout.h"

//------------------------------------------------------------------------------------------------//

class GP_ScoutWindow : public GP_BattleBaseWindow
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

	GP_ScoutWindow(const MkHashStr& name) : GP_BattleBaseWindow(name, L"Scene\\battle_root.msd") {}
	virtual ~GP_ScoutWindow() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageScout::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_ScoutWindow(GetPageName()), true);

	return true;
}

void GamePageScout::Update(const MkTimeState& timeState)
{
	
}

void GamePageScout::Clear(MkDataNode* sharingNode)
{
	GamePageBattleBase::Clear(sharingNode);
}

GamePageScout::GamePageScout(const MkHashStr& name) : GamePageBattleBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
