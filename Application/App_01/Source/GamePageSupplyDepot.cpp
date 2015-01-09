
#include "MkCore_MkDataNode.h"
#include "MkCore_MkPageManager.h"

#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkWindowEventManager.h"

#include "GameGlobalDefinition.h"
//#include "GameSystemManager.h"
#include "GameSharedUI.h"
#include "GamePageSupplyDepot.h"

//------------------------------------------------------------------------------------------------//

class GP_SupplyDepotWindow : public MkBaseWindowNode
{
public:

	virtual void UseWindowEvent(WindowEvent& evt)
	{
		if (evt.type == MkSceneNodeFamilyDefinition::eCursorLeftRelease)
		{
			if (evt.node->GetNodeName() == m_StartBtnKey)
			{
				MK_PAGE_MGR.ChangePageDirectly(GamePageName::BattlePhase);
			}
			else if (evt.node->GetNodeName() == m_BackBtnKey)
			{
				MK_PAGE_MGR.ChangePageDirectly(GamePageName::IslandAgora);
			}
		}
	}

	GP_SupplyDepotWindow(const MkHashStr& name) : MkBaseWindowNode(name)
	{
		MkDataNode node;
		if (node.Load(L"Scene\\supply_depot_page.msd"))
		{
			Load(node);

			AttachChildNode(GameSharedUI::SystemCB);

			m_StartBtnKey = L"Start";
			m_BackBtnKey = L"Back";
		}
	}

	virtual ~GP_SupplyDepotWindow() {}

protected:

	MkHashStr m_StartBtnKey;
	MkHashStr m_BackBtnKey;
};

//------------------------------------------------------------------------------------------------//

bool GamePageSupplyDepot::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_SupplyDepotWindow(GetPageName()), true);

	return true;
}

void GamePageSupplyDepot::Update(const MkTimeState& timeState)
{
	
}

void GamePageSupplyDepot::Clear(MkDataNode* sharingNode)
{
	if (GameSharedUI::SystemCB != NULL)
	{
		GameSharedUI::SystemCB->DetachFromParentNode();
	}

	MK_WIN_EVENT_MGR.RemoveWindow(GetPageName());
}

GamePageSupplyDepot::GamePageSupplyDepot(const MkHashStr& name) : MkBasePage(name)
{
	
}

//------------------------------------------------------------------------------------------------//
