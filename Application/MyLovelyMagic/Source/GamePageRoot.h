#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class GamePageRoot : public MkBasePage
{
public:

	static const MkHashStr Name;

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(void);

	GamePageRoot();
	virtual ~GamePageRoot() { Clear(); }
};