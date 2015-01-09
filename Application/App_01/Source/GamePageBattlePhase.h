#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "GamePageBattleBase.h"


class GamePageBattlePhase : public GamePageBattleBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageBattlePhase(const MkHashStr& name);
	virtual ~GamePageBattlePhase() { Clear(); }
};