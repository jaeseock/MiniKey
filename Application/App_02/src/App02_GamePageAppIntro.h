#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "App02_GamePageBase.h"


class GamePageAppIntro : public GamePageBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageAppIntro(void);
	virtual ~GamePageAppIntro() {}

protected:

	
};
