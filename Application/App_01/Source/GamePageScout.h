#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "GamePageBattleBase.h"


class GamePageScout : public GamePageBattleBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageScout(const MkHashStr& name);
	virtual ~GamePageScout() { Clear(); }
};