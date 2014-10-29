
#include "MkS2D_MkTexturePool.h"

#include "GamePageIslandRoot.h"

//------------------------------------------------------------------------------------------------//

bool GamePageIslandRoot::SetUp(MkDataNode& sharingNode)
{
	return true;
}

void GamePageIslandRoot::Update(const MkTimeState& timeState)
{
	
}

void GamePageIslandRoot::Clear(MkDataNode* sharingNode)
{
	// island 전용 리소스 삭제
	MK_TEXTURE_POOL.UnloadGroup(1);
}

GamePageIslandRoot::GamePageIslandRoot(const MkHashStr& name) : MkBasePage(name)
{
	
}

//------------------------------------------------------------------------------------------------//
