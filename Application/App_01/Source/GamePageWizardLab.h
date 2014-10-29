#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "GamePageIslandBase.h"


class GamePageWizardLab : public GamePageIslandBase
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageWizardLab(const MkHashStr& name);
	virtual ~GamePageWizardLab() { Clear(); }
};