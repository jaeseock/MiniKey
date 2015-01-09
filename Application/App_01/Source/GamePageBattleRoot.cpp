#include "MkCore_MkDataNode.h"

#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkBaseWindowNode.h"

#include "MkS2D_MkWindowEventManager.h"

#include "GameSharedUI.h"
#include "GamePageBattleRoot.h"


//------------------------------------------------------------------------------------------------//

class GP_BattleRootWindow : public MkBaseWindowNode
{
public:

	GP_BattleRootWindow(const MkHashStr& name) : MkBaseWindowNode(name)
	{
		MkDataNode node;
		if (node.Load(L"Scene\\battle_root.msd"))
		{
			Load(node);

			AttachChildNode(GameSharedUI::SystemCB);
		}
	}

	virtual ~GP_BattleRootWindow() {}
};


//------------------------------------------------------------------------------------------------//

bool GamePageBattleRoot::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_BattleRootWindow(GetPageName()), true);

	return true;
}

void GamePageBattleRoot::Update(const MkTimeState& timeState)
{
	
}

void GamePageBattleRoot::Clear(MkDataNode* sharingNode)
{
	if (GameSharedUI::SystemCB != NULL)
	{
		GameSharedUI::SystemCB->DetachFromParentNode();
	}

	MK_WIN_EVENT_MGR.RemoveWindow(GetPageName());

	// battle 전용 리소스 삭제
	MK_TEXTURE_POOL.UnloadGroup(1);
}

GamePageBattleRoot::GamePageBattleRoot(const MkHashStr& name) : MkBasePage(name)
{
	
}

//------------------------------------------------------------------------------------------------//
