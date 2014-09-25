#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkSpreadButtonNode.h"


class GameSUI_GlobalSystemCB : public MkSpreadButtonNode
{
public:

	virtual void UseWindowEvent(WindowEvent& evt);

	GameSUI_GlobalSystemCB(void);
	virtual ~GameSUI_GlobalSystemCB() {}
};