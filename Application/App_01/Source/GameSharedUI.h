#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

class MkBaseWindowNode;

class GameSharedUI
{
public:

	static MkBaseWindowNode* VisitCB;
	static MkBaseWindowNode* SystemCB;

	static void SetUp(void);
	static void Clear(void);
};