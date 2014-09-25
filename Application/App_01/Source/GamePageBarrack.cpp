
#include "MkS2D_MkWindowEventManager.h"

#include "GamePageBarrack.h"

//------------------------------------------------------------------------------------------------//

class GP_BarrackWindow : public GP_IslandBaseWindow
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

	GP_BarrackWindow(const MkHashStr& name) : GP_IslandBaseWindow(name, L"Scene\\barrack_page.msd") {}
	virtual ~GP_BarrackWindow() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageBarrack::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_BarrackWindow(GetPageName()), true);

	return true;
}

void GamePageBarrack::Update(const MkTimeState& timeState)
{
	
}

void GamePageBarrack::Clear(void)
{
	GamePageIslandBase::Clear();
}

GamePageBarrack::GamePageBarrack(const MkHashStr& name) : GamePageIslandBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
