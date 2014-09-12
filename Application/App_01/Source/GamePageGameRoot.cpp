
//#include "MkCore_MkPathName.h"
#include "MkCore_MkDataNode.h"

#include "GamePageGameRoot.h"

//------------------------------------------------------------------------------------------------//

const MkHashStr GamePageGameRoot::Name(L"GameRoot");


bool GamePageGameRoot::SetUp(MkDataNode& sharingNode)
{
	// 게임 공용 리소스 로딩
	

	return true;
}

void GamePageGameRoot::Update(const MkTimeState& timeState)
{
	
}

void GamePageGameRoot::Clear(void)
{
	
}

GamePageGameRoot::GamePageGameRoot() : MkBasePage(Name)
{
	
}

//------------------------------------------------------------------------------------------------//
