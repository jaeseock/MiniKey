#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class MkSceneNode;

class GamePageMainTitle : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageMainTitle(const MkHashStr& name);
	virtual ~GamePageMainTitle() { Clear(); }

protected:

	MkSceneNode* m_SceneNode;
};