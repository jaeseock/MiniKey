#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "GamePageIslandBase.h"


class GamePageBarrack : public GamePageIslandBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageBarrack(const MkHashStr& name);
	virtual ~GamePageBarrack() { Clear(); }
};