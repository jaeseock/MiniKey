
#include "MkCore_MkDataNode.h"
#include "MkS2D_MkWindowEventManager.h"

#include "GameSharedUI.h"
#include "GamePageBattleBase.h"

//------------------------------------------------------------------------------------------------//

GP_BattleBaseWindow::GP_BattleBaseWindow(const MkHashStr& name, const MkPathName& filePath) : MkBaseWindowNode(name)
{
	MkDataNode node;
	if (node.Load(filePath))
	{
		Load(node);

		//AttachChildNode(GameSharedUI::VisitCB);
		AttachChildNode(GameSharedUI::SystemCB);
	}
}

//------------------------------------------------------------------------------------------------//

void GamePageBattleBase::Clear(MkDataNode* sharingNode)
{
	//if (GameSharedUI::VisitCB != NULL)
	//{
	//	GameSharedUI::VisitCB->DetachFromParentNode();
	//}
	if (GameSharedUI::SystemCB != NULL)
	{
		GameSharedUI::SystemCB->DetachFromParentNode();
	}

	MK_WIN_EVENT_MGR.RemoveWindow(GetPageName());
}

//------------------------------------------------------------------------------------------------//
