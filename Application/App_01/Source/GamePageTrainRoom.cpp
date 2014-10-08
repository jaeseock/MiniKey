
#include "MkS2D_MkWindowEventManager.h"

#include "GamePageTrainRoom.h"

//------------------------------------------------------------------------------------------------//

class GP_TrainRoomWindow : public GP_IslandBaseWindow
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

	GP_TrainRoomWindow(const MkHashStr& name) : GP_IslandBaseWindow(name, L"Scene\\barrack_page.msd") {}
	virtual ~GP_TrainRoomWindow() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageTrainRoom::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_TrainRoomWindow(GetPageName()), true);

	return true;
}

void GamePageTrainRoom::Update(const MkTimeState& timeState)
{
	
}

void GamePageTrainRoom::Clear(void)
{
	GamePageIslandBase::Clear();
}

GamePageTrainRoom::GamePageTrainRoom(const MkHashStr& name) : GamePageIslandBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
