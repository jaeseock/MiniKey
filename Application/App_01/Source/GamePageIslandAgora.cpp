
#include "MkCore_MkPageManager.h"

#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkSpreadButtonNode.h"

#include "GameGlobalDefinition.h"
#include "GamePageIslandAgora.h"

//------------------------------------------------------------------------------------------------//

class GP_IslandAgoraWindow : public GP_IslandBaseWindow
{
public:

	virtual void UseWindowEvent(WindowEvent& evt)
	{
		if (evt.type == MkSceneNodeFamilyDefinition::eSetTargetItem)
		{
			MkSpreadButtonNode* cbNode = dynamic_cast<MkSpreadButtonNode*>(evt.node);
			if (cbNode != NULL)
			{
				if (cbNode->GetTargetItemKey() == m_ScoutBtnKey)
				{
					MK_PAGE_MGR.ChangePageDirectly(GamePageName::SupplyDepot);
				}
			}
		}
	}

	GP_IslandAgoraWindow(const MkHashStr& name) : GP_IslandBaseWindow(name, L"Scene\\island_lobby_page.msd")
	{
		m_ScoutBtnKey = L"Scout";
	}
	virtual ~GP_IslandAgoraWindow() {}

protected:

	MkHashStr m_ScoutBtnKey;
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

void GamePageIslandAgora::Clear(MkDataNode* sharingNode)
{
	GamePageIslandBase::Clear(sharingNode);
}

GamePageIslandAgora::GamePageIslandAgora(const MkHashStr& name) : GamePageIslandBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
