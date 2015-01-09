
#include "MkCore_MkDataNode.h"
#include "MkS2D_MkWindowEventManager.h"

#include "GamePageBattleBase.h"

//------------------------------------------------------------------------------------------------//

GP_BattleBaseWindow::GP_BattleBaseWindow(const MkHashStr& name, const MkPathName& filePath) : MkBaseWindowNode(name)
{
	MkDataNode node;
	if (node.Load(filePath))
	{
		Load(node);
	}
}

//------------------------------------------------------------------------------------------------//

void GamePageBattleBase::Clear(MkDataNode* sharingNode)
{
	MK_WIN_EVENT_MGR.RemoveWindow(GetPageName());
}

//------------------------------------------------------------------------------------------------//
