#include "GamePageRoot.h"

//------------------------------------------------------------------------------------------------//

const MkHashStr GamePageRoot::Name(L"Root");


bool GamePageRoot::SetUp(MkDataNode& sharingNode)
{
	return true;
}

void GamePageRoot::Update(const MkTimeState& timeState)
{
	
}

void GamePageRoot::Clear(void)
{
	
}

GamePageRoot::GamePageRoot() : MkBasePage(Name)
{
	
}

//------------------------------------------------------------------------------------------------//
