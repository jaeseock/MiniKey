#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "App02_GamePageBase.h"


class GamePageAppBriefingRoom : public GamePageBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	//virtual void Update(const MkTimeState& timeState);

	//virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageAppBriefingRoom(void);
	virtual ~GamePageAppBriefingRoom() {}

protected:

	
};
