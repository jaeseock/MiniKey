#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"
#include "MkS2D_MkBaseWindowNode.h"


class GP_BattleBaseWindow : public MkBaseWindowNode
{
public:

	GP_BattleBaseWindow(const MkHashStr& name, const MkPathName& filePath);
	virtual ~GP_BattleBaseWindow() {}
};

class GamePageBattleBase : public MkBasePage
{
public:

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageBattleBase(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~GamePageBattleBase() { Clear(); }
};