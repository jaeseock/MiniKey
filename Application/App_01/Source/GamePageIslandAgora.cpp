
#include "MkS2D_MkWindowEventManager.h"

#include "GamePageIslandAgora.h"

//------------------------------------------------------------------------------------------------//

class GP_IslandAgoraWindow : public GP_IslandBaseWindow
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

	GP_IslandAgoraWindow(const MkHashStr& name) : GP_IslandBaseWindow(name, L"Scene\\island_lobby_page.msd") {}
	virtual ~GP_IslandAgoraWindow() {}
};

//------------------------------------------------------------------------------------------------//

bool GamePageIslandAgora::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_IslandAgoraWindow(GetPageName()), true);

	return true;
}

void GamePageIslandAgora::Update(const MkTimeState& timeState)
{
	
}

void GamePageIslandAgora::Clear(void)
{
	GamePageIslandBase::Clear();
}

GamePageIslandAgora::GamePageIslandAgora(const MkHashStr& name) : GamePageIslandBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
