#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "App02_GamePageBase.h"


class GamePageAppBattleZone : public GamePageBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	//virtual void Update(const MkTimeState& timeState);

	//virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageAppBattleZone(void);
	virtual ~GamePageAppBattleZone() {}

protected:

	
};
