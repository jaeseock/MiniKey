#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"


class MkSceneNode;
class MkDrawSceneNodeStep;

class GamePageBase : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	GamePageBase(const MkHashStr& name);
	virtual ~GamePageBase() {}

protected:

	MkSceneNode* m_RootSceneNode;
	MkDrawSceneNodeStep* m_FinalDrawStep;
};
