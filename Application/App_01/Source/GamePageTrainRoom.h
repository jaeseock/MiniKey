#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "GamePageIslandBase.h"


class GamePageTrainRoom : public GamePageIslandBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageTrainRoom(const MkHashStr& name);
	virtual ~GamePageTrainRoom() { Clear(); }
};