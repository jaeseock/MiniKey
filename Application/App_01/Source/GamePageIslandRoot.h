#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class GamePageIslandRoot : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageIslandRoot(const MkHashStr& name);
	virtual ~GamePageIslandRoot() { Clear(); }

protected:
};