#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class GamePageAppRoot : public MkBasePage
{
public:

	static const MkHashStr Name;

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(void);

	GamePageAppRoot();
	virtual ~GamePageAppRoot() { Clear(); }
};