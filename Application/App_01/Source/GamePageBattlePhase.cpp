
#include "MkS2D_MkWindowEventManager.h"

#include "GamePageBattlePhase.h"

//------------------------------------------------------------------------------------------------//

class GP_BattlePhaseWindow : public GP_BattleBaseWindow
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

	GP_BattlePhaseWindow(const MkHashStr& name) : GP_BattleBaseWindow(name, L"Scene\\battle_root.msd") {}
	virtual ~GP_BattlePhaseWindow() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageBattlePhase::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_BattlePhaseWindow(GetPageName()), false);

	return true;
}

void GamePageBattlePhase::Update(const MkTimeState& timeState)
{
	
}

void GamePageBattlePhase::Clear(MkDataNode* sharingNode)
{
	GamePageBattleBase::Clear(sharingNode);
}

GamePageBattlePhase::GamePageBattlePhase(const MkHashStr& name) : GamePageBattleBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
