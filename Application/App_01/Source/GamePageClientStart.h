#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class MkSceneNode;

class GamePageClientStart : public MkBasePage
{
public:

	static const MkHashStr Name;

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(void);

	GamePageClientStart();
	virtual ~GamePageClientStart() { Clear(); }

protected:

	MkSceneNode* m_SceneNode;
};