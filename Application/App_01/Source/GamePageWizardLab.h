#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "GamePageIslandBase.h"


class GamePageWizardLab : public GamePageIslandBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(void);

	GamePageWizardLab(const MkHashStr& name);
	virtual ~GamePageWizardLab() { Clear(); }
};