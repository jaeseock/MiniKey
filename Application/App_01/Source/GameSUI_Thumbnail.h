#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


//------------------------------------------------------------------------------------------------//

class GameSUI_WizardThumbnail : public MkBaseWindowNode
{
public:

	bool CreateWizardThumbnail(const MkPathName& iconPath, const MkHashStr& iconSubset, int initLevel, const MkStr& name);

	MkSRect* SetWizardLevel(int level);

	GameSUI_WizardThumbnail(const MkHashStr& name) : MkBaseWindowNode(name) {}
	virtual ~GameSUI_WizardThumbnail() {}
};

//------------------------------------------------------------------------------------------------//

class GameSUI_SmallAgentThumbnail : public MkBaseWindowNode
{
public:

	bool CreateSmallAgentThumbnail(unsigned int agentID, int initLevel, bool toBattle);

	void SetAgentLevel(int level);
	void SetToBattle(bool enable);

	GameSUI_SmallAgentThumbnail(const MkHashStr& name) : MkBaseWindowNode(name) {}
	virtual ~GameSUI_SmallAgentThumbnail() {}
};

//------------------------------------------------------------------------------------------------//

class GameSUI_SmallAgentView : public MkSceneNode
{
public:

	bool CreateSmallAgentView(unsigned int agentID);

	GameSUI_SmallAgentView(const MkHashStr& name) : MkSceneNode(name) {}
	virtual ~GameSUI_SmallAgentView() {}
};

//------------------------------------------------------------------------------------------------//