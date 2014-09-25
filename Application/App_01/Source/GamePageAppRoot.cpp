
#include "MkCore_MkPathName.h"

#include "MkS2D_MkTexturePool.h"

#include "GamePageAppRoot.h"

//------------------------------------------------------------------------------------------------//

bool GamePageAppRoot::SetUp(MkDataNode& sharingNode)
{
	// �⺻ ��� �̹��� preloading
	MK_TEXTURE_POOL.LoadBitmapTexture(L"Scene\\main_bg.dds");
	return true;
}

void GamePageAppRoot::Update(const MkTimeState& timeState)
{
	
}

void GamePageAppRoot::Clear(void)
{
	
}

GamePageAppRoot::GamePageAppRoot(const MkHashStr& name) : MkBasePage(name)
{
	
}

//------------------------------------------------------------------------------------------------//
