#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"
#include "MkS2D_MkBaseWindowNode.h"


class GP_IslandBaseWindow : public MkBaseWindowNode
{
public:

	GP_IslandBaseWindow(const MkHashStr& name, const MkPathName& filePath);
	virtual ~GP_IslandBaseWindow() {}
};

class GamePageIslandBase : public MkBasePage
{
public:

	virtual void Clear(void);

	GamePageIslandBase(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~GamePageIslandBase() { Clear(); }
};