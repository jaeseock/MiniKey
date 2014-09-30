#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


//------------------------------------------------------------------------------------------------//

class GameSUI_ThumbnailWizard : public MkBaseWindowNode
{
public:

	bool CreateThumbnailWizard(const MkPathName& iconPath, const MkHashStr& iconSubset, int initLevel, const MkStr& name);

	MkSRect* SetWizardLevel(int level);

	GameSUI_ThumbnailWizard(const MkHashStr& name) : MkBaseWindowNode(name) {}
	virtual ~GameSUI_ThumbnailWizard() {}
};

//------------------------------------------------------------------------------------------------//