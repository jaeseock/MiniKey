#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkSpreadButtonNode.h"


class GameSUI_IslandVisitCB : public MkSpreadButtonNode
{
public:

	virtual void UseWindowEvent(WindowEvent& evt);

	GameSUI_IslandVisitCB(void);
	virtual ~GameSUI_IslandVisitCB() {}
};