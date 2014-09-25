#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class MkSceneNode;

class GamePageClientStart : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(void);

	GamePageClientStart(const MkHashStr& name);
	virtual ~GamePageClientStart() { Clear(); }

protected:

	MkSceneNode* m_SceneNode;
};