#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class GamePageAppRoot : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageAppRoot(const MkHashStr& name);
	virtual ~GamePageAppRoot() { Clear(); }
};