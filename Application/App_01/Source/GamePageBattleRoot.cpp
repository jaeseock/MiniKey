
#include "MkS2D_MkTexturePool.h"

#include "GamePageBattleRoot.h"

//------------------------------------------------------------------------------------------------//

bool GamePageBattleRoot::SetUp(MkDataNode& sharingNode)
{
	return true;
}

void GamePageBattleRoot::Update(const MkTimeState& timeState)
{
	
}

void GamePageBattleRoot::Clear(MkDataNode* sharingNode)
{
	// battle ���� ���ҽ� ����
	MK_TEXTURE_POOL.UnloadGroup(1);
}

GamePageBattleRoot::GamePageBattleRoot(const MkHashStr& name) : MkBasePage(name)
{
	
}

//------------------------------------------------------------------------------------------------//
